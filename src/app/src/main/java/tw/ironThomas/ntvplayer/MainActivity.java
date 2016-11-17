package tw.ironThomas.ntvplayer;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.view.SurfaceView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;
import tw.ironThomas.ntvplayer.keymap;

import java.io.File;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;


import org.videolan.libvlc.IVLCVout;
import org.videolan.libvlc.LibVLC;
import org.videolan.libvlc.Media;
import org.videolan.libvlc.MediaPlayer;
import org.videolan.libvlc.util.HWDecoderUtil;

import tw.ironThomas.ntvplayer.NTVService;

public class MainActivity extends Activity implements IVLCVout.Callback {
    final int Quality_FULLHD	   = 0;
    final int Quality_HD    = 1;
    final int Quality_DVD   = 2;

    final String tag= "NTVPlayer";
    final int btn0 = 0;
    final int btn1 = 1;
    final int btn2 = 2;
    final int btn3 = 3;
    final int btn4 = 4;
    final int btn5 = 5;
    final int btn6 = 6;
    final int btn7 = 7;
    final int btn8 = 8;
    final int btn9 = 9;
    final int btnPower = 10;
    final int btnUp = 11;
    final int btnDown = 12;
    final int btnConnect = 13;
    final int btnSetting = 14;
    final int btnInfo = 15;
    final int btnServerSetting = 16;
    final int btnReturn = 17;
    final int btnRc = 18;
    final int btnRecord = 19;

    final int btnTotalCount = 20; // counter

    int[] allButtonID = {
            R.id.btn_0,
            R.id.btn_1,
            R.id.btn_2,
            R.id.btn_3,
            R.id.btn_4,
            R.id.btn_5,
            R.id.btn_6,
            R.id.btn_7,
            R.id.btn_8,
            R.id.btn_9,
            R.id.btn_power,
            R.id.btn_forward,
            R.id.btn_backward,
            R.id.btn_connect,
            R.id.btn_setting,
            R.id.btn_info,
            R.id.btn_server_setting,
            R.id.btn_return,
            R.id.btn_rc,
            R.id.btn_record
    };

    int[] rcButtons = {
            btnPower, btnUp, btnDown, btnReturn,
            btn0, btn1, btn2, btn3, btn4, btn5, btn6, btn7, btn8, btn9
    };

    keymap[] keys = {
            new keymap(R.id.btn_power, "key:power"),
            new keymap(R.id.btn_return, "key:return"),
            new keymap(R.id.btn_backward, "key:ch_down"),
            new keymap(R.id.btn_forward, "key:ch_up"),
            new keymap(R.id.btn_0, "key:0"),
            new keymap(R.id.btn_1, "key:1"),
            new keymap(R.id.btn_2, "key:2"),
            new keymap(R.id.btn_3, "key:3"),
            new keymap(R.id.btn_4, "key:4"),
            new keymap(R.id.btn_5, "key:5"),
            new keymap(R.id.btn_6, "key:6"),
            new keymap(R.id.btn_7, "key:7"),
            new keymap(R.id.btn_8, "key:8"),
            new keymap(R.id.btn_9, "key:9"),
    };

    private LinearLayout mControl_panel = null;
    private RelativeLayout mRc_panel = null;
    private LinearLayout mServer_panel = null;
    private ImageButton[] btn = new ImageButton[btnTotalCount];

    private Button btnFullhd;
    private Button btnHd;
    private Button btnDVD;
    private Button btnStop;
    private Button btnReboot;

    private TextView textInfo;
    private ImageView imageRecord;



    private LibVLC mLibVLC = null;
    private Media   mMedia = null;
    private MediaPlayer mMediaPlayer = null;
    private FrameLayout mVideoSurfaceFrame = null;
    private SurfaceView mVideoSurface = null;


    private String mServerAddress;
    private String mLoginKey;
    private boolean mDisplayServerPanel;
    private boolean mDisplayRcPanel;
    private boolean mDisplayInfo;


    private Handler mHandler;


    private int mVideoHeight = 0;
    private int mVideoWidth = 0;
    private int mVideoVisibleHeight = 0;
    private int mVideoVisibleWidth = 0;
    private int mVideoSarNum = 0;
    private int mVideoSarDen = 0;


    float downX;
    float downY;
    float upX;
    float upY;

    int mRecordState;
    int mConnectStatus;


