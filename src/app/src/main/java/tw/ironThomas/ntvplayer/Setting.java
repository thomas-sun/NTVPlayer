package tw.ironThomas.ntvplayer;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.google.zxing.activity.CaptureActivity;


public class Setting extends Activity {
    final String tag= "NTVPlayer";
    Button mBtnBack;
    Button mBtnScanBarcode;
    TextView mAddr;
    TextView mLoginKey;
    RadioButton mCachingLowest;
    RadioButton mCachingLow;
    RadioButton mCachingNormal;
    RadioButton mCachingHigh;
    RadioButton mCachingHigher;

    String NTVKey;


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if(resultCode == RESULT_OK) {
            String key = data.getExtras().getString("result");
            Log.i(tag,key);

            if(key.startsWith("NTV_Key:")) {
                SharedPreferences.Editor editor =
                        PreferenceManager.getDefaultSharedPreferences(Setting.this).edit();
                NTVKey = key;
                editor.putString("login_key",key);
                editor.commit();
                mLoginKey.setText(key);


            }





        }







    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setting);
        mBtnBack =(Button)this.findViewById(R.id.setting_back);
        mBtnScanBarcode =(Button)this.findViewById(R.id.scan_barcode);
        mAddr =(TextView)this.findViewById(R.id.txt_addr);
        mLoginKey =(TextView)this.findViewById(R.id.login_key);



        mCachingLowest =(RadioButton)this.findViewById(R.id.caching_lowest);
        mCachingLow =(RadioButton)this.findViewById(R.id.caching_low);
        mCachingNormal =(RadioButton)this.findViewById(R.id.caching_normal);
        mCachingHigh =(RadioButton)this.findViewById(R.id.caching_high);
        mCachingHigher =(RadioButton)this.findViewById(R.id.caching_higher);


        SharedPreferences  settings = PreferenceManager.getDefaultSharedPreferences(Setting.this);
        String addr = settings.getString("ip_addr","");
        mAddr.setText(addr);

        NTVKey = settings.getString("login_key","");
        mLoginKey.setText(NTVKey);




        int network_cacheing = settings.getInt("network_cacheing",2);
        if(network_cacheing < 0 || network_cacheing > 4)
            network_cacheing = 2;

        switch(network_cacheing) {
            case 0:
                mCachingLowest.setChecked(true);
                mCachingLow.setChecked(false);
                mCachingNormal.setChecked(false);
                mCachingHigh.setChecked(false);
                mCachingHigher.setChecked(false);
                break;
            case 1:
                mCachingLowest.setChecked(false);
                mCachingLow.setChecked(true);
                mCachingNormal.setChecked(false);
                mCachingHigh.setChecked(false);
                mCachingHigher.setChecked(false);
                break;
            case 2:
                mCachingLowest.setChecked(false);
                mCachingLow.setChecked(false);
                mCachingNormal.setChecked(true);
                mCachingHigh.setChecked(false);
                mCachingHigher.setChecked(false);
                break;
            case 3:
                mCachingLowest.setChecked(false);
                mCachingLow.setChecked(false);
                mCachingNormal.setChecked(false);
                mCachingHigh.setChecked(true);
                mCachingHigher.setChecked(false);
                break;
            case 4:
                mCachingLowest.setChecked(false);
                mCachingLow.setChecked(false);
                mCachingNormal.setChecked(false);
                mCachingHigh.setChecked(false);
                mCachingHigher.setChecked(true);
                break;
        }



        mBtnScanBarcode.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(Setting.this, CaptureActivity.class);
                startActivityForResult(intent, 1);

            }
        });

        mBtnBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                SharedPreferences.Editor editor =
                        PreferenceManager.getDefaultSharedPreferences(Setting.this).edit();

                String addr = mAddr.getText().toString();
                if(addr == null)
                    addr = "";
                editor.putString("ip_addr",addr);


                RadioGroup NetworkCaching =(RadioGroup)findViewById(R.id.network_caching);
                switch(NetworkCaching.getCheckedRadioButtonId()) {
                    case R.id.caching_lowest:
                        editor.putInt("network_cacheing", 0);
                        break;
                    case R.id.caching_low:
                        editor.putInt("network_cacheing", 1);
                        break;
                    case R.id.caching_normal:
                        editor.putInt("network_cacheing", 2);
                        break;
                    case R.id.caching_high:
                        editor.putInt("network_cacheing", 3);
                        break;
                    case R.id.caching_higher:
                        editor.putInt("network_cacheing", 4);
                        break;
                    default:
                        editor.putInt("network_cacheing", 2);
                        break;
                }

                editor.commit();



                finish();
            }
        });

    }

}
