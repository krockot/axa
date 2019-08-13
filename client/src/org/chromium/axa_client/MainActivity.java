package org.chromium.axa_client;

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
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.view.View;

public final class MainActivity
    extends Activity
    implements View.OnClickListener {
  private Button mButton;
  private EditText mText;
  private ServiceConnection mService;
  private Messenger mMessenger;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    mButton = findViewById(R.id.button);
    mButton.setOnClickListener(this);
    mText = findViewById(R.id.text);
    mService = new ServiceConnection() {
      @Override
      public void onServiceConnected(ComponentName name, IBinder service) {
        mMessenger = new Messenger(service);
      }

      @Override
      public void onServiceDisconnected(ComponentName name) {
        mMessenger = null;
      }
    };

    Intent intent = new Intent();
    intent.setClassName("org.chromium.axa_service",
                        "org.chromium.axa_service.Notifier");
    bindService(intent, mService, Context.BIND_AUTO_CREATE);
  }

  @Override
  public void onClick(View view) {
    if (mMessenger == null)
      return;
    Message msg = Message.obtain(null, 1, 0, 0);
    Bundle bundle = new Bundle();
    bundle.putString("text", mText.getText().toString());
    msg.setData(bundle);
    try {
      mMessenger.send(msg);
    } catch (RemoteException e) {
      e.printStackTrace();
    }
  }
}