    void SetRcPanel(boolean state) {
        for (int idx : rcButtons) {
            btn[idx].setEnabled(state);
        }
    }

    void SetServerPanel(boolean state) {
        btnFullhd.setEnabled(state);
        btnHd.setEnabled(state);
        btnDVD.setEnabled(state);
        btnStop.setEnabled(state);
        btnReboot.setEnabled(state);
    }



    public void OnServerControlButton(View view)
    {
        switch(view.getId())
        {
        case R.id.btn_fullhd:
            NTVService.quality(Quality_FULLHD);
            break;
        case R.id.btn_hd:
            NTVService.quality(Quality_HD);
            break;
        case R.id.btn_dvd:
            NTVService.quality(Quality_DVD);
            break;
        case R.id.btn_server_stop:
            NTVService.command("cmd:stop");
            break;
        case R.id.btn_reboot:
            NTVService.command("cmd:reboot");
            break;
        };

    }


    public void OnServiceCommandButton(View view)
    {
        int id = view.getId();

        for(keymap key : keys) {
            if(key.id == id) {
                NTVService.command(key.func);
                break;
            }
        }
    }

    void GetSettings()
    {
        SharedPreferences  settings = PreferenceManager.getDefaultSharedPreferences(MainActivity.this);
        mDisplayServerPanel = settings.getBoolean("display_server_panel",true);
        mDisplayRcPanel = settings.getBoolean("display_rc_panel",true);
        mDisplayInfo = settings.getBoolean("display_info",true);
    }

    void FindViews()
    {
        mVideoSurfaceFrame = (FrameLayout) findViewById(R.id.video_surface_frame);
        mVideoSurface = (SurfaceView) findViewById(R.id.surface_video);

        mControl_panel = (LinearLayout)findViewById(R.id.control_panel);
        mRc_panel = (RelativeLayout)findViewById(R.id.rc_panel);
        mServer_panel = (LinearLayout)findViewById(R.id.server_panel);

        textInfo = (TextView)findViewById(R.id.text_info);
        imageRecord = (ImageView)findViewById(R.id.image_record);


        btnFullhd= (Button)findViewById(R.id.btn_fullhd);
        btnHd= (Button)findViewById(R.id.btn_hd);
        btnDVD= (Button)findViewById(R.id.btn_dvd);
        btnStop= (Button)findViewById(R.id.btn_server_stop);
        btnReboot= (Button)findViewById(R.id.btn_reboot);



        for(int x = 0; x < btnTotalCount; x++) {
            btn[x]=(ImageButton)findViewById(allButtonID[x]);
        }
    }

    void SaveSettingBoolean(String key, boolean b)
    {
        SharedPreferences.Editor editor =
                PreferenceManager.getDefaultSharedPreferences(MainActivity.this).edit();
        editor.putBoolean(key, b);
        editor.commit();
    }

    public class infoTask extends TimerTask
    {
        public void run()
        {
            Message m = new Message();
            m.what = 1;
            mHandler.sendMessage(m);

        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);


        File file = new File("/sdcard/NTVPlayerRecorder/");
        if (!file.isDirectory()) file.mkdirs();

        final ArrayList<String> args = new ArrayList<>();
        args.add("-vvv");

        mLibVLC = new LibVLC(this);
        mMediaPlayer = new MediaPlayer(mLibVLC);

        FindViews();
        GetSettings();

        mConnectStatus = 1;
        btn[btnRecord].setEnabled(false);
        imageRecord.setVisibility(View.INVISIBLE);

        SetRcPanel(false);
        SetServerPanel(false);

        if(mDisplayServerPanel == false)
            mServer_panel.setVisibility(View.INVISIBLE);

        if(mDisplayRcPanel == false)
            mRc_panel.setVisibility(View.INVISIBLE);

        if(mDisplayInfo == false)
            textInfo.setVisibility(View.INVISIBLE);

