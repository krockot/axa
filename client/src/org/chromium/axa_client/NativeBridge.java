package org.chromium.axa_client;

import org.chromium.base.annotations.JNINamespace;
import org.chromium.base.annotations.NativeMethods;

@JNINamespace("axa_client")
public final class NativeBridge {
  private NativeBridge() {}

  public static void initRuntime() {
    NativeBridgeJni.get().initRuntime();
  }

  @NativeMethods
  interface Natives {
    void initRuntime();
  }
}

