package edu.ucar.rap.maw;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	private Switch _sw;
	private Controller _controller;
	private MediaPlayer _mediaPlayer;
	
	public final static String EXTRA_LOCATION = "edu.ucar.rap.maw.LOCATION";

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		final MainActivity me = this;
		if ( _controller == null ) {
			_controller = new Controller(this);
			// _controller.startLocationUpdates();
		}
		
		loadPref();
		
		// This is how to get the text off a text field.
		// text = (EditText) findViewById(R.string.visibility1);
		
		// Listen for changes on the main switch
		_sw = (Switch) findViewById(R.id.switch1);
		_sw.setOnCheckedChangeListener( new CompoundButton.OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				
				// Toast.makeText(me, "OnCheckedChangeListener()...", Toast.LENGTH_LONG).show();
				if ( _sw.isChecked() ) {
					_controller.startAlerts();
//					String url = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php";
//					DataLoginTask dlt = new DataLoginTask();
//					dlt.setMainActivity(me);
//					dlt.setController(_controller);
//					dlt.execute(url);
				}
				else {
					_controller.stopAlerts();
				}
			}
		});
	}
	
//	public void onMainSwitchClick(View view) {
//		// Toast.makeText(this, "Switch Clicked", Toast.LENGTH_LONG).show();
//		
//		Switch sw = (Switch) findViewById(R.id.switch1);
//		if ( sw.isChecked() ) {
//			_controller.startAlerts();
//		}
//		else {
//			_controller.stopAlerts();
//		}
//	}
	
	public void onGetLocationClick(View view) {
		_controller.getLocation();
	}
	
	public void onPlayAudioClick(View view) {
		_controller.playAlert(R.raw.audio_alert_1);
	}
	
	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		Intent intent = new Intent();
		intent.setClass(MainActivity.this, SettingsActivity.class);
		startActivityForResult(intent, 0); 
		
		return true;
	} 
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		loadPref();
	}

	private void loadPref() {
		SharedPreferences mySharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
				  
		String my_login_preference = mySharedPreferences.getString("user_name", "unknown");
		String strippedPhoneId = my_login_preference.replaceAll("\\s","");
		if ( strippedPhoneId.length() > 0 ) {
			// Use the user-specified phone number.
			Log.i(getClass().getSimpleName(), "Using phone number specified by the user in settings: " + strippedPhoneId);
		}
		else {
			// Use the phone number from the TelephonyManager
			strippedPhoneId = _controller.getPhoneNumberFromTelephony();
			Log.i(getClass().getSimpleName(), "Using phone number from telephony: " + strippedPhoneId);
			if ( strippedPhoneId.length() <= 0 ) {
				Log.i(getClass().getSimpleName(), "    Phone Id not available from telephony mgr or via settings.");
				Toast.makeText( getApplicationContext(), "WARNING: Cannot determine phone number from TelphonyManager. Please specify your phone number in Settings.", Toast.LENGTH_SHORT).show();
			}
		}
		TextView login = (TextView) findViewById(R.id.loginText);
		login.setText(strippedPhoneId);
		
		String my_state_preference = mySharedPreferences.getString("state", "unknown");
		String output = my_state_preference.substring(0, 1).toUpperCase() + my_state_preference.substring(1);
		TextView state = (TextView) findViewById(R.id.stateText);
		state.setText(output);
		if ( mySharedPreferences.getBoolean("use_test_datasource", false) ) 
			state.setText("Test Data");
	}
	
	/**
	 * Hack! No time to move all the initialization in onCreate() to (parent) Application class.
	 *  -- to fix: onCreate() call when orientation changes...
	 */
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
	}
}