        btn[btnSetting].setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, Setting.class);
                startActivity(intent);
            }
        });

        btn[btnServerSetting].setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mServer_panel.getVisibility() == View.VISIBLE) {
                    mServer_panel.setVisibility(View.INVISIBLE);
                    mDisplayServerPanel = false;
                }
                else {
                    mServer_panel.setVisibility(View.VISIBLE);
                    mServer_panel.bringToFront();
                    mDisplayServerPanel = true;
                }
                SaveSettingBoolean("display_server_panel", mDisplayServerPanel);
            }
        });

        btn[btnInfo].setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(textInfo.getVisibility() == View.VISIBLE) {
                    textInfo.setVisibility(View.INVISIBLE);
                    mDisplayInfo = false;
                }
                else {
                    textInfo.setVisibility(View.VISIBLE);
                    textInfo.bringToFront();
                    mDisplayInfo = true;
                }
                SaveSettingBoolean("display_info", mDisplayInfo);
            }
        });


        btn[btnRc].setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mRc_panel.getVisibility() == View.VISIBLE) {
                    mRc_panel.setVisibility(View.INVISIBLE);
                    mDisplayRcPanel = false;
                }
                else {
                    mRc_panel.setVisibility(View.VISIBLE);
                    mRc_panel.bringToFront();
                    mDisplayRcPanel = true;
                }
                SaveSettingBoolean("display_rc_panel", mDisplayRcPanel);
            }
        });

        btn[btnRecord].setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int w = -1;
                int h = -1;

                if(mRecordState != 0) {
                    mRecordState = 0;
                    imageRecord.setVisibility(View.INVISIBLE);
                } else {
                    mRecordState = 1;
                    imageRecord.setVisibility(View.VISIBLE);
                    imageRecord.bringToFront();

                    if (mMedia != null) {
                        if (mMedia.isParsed()) {
                            for (int i = 0; i < mMedia.getTrackCount(); ++i) {
                                Media.Track track = mMedia.getTrack(i);

                                if (track == null)
                                    continue;
                                if (track.type == Media.Track.Type.Video) {
                                    final Media.VideoTrack videoTrack = (Media.VideoTrack) track;
                                    w = videoTrack.width;
                                    h = videoTrack.height;
                                    break;
                                }
                            }

                        }
                    }
                    //libvlc_media_get_tracks_info

                }
                NTVService.record(mRecordState, w, h);

            }
        });

        btn[btnConnect].setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mConnectStatus == 1) {
                    SharedPreferences  settings = PreferenceManager.getDefaultSharedPreferences(MainActivity.this);
                    mServerAddress = settings.getString("ip_addr","");
                    mLoginKey = settings.getString("login_key","NTV_Key:no_key");

                    if(mServerAddress.equals("")) {
                        Toast.makeText(MainActivity.this, "You need to edit settings first!", Toast.LENGTH_LONG).show();
                        return;
                    }

                    btn[btnConnect].setImageResource(R.drawable.stop);
                    mConnectStatus = 0;
                    btn[btnRecord].setEnabled(true);


                    SetRcPanel(true);
                    SetServerPanel(true);

                    mRecordState = 0;

                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            Timer timer = new Timer(true);
                            timer.schedule(new infoTask(), 1000, 1000);
                            NTVService.start(mServerAddress,1234,1234, mLoginKey);
                            timer.cancel();
                            Message m = new Message();
                            m.what = 0;
                            mHandler.sendMessage(m);
                        }
                    }).start();
                }
                else { // disconnect

                    mRecordState = 0;
                    //btn[btnConnect].setImageResource(R.drawable.play);
                    btn[btnConnect].setEnabled(false);
                    btn[btnRecord].setEnabled(false);

                    SetRcPanel(false);
                    SetServerPanel(false);
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            NTVService.stop();

                        }
                    }).start();

                }

            }
        });

        mVideoSurface.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                float X = event.getX();
                float Y = event.getY();

                switch (event.getAction()) {

                    case MotionEvent.ACTION_DOWN: 
                        downX = event.getX();
                        downY = event.getY();
                        return true;
                    case MotionEvent.ACTION_MOVE: 

                        return true;
                    case MotionEvent.ACTION_UP: 
                        Log.d("onTouchEvent-ACTION_UP","UP");
                        upX = event.getX();
                        upY = event.getY();
                        float x=Math.abs(upX-downX);
                        float y=Math.abs(upY-downY);
                        double z=Math.sqrt(x*x+y*y);
                        int jiaodu=Math.round((float)(Math.asin(y/z)/Math.PI*180));

                        if (upY < downY && jiaodu>45) {//up

                        }else if(upY > downY && jiaodu>45) {//down

                        }else if(upX < downX && jiaodu<=45) {//left
                            if(z > 35)
                                NTVService.command("key:ch_up");
                        }else if(upX > downX && jiaodu<=45) {//right
                            if(z > 35)
                                NTVService.command("key:ch_down");
                        } else {
                            if(z < 20) {
                                if(mControl_panel.getVisibility() == View.VISIBLE ) {
                                    mControl_panel.setVisibility(View.INVISIBLE);
                                    mRc_panel.setVisibility(View.INVISIBLE);
                                    mServer_panel.setVisibility(View.INVISIBLE);
                                    textInfo.setVisibility(View.INVISIBLE);
                                } else {
                                    if(mDisplayServerPanel == true)
                                        mServer_panel.setVisibility(View.VISIBLE);

                                    if(mDisplayRcPanel == true)
                                        mRc_panel.setVisibility(View.VISIBLE);

                                    if(mDisplayInfo == true)
                                        textInfo.setVisibility(View.VISIBLE);

                                    mControl_panel.setVisibility(View.VISIBLE);

                                }
                            }
                        }
                        return true;
                }
                return true;
            }
        });

        mHandler = new Handler() {
            public void handleMessage(Message msg) {
                switch(msg.what){
                    case 0:
                        btn[btnConnect].setEnabled(true);
                        btn[btnConnect].setImageResource(R.drawable.play);
                        mConnectStatus = 1;
                        btn[btnRecord].setEnabled(false);


                        SetRcPanel(false);
                        SetServerPanel(false);
                        imageRecord.setVisibility(View.INVISIBLE);

                        break;
                    case 1:
                        String str=NTVService.info();
                        textInfo.setText(str);
                        if(mRecordState != 0) {
                            if(imageRecord.getVisibility() == View.VISIBLE) {
                                imageRecord.setVisibility(View.INVISIBLE);
                            }
                            else {
                                imageRecord.setVisibility(View.VISIBLE);
                                imageRecord.bringToFront();
                            }
                        }
                        break;
                }
                super.handleMessage(msg);
            }
        };

    }

    @Override
    public void onBackPressed() {
        finish();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (mMedia != null) {
            mMedia.release();
            mMedia = null;
        }

        if (mMediaPlayer != null) {
            mMediaPlayer.release();
            mMediaPlayer = null;
        }

        if (mLibVLC != null) {
            mLibVLC.release();
            mLibVLC = null;
        }
    }


    @Override
    public void onNewLayout(IVLCVout vlcVout, int width, int height, int visibleWidth, int visibleHeight, int sarNum, int sarDen) {
        mVideoWidth = width;
        mVideoHeight = height;
        mVideoVisibleWidth = visibleWidth;
        mVideoVisibleHeight = visibleHeight;
        mVideoSarNum = sarNum;
        mVideoSarDen = sarDen;
        updateVideoSurfaces();
    }


    @Override
    public void onSurfacesCreated(IVLCVout vlcVout) {
    }

    @Override
    public void onSurfacesDestroyed(IVLCVout vlcVout) {
    }






    private void updateVideoSurfaces() {

        return;
    }

    @Override
    public void onStart() {
        super.onStart();

        int cacheSize[] = {200,500,800,1100,1500};


        SharedPreferences  settings = PreferenceManager.getDefaultSharedPreferences(MainActivity.this);
        String addr = settings.getString("ip_addr","");

        int network_cacheing = settings.getInt("network_cacheing",2);
        if(network_cacheing < 0 || network_cacheing > 4)
            network_cacheing = 2;


        final IVLCVout vlcVout = mMediaPlayer.getVLCVout();
        vlcVout.setVideoView(mVideoSurface);

        vlcVout.attachViews();
        mMediaPlayer.getVLCVout().addCallback(this);

        mMedia = new Media(mLibVLC, Uri.parse("udp://@:1234"));
        mMedia.setHWDecoderEnabled(true, true);

        mMedia.addOption(":network-caching="+cacheSize[network_cacheing]);

        mMediaPlayer.setMedia(mMedia);
        mMediaPlayer.play();
        Log.i(tag, "play");

    }

    @Override
    public void onStop() {
        NTVService.stop();
        super.onStop();
        mMediaPlayer.stop();
        mMediaPlayer.getVLCVout().detachViews();
        mMediaPlayer.getVLCVout().removeCallback(this);
    }
}
