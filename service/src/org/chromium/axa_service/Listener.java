package org.chromium.axa_service;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Handler;
import android.os.Message;
import android.os.Messenger;
import android.os.Parcelable;
import android.os.ParcelFileDescriptor;

import org.chromium.base.ContextUtils;
import org.chromium.base.library_loader.LibraryLoader;
import org.chromium.base.library_loader.LibraryProcessType;
import org.chromium.base.library_loader.ProcessInitException;

public final class Listener extends Service {
  class ClientChannelHandler extends Handler  {
    public ClientChannelHandler() {}

    @Override
    public void handleMessage(Message msg) {
      Bundle bundle = msg.getData();
      Parcelable[] parcels = bundle.getParcelableArray("channel");
      ParcelFileDescriptor channelFd = (ParcelFileDescriptor)parcels[0];
      NativeBridge.inviteMojoClient(channelFd.detachFd());
    }
  }

  Messenger mMessenger = new Messenger(new ClientChannelHandler());

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

