#include "axa/client/jni_headers/NativeBridge_jni.h"
#include "axa/mojom/mini_chrome.mojom.h"
#include "base/android/base_jni_onload.h"
#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/no_destructor.h"
#include "base/threading/thread.h"
#include "mojo/core/embedder/embedder.h"
#include "mojo/core/embedder/scoped_ipc_support.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/platform/platform_channel.h"
#include "mojo/public/cpp/system/invitation.h"
#include "mojo/public/cpp/system/message_pipe.h"

namespace {

mojo::PlatformChannelEndpoint& GetInvitationEndpoint() {
  static base::NoDestructor<mojo::PlatformChannelEndpoint> endpoint;
  return *endpoint;
}

mojo::Remote<mini::mojom::MiniChrome>& GetMiniChromeRemote() {
  static base::NoDestructor<mojo::Remote<mini::mojom::MiniChrome>> remote;
  return *remote;
}

}  // namespace

namespace axa_client {

jint JNI_NativeBridge_InitRuntime(JNIEnv* env) {
  mojo::core::Init();

  static base::NoDestructor<base::MessageLoop> loop{base::MessagePumpType::UI};

  static base::NoDestructor<base::Thread> io_thread{"io thread"};
  io_thread->StartWithOptions({base::MessagePumpType::IO, 0});

  static base::NoDestructor<mojo::core::ScopedIPCSupport> ipc_support{
    io_thread->task_runner(),
    mojo::core::ScopedIPCSupport::ShutdownPolicy::FAST};

  mojo::PlatformChannel channel;
  GetInvitationEndpoint() = channel.TakeLocalEndpoint();
  return channel.TakeRemoteEndpoint().TakePlatformHandle().ReleaseFD();
}

void JNI_NativeBridge_MoarInit(JNIEnv* env) {
  // NOTE: |Accept()| blocks the thread until it gets some communication from
  // the remote service.
  auto invitation =
      mojo::IncomingInvitation::Accept(std::move(GetInvitationEndpoint()));
  GetMiniChromeRemote().Bind(mojo::PendingRemote<mini::mojom::MiniChrome>(
      invitation.ExtractMessagePipe(0), 0));
}

void JNI_NativeBridge_Notify(JNIEnv* env,
                             const base::android::JavaParamRef<jstring>& text) {
  mojo::Remote<mini::mojom::Notifier> notifier;
  GetMiniChromeRemote()->BindNotifier(notifier.BindNewPipeAndPassReceiver());
  notifier->Notify(base::android::ConvertJavaStringToUTF8(env, text));
}

}  // namespace axa_client

JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  base::android::InitVM(vm);
  if (!base::android::OnJNIOnLoadInit())
    return -1;
  return JNI_VERSION_1_4;
}

