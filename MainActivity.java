

import androidx.appcompat.app.AppCompatActivity;
import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;

import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

import com.harrysoft.androidbluetoothserial.BluetoothManager;
import com.harrysoft.androidbluetoothserial.BluetoothSerialDevice;
import com.harrysoft.androidbluetoothserial.SimpleBluetoothDeviceInterface;

import java.util.Collection;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.schedulers.Schedulers;


public class MainActivity extends AppCompatActivity {

    private Context context;

    private BluetoothManager bluetoothManager;
    private SimpleBluetoothDeviceInterface deviceInterface;

    private final String ESP32_DEVICE = "AirQualityControl";
    private String Esp32_Mac;

    private Button btnScan;
    private Button btnSend;
    private TextView txtStatusCheck;
    private TextView txtResponse;
    private EditText editSSID;
    private EditText editKey;
    private RadioButton radioWIFI;
    private RadioButton radioSleepON;
    private RadioButton radioSleepOFF;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        context = getApplicationContext();

        bluetoothManager = BluetoothManager.getInstance();
        if (bluetoothManager == null) {
            Toast.makeText(context, "Bluetooth isnt available", Toast.LENGTH_LONG).show();
            finish();
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        this.registerReceiver(mReceiver, filter);

        btnScan = findViewById(R.id.btnScan);
        btnSend = findViewById(R.id.btnSendData);
        txtStatusCheck = findViewById(R.id.txtStatusCheck);
        editKey = findViewById(R.id.editKey);
        editSSID = findViewById(R.id.editSSID);
        txtResponse = findViewById(R.id.txtResponse);
        radioWIFI = findViewById(R.id.radioWIFI);
        radioSleepON = findViewById(R.id.radioSleepON);
        radioSleepOFF = findViewById(R.id.radioSleepOFF);

        btnScan.setEnabled(true);
        btnSend.setEnabled(false);

        btnScan.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                scanDevices();
            }
        });

        radioWIFI.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (b){
                    radioSleepOFF.setChecked(false);
                    radioSleepON.setChecked(false);
                }
            }
        });
        radioSleepOFF.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (b){
                    radioSleepON.setChecked(false);
                    radioWIFI.setChecked(false);
                }
            }
        });
        radioSleepON.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (b){
                    radioSleepOFF.setChecked(false);
                    radioWIFI.setChecked(false);
                }
            }
        });

        btnSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (radioWIFI.isChecked()) {
                    String ssid = editSSID.getText().toString();
                    String key = editKey.getText().toString();
                    if (ssid.contains("\n"))
                        ssid.replace("\n", "");
                    if (key.contains("\n"))
                        key.replace("\n", "");
                    deviceInterface.sendMessage(ssid + "&" + key);
                }else if (radioSleepOFF.isChecked())
                    deviceInterface.sendMessage("deepsleep=no");
                else if (radioSleepON.isChecked())
                    deviceInterface.sendMessage("deepsleep=yes");

            }
        });

    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            if (BluetoothDevice.ACTION_ACL_CONNECTED.equals(action)) {
                txtStatusCheck.setText("Connected");
                txtStatusCheck.setTextColor(Color.GREEN);
                btnScan.setEnabled(false);
                btnSend.setEnabled(true);
            } else if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
                txtStatusCheck.setText("Disconnected");
                txtStatusCheck.setTextColor(Color.RED);
                btnScan.setEnabled(true);
                btnSend.setEnabled(false);
            }
        }
    };


    @SuppressLint("MissingPermission")
    private void scanDevices() {

        Esp32_Mac = "";

        Collection<BluetoothDevice> pairedDevices = bluetoothManager.getPairedDevicesList();
        for (BluetoothDevice device : pairedDevices)
            if (device.getName().equals(ESP32_DEVICE)) {
                Esp32_Mac = device.getAddress();
                break;
            }

        if (!Esp32_Mac.isEmpty())
            connectDevice(Esp32_Mac);



    }


    private void connectDevice(String mac) {

        bluetoothManager.openSerialDevice(mac)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(this::onConnected, this::onError);

    }

    private void onConnected(BluetoothSerialDevice connectedDevice) {

        deviceInterface = connectedDevice.toSimpleDeviceInterface();
        deviceInterface.setListeners(this::onMessageReceived, this::onMessageSent, this::onError);



    }

    private void onMessageSent(String message) {

        Toast.makeText(context, "Data sent", Toast.LENGTH_SHORT).show();

    }

    private void onMessageReceived(String message) {

        txtResponse.setText(message);

    }

    private void onError(Throwable error) {
        bluetoothManager.closeDevice(Esp32_Mac);
        bluetoothManager.close() ;
    }



}
