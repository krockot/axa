#include "axa/service/mini_chrome.h"

#include "axa/mojom/mini_chrome.mojom.h"
#include "axa/service/native_bridge.h"
#include "base/bind.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/no_destructor.h"
#include "base/optional.h"
#include "base/threading/thread.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/system/data_pipe_utils.h"
#include "services/network/network_service.h"
#include "services/network/test/test_network_service_client.h"
#include "services/network/test/test_network_context_client.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/public/mojom/network_service.mojom.h"

namespace {

base::Thread& GetNetworkThread() {
  static base::NoDestructor<base::Thread> thread{"network"};
  return *thread;
}

void InitNetworkService(
    mojo::PendingReceiver<network::mojom::NetworkService> receiver) {
  using ReceiverType = decltype(receiver);
  GetNetworkThread().StartWithOptions({base::MessagePumpType::IO, 0});
  GetNetworkThread().task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce([](ReceiverType receiver) {
        static base::NoDestructor<network::NetworkService> service{
            nullptr, std::move(receiver)};
      },
      std::move(receiver)));
}

class MiniChromeImpl
    : public mini::mojom::MiniChrome,
      public mini::mojom::Notifier {
 public:
  MiniChromeImpl() {
    InitNetworkService(network_service_remote_.BindNewPipeAndPassReceiver());
    network::mojom::NetworkServiceClientPtr client;
    service_client_.emplace(mojo::MakeRequest(&client));
    network_service_remote_->SetClient(
        std::move(client), network::mojom::NetworkServiceParams::New());
    auto context_params = network::mojom::NetworkContextParams::New();
    context_params->user_agent = "a very small browser";
    network_service_remote_->CreateNetworkContext(
        network_context_.BindNewPipeAndPassReceiver(),
        std::move(context_params));
    network::mojom::NetworkContextClientPtr context_client;
    context_client_receiver_.Bind(mojo::MakeRequest(&context_client));
    network_context_->SetClient(std::move(context_client));
  }

  ~MiniChromeImpl() override = default;

  void Bind(mojo::PendingReceiver<mini::mojom::MiniChrome> receiver) {
    receivers_.Add(this, std::move(receiver));
  }

  // mini::mojom::MiniChrome implementation:
  void BindNotifier(
      mojo::PendingReceiver<mini::mojom::Notifier> receiver) override {
    notifier_receivers_.Add(this, std::move(receiver));
  }

  void BindUrlLoaderFactory(
      mojo::PendingReceiver<network::mojom::URLLoaderFactory> receiver)
      override {
    auto params = network::mojom::URLLoaderFactoryParams::New();
    params->process_id = network::mojom::kBrowserProcessId;
    params->is_corb_enabled = false;
    network_context_->CreateURLLoaderFactory(std::move(receiver),
                                             std::move(params));
  }

  // mini::mojom::Notifier implementation:
  void Notify(mojo::ScopedDataPipeConsumerHandle consumer) override {
    std::string text;
    CHECK(mojo::BlockingCopyToString(std::move(consumer), &text));
    DoNotify(text);
  }

 private:
  mojo::ReceiverSet<mini::mojom::MiniChrome> receivers_;
  mojo::ReceiverSet<mini::mojom::Notifier> notifier_receivers_;
  mojo::Remote<network::mojom::NetworkService> network_service_remote_;
  base::Optional<network::TestNetworkServiceClient> service_client_;
  mojo::Remote<network::mojom::NetworkContext> network_context_;
  network::TestNetworkContextClient context_client_;
  mojo::Receiver<network::mojom::NetworkContextClient>
      context_client_receiver_{&context_client_};

  DISALLOW_COPY_AND_ASSIGN(MiniChromeImpl);
};

}  // namespace

void BindMiniChrome(mojo::ScopedMessagePipeHandle pipe) {
  static base::NoDestructor<MiniChromeImpl> impl;
  impl->Bind(mojo::PendingReceiver<mini::mojom::MiniChrome>(std::move(pipe)));
}

