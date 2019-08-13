#include "axa/service/mini_chrome.h"

#include "axa/mojom/mini_chrome.mojom.h"
#include "axa/service/native_bridge.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/no_destructor.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver_set.h"

namespace {

class MiniChromeImpl
    : public mini::mojom::MiniChrome,
      public mini::mojom::Notifier {
 public:
  MiniChromeImpl() = default;
  ~MiniChromeImpl() override = default;

  void Bind(mojo::PendingReceiver<mini::mojom::MiniChrome> receiver) {
    receivers_.Add(this, std::move(receiver));
  }

  // mini::mojom::MiniChrome implementation:
  void BindNotifier(
      mojo::PendingReceiver<mini::mojom::Notifier> receiver) override {
    notifier_receivers_.Add(this, std::move(receiver));
  }

  // mini::mojom::Notifier implementation:
  void Notify(const std::string& text) override {
    DoNotify(text);
  }

 private:
  mojo::ReceiverSet<mini::mojom::MiniChrome> receivers_;
  mojo::ReceiverSet<mini::mojom::Notifier> notifier_receivers_;
  DISALLOW_COPY_AND_ASSIGN(MiniChromeImpl);
};

}  // namespace

void BindMiniChrome(mojo::ScopedMessagePipeHandle pipe) {
  static base::NoDestructor<MiniChromeImpl> impl;
  impl->Bind(mojo::PendingReceiver<mini::mojom::MiniChrome>(std::move(pipe)));
}

