package org.chromium.axa_client;

import java.lang.String;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.os.Parcelable;
import android.os.ParcelFileDescriptor;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.view.View;

import org.chromium.base.ContextUtils;
import org.chromium.base.library_loader.LibraryLoader;
import org.chromium.base.library_loader.LibraryProcessType;
import org.chromium.base.library_loader.ProcessInitException;

public final class MainActivity extends Activity {
  private EditText mNotifyText;
  private Button mNotifyButton;
  private EditText mUrlText;
  private Button mFetchButton;
  private TextView mFetchedText;
  private ServiceConnection mService;
  private Messenger mMessenger;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    ContextUtils.initApplicationContext(this);
    setContentView(R.layout.activity_main);
    mNotifyText = findViewById(R.id.notify_text);
    mNotifyButton = findViewById(R.id.notify_button);
    mNotifyButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        NativeBridge.notify(mNotifyText.getText().toString());
      }
    });
    mUrlText = findViewById(R.id.url_text);
    mFetchButton = findViewById(R.id.fetch_button);
    mFetchedText = findViewById(R.id.fetched_text);
    mFetchButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        NativeBridge.fetchUrl(mUrlText.getText().toString());
      }
    });

    mService = new ServiceConnection() {
      @Override
      public void onServiceConnected(ComponentName name, IBinder service) {
        mMessenger = new Messenger(service);
        Message msg = Message.obtain(null, 1, 0, 0);
        Bundle bundle = new Bundle();
        Parcelable[] parcels = new Parcelable[1];
        parcels[0] = ParcelFileDescriptor.adoptFd(NativeBridge.initRuntime());
        bundle.putParcelableArray("channel", parcels);
        msg.setData(bundle);
        try {
          mMessenger.send(msg);
        } catch (RemoteException e) {
          e.printStackTrace();
        }

        NativeBridge.moarInit();
      }

      @Override
      public void onServiceDisconnected(ComponentName name) {
        mMessenger = null;
      }
    };

    try {
      LibraryLoader.getInstance().ensureInitialized(LibraryProcessType.PROCESS_BROWSER);
    } catch (ProcessInitException e) {
      e.printStackTrace();
    }

    Intent intent = new Intent();
    intent.setClassName("org.chromium.axa_service",
                        "org.chromium.axa_service.Listener");
    bindService(intent, mService, Context.BIND_AUTO_CREATE);
  }

  public void updateFetchedText(String text) {
    mFetchedText.setText(text);
  }
}

