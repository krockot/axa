package org.chromium.axa_service;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Handler;
import android.os.Message;
import android.os.Messenger;

import org.chromium.base.ContextUtils;
import org.chromium.base.library_loader.LibraryLoader;
import org.chromium.base.library_loader.LibraryProcessType;
import org.chromium.base.library_loader.ProcessInitException;

public final class Notifier extends Service {
  class IncomingHandler extends Handler  {
    Notifier mNotifier;

    public IncomingHandler(Notifier notifier) {
      mNotifier = notifier;
    }

    @Override
    public void handleMessage(Message msg) {
      Bundle bundle = msg.getData();
      String text = (String)bundle.get("text");
      Notification noti = new Notification.Builder(mNotifier)
          .setContentTitle("Client says something")
          .setContentText(text)
          .setSmallIcon(R.drawable.axa)
          .build();
      NotificationManager mgr = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
      mgr.notify(42, noti);
    }
  }

  Messenger mMessenger = new Messenger(new IncomingHandler(this));

  @Override
  public void onCreate() {
    ContextUtils.initApplicationContext(this);
    try {
      LibraryLoader.getInstance().ensureInitialized(LibraryProcessType.PROCESS_BROWSER);
    } catch (ProcessInitException e) {
      e.printStackTrace();
    }

    NativeBridge.initRuntime();
  }

  @Override
  public IBinder onBind(Intent intent) {
    return mMessenger.getBinder();
  }
}

