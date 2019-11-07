package fr.pchab.androidrtc;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.graphics.Point;
import android.media.AudioManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.speech.RecognizerIntent;
import android.speech.tts.TextToSpeech;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager.LayoutParams;
import android.widget.ImageView;
import android.widget.Toast;

import org.json.JSONException;
import org.webrtc.MediaStream;
import org.webrtc.VideoRenderer;
import org.webrtc.VideoRendererGui;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.UUID;

import fr.pchab.webrtcclient.PeerConnectionParameters;
import fr.pchab.webrtcclient.WebRtcClient;

public class RtcActivity extends Activity implements WebRtcClient.RtcListener {
    private final static String TAG = RtcActivity.class.getCanonicalName();
    private final static int VIDEO_CALL_SENT = 666;
    private final static int CALL_ACCEPT = 10;
    private static final String VIDEO_CODEC_VP9 = "VP9";
    private static final String AUDIO_CODEC_OPUS = "opus";
    // Local preview screen position before call is connected.
    private static final int LOCAL_X_CONNECTING = 0;
    private static final int LOCAL_Y_CONNECTING = 0;
    private static final int LOCAL_WIDTH_CONNECTING = 100;
    private static final int LOCAL_HEIGHT_CONNECTING = 100;
    // Local preview screen position after call is connected.
    private static final int LOCAL_X_CONNECTED = 72;
    private static final int LOCAL_Y_CONNECTED = 72;
    private static final int LOCAL_WIDTH_CONNECTED = 25;
    private static final int LOCAL_HEIGHT_CONNECTED = 25;
    // Remote video screen position
    private static final int REMOTE_X = 0;
    private static final int REMOTE_Y = 0;
    private static final int REMOTE_WIDTH = 100;
    private static final int REMOTE_HEIGHT = 100;
    private VideoRendererGui.ScalingType scalingType = VideoRendererGui.ScalingType.SCALE_ASPECT_FILL;
    private GLSurfaceView vsv;
    ImageView iv;
    private VideoRenderer.Callbacks localRender;
    private VideoRenderer.Callbacks remoteRender;
    private WebRtcClient client;
    private String mSocketAddress;
    private String callerId;

    private static final String[] RequiredPermissions = new String[]{Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO};
    protected PermissionChecker permissionChecker = new PermissionChecker();

    private TextToSpeech textToSpeech;
    private AudioManager audioManager;

    private BluetoothAdapter btAdapter;
    private BluetoothDevice btDevice;
    private BluetoothSocket btSocket;
    private OutputStream outStream;

    // Well known SPP UUID, after run intel SPP_loopback.py, you can see this in edison bluetooth
    private static final UUID SSP_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");

    // intel edison bluetooth MAC address 98:4F:EE:04:0E:B5
    private static String EDISON_ADDRESS = "98:4F:EE:04:6E:E1";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(
                LayoutParams.FLAG_FULLSCREEN
                        | LayoutParams.FLAG_KEEP_SCREEN_ON
                        | LayoutParams.FLAG_DISMISS_KEYGUARD
                        | LayoutParams.FLAG_SHOW_WHEN_LOCKED
                        | LayoutParams.FLAG_TURN_SCREEN_ON);
        setContentView(R.layout.main);
        mSocketAddress = "http://" + getResources().getString(R.string.host);
        mSocketAddress += (":" + getResources().getString(R.string.port) + "/");

        vsv = (GLSurfaceView) findViewById(R.id.glview_call);
        vsv.setPreserveEGLContextOnPause(true);
        vsv.setKeepScreenOn(true);
        VideoRendererGui.setView(vsv, new Runnable() {
            @Override
            public void run() {
                init();
            }
        });

        iv = findViewById(R.id.imageView);
        iv.setVisibility(View.GONE);

        // local and remote render
        remoteRender = VideoRendererGui.create(
                REMOTE_X, REMOTE_Y,
                REMOTE_WIDTH, REMOTE_HEIGHT, scalingType, false);
        localRender = VideoRendererGui.create(
                LOCAL_X_CONNECTING, LOCAL_Y_CONNECTING,
                LOCAL_WIDTH_CONNECTING, LOCAL_HEIGHT_CONNECTING, scalingType, true);

