package org.chromium.axa_service;

import java.lang.String;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;

import org.chromium.base.ContextUtils;
import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;
import org.chromium.base.annotations.NativeMethods;

@JNINamespace("axa_service")
public final class NativeBridge {
  private NativeBridge() {}

  public static void initRuntime() {
    NativeBridgeJni.get().initRuntime();
  }

  public static void inviteMojoClient(int fd) {
    NativeBridgeJni.get().inviteMojoClient(fd);
  }

  @CalledByNative
  public static void notify(String text) {
    Context context = ContextUtils.getApplicationContext();
    Notification noti = new Notification.Builder(context)
        .setContentTitle("Client says something")
        .setContentText(text)
        .setSmallIcon(R.drawable.axa)
        .build();
    NotificationManager mgr =
        (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
    mgr.notify(42, noti);
  }

  @NativeMethods
  interface Natives {
    void initRuntime();
    void inviteMojoClient(int fd);
  }
}

