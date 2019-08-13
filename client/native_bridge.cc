#include "base/android/base_jni_onload.h"
#include "base/android/jni_android.h"
#include "base/logging.h"
#include "axa/client/jni_headers/NativeBridge_jni.h"

namespace axa_client {

void JNI_NativeBridge_InitRuntime(JNIEnv* env) {
}

}  // namespace axa_client

JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  base::android::InitVM(vm);
  if (!base::android::OnJNIOnLoadInit())
    return -1;
  return JNI_VERSION_1_4;
}