        final Intent intent = getIntent();
        final String action = intent.getAction();

        if (Intent.ACTION_VIEW.equals(action)) {
            final List<String> segments = intent.getData().getPathSegments();
            callerId = segments.get(0);
        }

        checkPermissions();

        textToSpeech = new TextToSpeech(getApplicationContext(), new TextToSpeech.OnInitListener() {
            @Override
            public void onInit(int status) {
                if (status == TextToSpeech.SUCCESS) {
                    int ttsLang = textToSpeech.setLanguage(Locale.US);

                    if (ttsLang == TextToSpeech.LANG_MISSING_DATA
                            || ttsLang == TextToSpeech.LANG_NOT_SUPPORTED) {
                        Log.e("TTS", "The Language is not supported!");
                    } else {
                        Log.i("TTS", "Language Supported.");
                    }
                    Log.i("TTS", "Initialization success.");
                } else {
                    Toast.makeText(getApplicationContext(), "TTS Initialization failed!", Toast.LENGTH_SHORT).show();
                }
            }
        });

        /*
        audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        Thread thread = new Thread() {
            @Override
            public void run() {
                try {
                    while(true) {
                        sleep(1000);
                        audioManager.setMode(AudioManager.MODE_IN_CALL);
                        if (!audioManager.isSpeakerphoneOn())
                            audioManager.setSpeakerphoneOn(true);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        };

        thread.start();
        */

