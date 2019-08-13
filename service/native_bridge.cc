#include "axa/service/native_bridge.h"

#include "axa/service/jni_headers/NativeBridge_jni.h"
#include "axa/service/mini_chrome.h"
#include "base/android/base_jni_onload.h"
#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/message_loop/message_loop.h"
#include "base/task/thread_pool/thread_pool.h"
#include "base/threading/thread.h"
#include "mojo/core/embedder/embedder.h"
#include "mojo/core/embedder/scoped_ipc_support.h"
#include "mojo/public/cpp/platform/platform_handle.h"
#include "mojo/public/cpp/system/invitation.h"

namespace axa_service {

base::Thread& GetIOThread() {
  static base::NoDestructor<base::Thread> io_thread{"io thread"};
  return *io_thread;
}

void JNI_NativeBridge_InitRuntime(JNIEnv* env) {
  mojo::core::Init();

  static base::NoDestructor<base::MessageLoop> loop{base::MessagePumpType::UI};

  GetIOThread().StartWithOptions({base::MessagePumpType::IO, 0});

  static base::NoDestructor<mojo::core::ScopedIPCSupport> ipc_support{
    GetIOThread().task_runner(),
    mojo::core::ScopedIPCSupport::ShutdownPolicy::FAST};

  base::ThreadPoolInstance::CreateAndStartWithDefaultParams("THREADS!");
}

void JNI_NativeBridge_InviteMojoClient(JNIEnv* env, jint fd) {
  auto endpoint =
      mojo::PlatformChannelEndpoint(mojo::PlatformHandle(base::ScopedFD(fd)));
  mojo::OutgoingInvitation invitation;
  auto pipe = invitation.AttachMessagePipe(0);
  mojo::OutgoingInvitation::Send(
      std::move(invitation), base::kNullProcessHandle, std::move(endpoint));

  BindMiniChrome(std::move(pipe));
}

}  // namespace axa_service

JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  base::android::InitVM(vm);
  if (!base::android::OnJNIOnLoadInit())
    return -1;
  return JNI_VERSION_1_4;
}

void DoNotify(const std::string& text) {
  JNIEnv* env = base::android::AttachCurrentThread();
  axa_service::Java_NativeBridge_notify(
      env, base::android::ConvertUTF8ToJavaString(env, text));
}

