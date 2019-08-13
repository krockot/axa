package org.chromium.axa_service;

import org.chromium.base.annotations.JNINamespace;
import org.chromium.base.annotations.NativeMethods;

@JNINamespace("axa_service")
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