        //initBluetooth();
        //bluetoothConnect();
        //bluetoothSendMsg("w");

    }

    private void checkPermissions() {
        permissionChecker.verifyPermissions(this, RequiredPermissions, new PermissionChecker.VerifyPermissionsCallback() {

            @Override
            public void onPermissionAllGranted() {

            }

            @Override
            public void onPermissionDeny(String[] permissions) {
                Toast.makeText(RtcActivity.this, "Please grant required permissions.", Toast.LENGTH_LONG).show();
            }
        });
    }

    private void init() {
        Point displaySize = new Point();
        getWindowManager().getDefaultDisplay().getSize(displaySize);
        PeerConnectionParameters params = new PeerConnectionParameters(
                true, false, displaySize.x, displaySize.y, 30, 1, VIDEO_CODEC_VP9, true, 1, AUDIO_CODEC_OPUS, true);
        client = new WebRtcClient(this, mSocketAddress, params, VideoRendererGui.getEGLContext());
    }

    @Override
    public void onPause() {
        super.onPause();
        vsv.onPause();
        if (client != null) {
            client.onPause();
        }
        if (btSocket != null) {
            try {
                btSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        vsv.onResume();
        if (client != null) {
            client.onResume();
        }
    }

    @Override
    public void onDestroy() {
        if (client != null) {
            client.onDestroy();
        }
        super.onDestroy();
        if (textToSpeech != null) {
            textToSpeech.stop();
            textToSpeech.shutdown();
        }
    }

    @Override
    public void onCallReady(String callId) {
        if (callerId != null) {
            try {
                answer(callerId);
            } catch (JSONException e) {
                e.printStackTrace();
            }
        } else {
            call(callId);
        }
    }

    public void answer(String callerId) throws JSONException {
        client.sendMessage(callerId, "init", null);
        startCam();
    }

    public void call(String callId) {
        Intent msg = new Intent(Intent.ACTION_SEND);
        msg.putExtra(Intent.EXTRA_TEXT, mSocketAddress + callId);
        msg.setType("text/plain");
        startActivityForResult(Intent.createChooser(msg, "Call someone :"), VIDEO_CALL_SENT);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case VIDEO_CALL_SENT:
                startCam();
                break;
            case CALL_ACCEPT:
                ArrayList<String> words = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);
                boolean answered = false;
                for (String word : words) {
                    if (word.toLowerCase().equals("hello")) {
                        answerIncomingCall();
                        answered = true;
                        break;
                    }
                }
                if (!answered) {
                    initiateIncomingCall("Diana");
                }
                break;
        }
    }

    public void startCam() {
        // Camera settings
        if (PermissionChecker.hasPermissions(this, RequiredPermissions)) {
            client.start("android_test");
        }
    }

    @Override
    public void onStatusChanged(final String newStatus) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), newStatus, Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void onLocalStream(MediaStream localStream) {
        localStream.videoTracks.get(0).addRenderer(new VideoRenderer(localRender));
        VideoRendererGui.update(localRender,
                LOCAL_X_CONNECTING, LOCAL_Y_CONNECTING,
                LOCAL_WIDTH_CONNECTING, LOCAL_HEIGHT_CONNECTING,
                scalingType, false);
    }

    @Override
    public void onAddRemoteStream(MediaStream remoteStream, int endPoint) {
        remoteStream.videoTracks.get(0).addRenderer(new VideoRenderer(remoteRender));
        VideoRendererGui.update(remoteRender,
                REMOTE_X, REMOTE_Y,
                REMOTE_WIDTH, REMOTE_HEIGHT, scalingType, false);
        VideoRendererGui.update(localRender,
                LOCAL_X_CONNECTED, LOCAL_Y_CONNECTED,
                LOCAL_WIDTH_CONNECTED, LOCAL_HEIGHT_CONNECTED,
                scalingType, false);
    }

    @Override
    public void onRemoveRemoteStream(int endPoint) {
        VideoRendererGui.update(localRender,
                LOCAL_X_CONNECTING, LOCAL_Y_CONNECTING,
                LOCAL_WIDTH_CONNECTING, LOCAL_HEIGHT_CONNECTING,
                scalingType, false);
    }

    @Override
    public void onMessageReceived(final String message) {
        switch (message) {
            case "showImage":
                initiateIncomingCall("Diana");
                break;
            case "hideImage":
                cancelIncomingCall();
                break;
            default:
                break;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        permissionChecker.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    private void initBluetooth() {

        btAdapter = BluetoothAdapter.getDefaultAdapter();

        if (btAdapter == null) {
            Toast.makeText(getApplicationContext(), "No bluetooth!", Toast.LENGTH_LONG).show();
            Log.d(TAG, "onCreate, No bluetooth detect...");
            //    finish();
        }
        else {
            if (btAdapter.isEnabled()) {
                Log.d(TAG, "check bluetooth enabled...");
            } else {
                Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(intent, 1);
            }
        }
    }

    //connect bluetooth and init write stream
    private void bluetoothConnect() {

        btDevice = btAdapter.getRemoteDevice(EDISON_ADDRESS);
        if (btDevice == null) {
            Log.d(TAG,"get remote device fail!");
            finish();
        }

        try {
            btSocket = btDevice.createRfcommSocketToServiceRecord(SSP_UUID);
        } catch (IOException e) {
            Log.d(TAG,"bluetooth socket create fail.");
        }
        //save resource by cancel discovery
        btAdapter.cancelDiscovery();

        //connect
        try {
            btSocket.connect();
            Log.d(TAG,"Connection established.");
        } catch ( IOException e) {
            try {
                btSocket.close();
            }catch (IOException e2) {
                Log.d(TAG,"unable to close socket after connect fail.");
            }
        }

        //prepare outStream to send message
        try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            Log.d(TAG,"output stream init fail!");
        }

    }

    //bluetooth send serial message
    private void bluetoothSendMsg(String s) {
        byte [] msgOnBuf;
        msgOnBuf = s.getBytes();
        try {
            outStream.write(msgOnBuf);
        } catch (IOException e) {
            Log.d(TAG,"send message fail!");
        }
    }

    private void initiateIncomingCall(String name) {
        client.muteAudio();
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                iv.setVisibility(View.VISIBLE);
            }
        });
        textToSpeech.speak(
                String.format("Brainspin team calling to %s. Say hello to answer the call.", name),
                TextToSpeech.QUEUE_FLUSH,
                null
        );
        while(textToSpeech.isSpeaking());
        Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale.ENGLISH);
        startActivityForResult(intent, CALL_ACCEPT);
    }

    private void answerIncomingCall() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                iv.setVisibility(View.GONE);
            }
        });
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        client.unmuteAudio();
    }

    private void cancelIncomingCall() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                iv.setVisibility(View.GONE);
            }
        });
    }

}