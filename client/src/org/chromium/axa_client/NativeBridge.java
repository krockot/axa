package org.chromium.axa_client;

import org.chromium.base.annotations.JNINamespace;
import org.chromium.base.annotations.NativeMethods;

@JNINamespace("axa_client")
public final class NativeBridge {
  private NativeBridge() {}

  public static int initRuntime() {
    return NativeBridgeJni.get().initRuntime();
  }

  public static void moarInit() {
    NativeBridgeJni.get().moarInit();
  }

  public static void notify(String text) {
    NativeBridgeJni.get().notify(text);
  }

  @NativeMethods
  interface Natives {
    int initRuntime();
    void moarInit();
    void notify(String text);
  }
}

