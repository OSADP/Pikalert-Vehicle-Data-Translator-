package edu.ucar.rap.maw;

import java.io.IOException;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import org.apache.http.NameValuePair;
import org.apache.http.client.utils.URLEncodedUtils;
import org.apache.http.message.BasicNameValuePair;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

public class Controller {
	
	Activity _mainActivity;
	String _sessionId;
	int _sequenceNo = -9999;
	int _prevSequenceNo;
	String _latestAlertGenTime;
	String _latestAlertRequestTime;
	int _latestPrecipCode;
	int _latestPavementCode;
	int _latestVisibilityCode;
	int _latestActionCode = -9999;
	int _prevActionCode;
	
	Date _lastAudioPlayed;
	
	Location _location;
	
	MediaPlayer _mediaPlayer;
	boolean _isAudioPlaying = false;

	LinkedList<Integer> _pendingAudioResources = new LinkedList<Integer>();
	
	LocationManager  _locManager;
	LocationListener _locListener;
	
	String _phoneNumberFromTelephony;
	
	Timer _timer;
	MyTimerTask _timerTask;
	
	public final int TELEPHONY_SOURCE = 33;
	public final int SETTINGS_SOURCE = 44;
	public final int NEITHER_SOURCE = 55;
	
	public Controller(Activity activity) {
		_mainActivity = activity;
		
		// Hide the location fields.
		showLocationInfo(false);
		// showLocationInfo(true);

		// Called from owning object, now.
		// startLocationUpdates();
		
		// Todo: Don't forget to call stopLocationUpdates() somewhere!
		Context appContext = _mainActivity.getApplicationContext();
		TelephonyManager tMgr = (TelephonyManager)appContext.getSystemService(Context.TELEPHONY_SERVICE);
		_phoneNumberFromTelephony = tMgr.getLine1Number();
		if ( _phoneNumberFromTelephony.length() > 0 && _phoneNumberFromTelephony.startsWith("1") ) {
			_phoneNumberFromTelephony = _phoneNumberFromTelephony.substring(1);
		}
		Log.i(getClass().getSimpleName(), "Got phone number from TelephonyManager: " + _phoneNumberFromTelephony);
		
	}
	
	public void startAlerts() {
		startLocationUpdates();
		
		showLocationInfo(true);
		
		TextView alertTextView = (TextView) _mainActivity.findViewById(R.id.alertText);
		alertTextView.setText("Logging in...");
		
		requestLogin();
	}
	
	public void stopAlerts() {
		showLocationInfo(false);
		
		// Stop the alert update timer before logging out.
		if ( _timer != null ) 
			_timer.cancel();
		_timer = null;
		_timerTask = null;
		
		RelativeLayout mainLayout = (RelativeLayout) _mainActivity.findViewById(R.id.main);
		mainLayout.setBackgroundColor( _mainActivity.getResources().getColor(R.color.disabledColor) );
		TextView sessionTextView = (TextView) _mainActivity.findViewById(R.id.textSessionId);
		sessionTextView.setText("No Session");
		TextView sequenceTextView = (TextView) _mainActivity.findViewById(R.id.textSequenceNo);
		sequenceTextView.setText(String.valueOf("Alerts are Off"));
		TextView alertTextView = (TextView) _mainActivity.findViewById(R.id.alertText);
		alertTextView.setText(" ");
		
		requestLogout();
		clearPendingAudioAlerts();
	}
	
	public void setSessionId(String sessionId) {
		_sessionId = sessionId;
		Log.i(getClass().getSimpleName(), "Set session id on Controller: " + _sessionId);
//		TextView tv = (TextView) _mainActivity.findViewById(R.id.textSessionId);
//		tv.setText(_sessionId);
	}
	
	public void setSequenceNo(int newSeqNo) {
		if ( _sequenceNo > 0 ) {
			_prevSequenceNo = _sequenceNo;
		}
		else {
			// Should only happen once, to initialize the prev seq no.
			_prevSequenceNo = newSeqNo;
		}
		_sequenceNo = newSeqNo;
		
		Log.i(getClass().getSimpleName(), "Set sequence number on Controller: " + _sequenceNo);
//		TextView tv = (TextView) _mainActivity.findViewById(R.id.textSequenceNo);
//		tv.setText(_sequenceNo);
	}
	
	public void setLatestAlertGenTime(String time) {
		_latestAlertGenTime = time;
	}

	public void setLatestAlertRequestTime(String time) {
		_latestAlertRequestTime = time;
	}
	
    public void setLatestPrecipCode(int code) {
		_latestPrecipCode = code;
		Log.i(getClass().getSimpleName(), "Set precip code on Controller: " + _latestPrecipCode);
	}

    public void setLatestPavementCode(int code) {
		_latestPavementCode = code;
		Log.i(getClass().getSimpleName(), "Set pavement code on Controller: " + _latestPavementCode);
	}

    public void setLatestVisibilityCode(int code) {
		_latestVisibilityCode = code;
		Log.i(getClass().getSimpleName(), "Set visibility code on Controller: " + _latestVisibilityCode);
	}

    public void setLatestActionCode(int code) {
    	_prevActionCode = _latestActionCode;
    	_latestActionCode = code;
		Log.i(getClass().getSimpleName(), "Set action code on Controller: " + _latestActionCode);
	}
    
    public String getPhoneNumberFromTelephony() {
    	return _phoneNumberFromTelephony;
    }
	
	protected void requestLogin() {
		Log.i(getClass().getSimpleName(), "Requesting login...");
		
		final String url = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php";
		final Controller me = this;

		// Get state from the preferences.
		SharedPreferences mySharedPreferences = PreferenceManager.getDefaultSharedPreferences(_mainActivity);
		String my_state_preference = mySharedPreferences.getString("state", "unknown");
		
		String phoneId = getPhoneNumberString();
		if ( phoneId.length() <= 0 ) {
			// If we get no id at all, it means telephony failed and the settings are blank.
			Log.i(getClass().getSimpleName(), "    Phone Id not specified. Cannot log in. Resetting UI.");
			handleAlertError("Phone Id not specified. Use settings to enter a registered Phone Id.");
			return;
		}
		
        List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(5);
        nameValuePairs.add(new BasicNameValuePair("path", "/maw_login_dev"));
        nameValuePairs.add(new BasicNameValuePair("phone_id", phoneId));
        // nameValuePairs.add(new BasicNameValuePair("state", my_state_preference));
        nameValuePairs.add(new BasicNameValuePair("action", "login"));
        final String paramString = URLEncodedUtils.format(nameValuePairs, "UTF-8");
        
		_mainActivity.runOnUiThread((new Runnable() {
			public void run() {
				DataLoginTask loginRequest = new DataLoginTask();
				loginRequest.setController(me);
				loginRequest.setMainActivity(_mainActivity);
				loginRequest.execute(url, paramString);
			}
		}));
	}
	
	protected void requestLogout() {
		final String url = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php";
		final Controller me = this;

		String phoneId = getPhoneNumberString();
		if ( phoneId.length() <= 0 ) {
			// If we get no id at all, it means telephony failed and the settings are blank.
			Log.i(getClass().getSimpleName(), "    Phone Id not specified. Cannot log out. Resetting UI.");
			handleAlertError("Phone Id not specified. Use settings to enter a registered Phone Id.");
			return;
		}
		
        List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(4);
        nameValuePairs.add(new BasicNameValuePair("path", "/maw_login_dev"));
        nameValuePairs.add(new BasicNameValuePair("session_id", _sessionId));
        nameValuePairs.add(new BasicNameValuePair("phone_id", phoneId));
        nameValuePairs.add(new BasicNameValuePair("action", "logout"));
        final String paramString = URLEncodedUtils.format(nameValuePairs, "UTF-8");
        
		_mainActivity.runOnUiThread((new Runnable() {
			public void run() {
				DataLoginTask loginRequest = new DataLoginTask();
				loginRequest.setController(me);
				loginRequest.setMainActivity(_mainActivity);
				loginRequest.execute(url, paramString);
			}
		}));
	}
	
	protected void requestAlertUpdate() {
		final String url = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php";
		final Controller me = this;
		
		Location loc = getLocation();
		if ( loc == null ) {
			Toast.makeText( _mainActivity.getApplicationContext(), "Location Unknown. Skipping Alert", Toast.LENGTH_SHORT).show();
			
			RelativeLayout mainLayout = (RelativeLayout) _mainActivity.findViewById(R.id.main);
			mainLayout.setBackgroundColor( _mainActivity.getResources().getColor(R.color.disabledColor) );
			// TextView sessionTextView = (TextView) _mainActivity.findViewById(R.id.textSessionId);
			// sessionTextView.setText("No Session");
			// TextView sequenceTextView = (TextView) _mainActivity.findViewById(R.id.textSequenceNo);
			// sequenceTextView.setText(String.valueOf("Alerts are Off"));
			TextView alertTextView = (TextView) _mainActivity.findViewById(R.id.alertText);
			alertTextView.setText("Waiting for GPS location...");
			
			return;
		}

		String lat = String.valueOf( loc.getLatitude() );
		String lon = String.valueOf( loc.getLongitude() );
		
		// Get the state from the preferences.
		SharedPreferences mySharedPreferences = PreferenceManager.getDefaultSharedPreferences(_mainActivity);
		String my_state_preference = mySharedPreferences.getString("state", "unknown");
		
		boolean useTestData = mySharedPreferences.getBoolean("use_test_datasource", false);
		
		List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(6);
		if ( useTestData ) {
//			// Temporary -- until the login is moved out of the alert request
//			String phoneId = getPhoneNumberString();
//			if ( phoneId.length() <= 0 ) {
//				Log.i(getClass().getSimpleName(), "    Phone Id not specified. Cannot log in. Resetting UI.");
//				handleAlertError("Phone Id not specified. Use settings to enter a registered Phone Id.");
//				return;
//			}
			nameValuePairs.add(new BasicNameValuePair("path", "/maw_alerts"));
		}
		else {
			nameValuePairs.add(new BasicNameValuePair("path", "/maw_alerts_dev"));
		}

		String phoneId = getPhoneNumberString();
		if ( phoneId.length() <= 0 ) {
			Log.i(getClass().getSimpleName(), "    Phone Id not specified. Cannot log in. Resetting UI.");
			handleAlertError("Phone Id not specified. Use settings to enter a registered Phone Id.");
			return;
		}
        nameValuePairs.add(new BasicNameValuePair("phone_id", phoneId));
		nameValuePairs.add(new BasicNameValuePair("lat", lat));
		nameValuePairs.add(new BasicNameValuePair("lon", lon));
		nameValuePairs.add(new BasicNameValuePair("session_id", _sessionId));
		nameValuePairs.add(new BasicNameValuePair("prev_sequence_no", String.valueOf(_sequenceNo) ));
		nameValuePairs.add(new BasicNameValuePair("state", my_state_preference));
		final String paramString = URLEncodedUtils.format(nameValuePairs, "UTF-8");
		
		_mainActivity.runOnUiThread((new Runnable() {
			public void run() {
				DataUpdateTask loginRequest = new DataUpdateTask();
				loginRequest.setController(me);
				loginRequest.setMainActivity(_mainActivity);
				loginRequest.execute(url, paramString);
			}
		}));
	}
	
	protected void submitAlertConfirmation(String sessionId, int confirmedSequenceNo, String playedAudio) {
		final String url = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php";
		final Controller me = this;
		
		Location loc = getLocation();
		String lat = "0.0";
		String lon = "0.0";
		if ( loc != null ) {
			lat = String.valueOf( loc.getLatitude() );
			lon = String.valueOf( loc.getLongitude() );
		}
		
		List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(4);
		nameValuePairs.add(new BasicNameValuePair("path", "/maw_alerts_confirm"));
		nameValuePairs.add(new BasicNameValuePair("session_id", sessionId));
		nameValuePairs.add(new BasicNameValuePair("prev_sequence_no", String.valueOf(confirmedSequenceNo)));
		nameValuePairs.add(new BasicNameValuePair("played_audio", playedAudio));
		final String paramString = URLEncodedUtils.format(nameValuePairs, "UTF-8");
		
		_mainActivity.runOnUiThread((new Runnable() {
			public void run() {
				DataConfirmTask loginRequest = new DataConfirmTask();
				loginRequest.setController(me);
				loginRequest.setMainActivity(_mainActivity);
				loginRequest.execute(url, paramString);
			}
		}));
	}
	
	protected void loginComplete(Long result) {
		Log.i(getClass().getSimpleName(), " ");
		Log.i(getClass().getSimpleName(), "Login Complete: " + ((result == null) ? "null" : result.toString()) );
		Log.i(getClass().getSimpleName(), " ");

		// If the Alerts are turned off, do not start the alert timer (there may have been an error)
		Switch sw = (Switch) _mainActivity.findViewById(R.id.switch1);
		if ( !sw.isChecked() ) 
			return;
		
		// Start the alert update timer...
		_timer = new Timer();
		_timerTask = new MyTimerTask();
		_timer.scheduleAtFixedRate(_timerTask, 0, 15000);
	}
	
	protected void logoutComplete(Long result) {
		Log.i(getClass().getSimpleName(), " ");
		Log.i(getClass().getSimpleName(), "Logout Complete: " + ((result == null) ? "null" : result.toString()) );
		Log.i(getClass().getSimpleName(), " ");
		
		// Moved this to before the logout, to avoid alert requests during the logout process.
		// // Stop the alert update timer.
		// _timer.cancel();
		// _timer = null;
		// _timerTask = null;
		
		_lastAudioPlayed = null;
		_sequenceNo = -9999;
		_prevSequenceNo = -9999;
		_latestAlertGenTime = "";
		_latestAlertRequestTime = "";
		_latestPrecipCode = -9999;
		_latestPavementCode = -9999;
		_latestVisibilityCode = -9999;
		_latestActionCode = -9999;
		_prevActionCode = -9999;
	}
	
	protected void alertUpdateComplete(Long result) {
		Log.i(getClass().getSimpleName(), " ");
		Log.i(getClass().getSimpleName(), "Alert Update Complete: " + ((result == null) ? "null" : result.toString()) );
		Log.i(getClass().getSimpleName(), " ");
		
		// If the Alerts are turned off, cancel this UI update (there may have been an error)
		Switch sw = (Switch) _mainActivity.findViewById(R.id.switch1);
		if ( !sw.isChecked() ) 
			return;
		
		// Update the GUI
		TextView sessionTextView = (TextView) _mainActivity.findViewById(R.id.textSessionId);
		sessionTextView.setText(_sessionId);
		TextView sequenceTextView = (TextView) _mainActivity.findViewById(R.id.textSequenceNo);
		sequenceTextView.setText(String.valueOf(_sequenceNo));
		RelativeLayout mainLayout = (RelativeLayout) _mainActivity.findViewById(R.id.main);
		
		mainLayout.setBackgroundColor( _mainActivity.getResources().getColor(Lookup.getColorResForAction(_latestActionCode)) );
		TextAudioResourcePair precipPair = Lookup.getResourcesForPrecip(_latestPrecipCode);
		TextAudioResourcePair pavementPair = Lookup.getResourcesForPavement(_latestPavementCode);
		TextAudioResourcePair visibilityPair = Lookup.getResourcesForVisibility(_latestVisibilityCode);
		TextAudioResourcePair actionPair = Lookup.getResourcesForAction(_latestActionCode);
		// mainLayout.getBackground().setColorFilter(Color.parseColor("#00ff00"), PorterDuff.Mode.DARKEN);
		// mainLayout.setBackgroundColor( Color.parseColor(_latestAlertColor) );
		
		// Play the audio, if warranted.
		//  - Always play if the latest action code is higher than the previous one
		//  - Play an alert every 15 minutes
		Date now = new Date();
		boolean playAudio = false;
		if ( _latestActionCode > _prevActionCode ) {
			playAudio = true;
			_lastAudioPlayed = new Date();
		}
		else {
			// Check the timer and play audio if 15 minutes have passed.
			if ( _lastAudioPlayed == null || now.getTime() - _lastAudioPlayed.getTime() > 900000 ) {
			// Todo: Change back to 15 mins, above!
			// if ( _lastAudioPlayed == null || now.getTime() - _lastAudioPlayed.getTime() > 60000 ) {
				playAudio = true;
			}
			else {
				Log.i(getClass().getSimpleName(), "Skipping audio because only " + (now.getTime() - _lastAudioPlayed.getTime()) + " ms have elapsed since the same level of alert." );
			}
		}
		
		// Create the text alert
		boolean capLetter = true;
		boolean foundSomething = false;
		String textAlert = new String();
		if ( _latestPrecipCode > 0 ) {
			String a = _mainActivity.getResources().getText( precipPair.getTextResId() ).toString();
			String output = a.substring(0, 1).toUpperCase() + a.substring(1);
			textAlert += output + " ahead. ";
			foundSomething = true;
		}
		if ( _latestPavementCode > 0 ) {
			String a = _mainActivity.getResources().getText( pavementPair.getTextResId() ).toString();
			String output = a;
			if ( capLetter ) {
				output = a.substring(0, 1).toUpperCase() + a.substring(1);
				capLetter = false;
			}
			textAlert += output;
			if ( !foundSomething ) 
				textAlert += " ahead. ";
			else
				textAlert += ", ";
			
			foundSomething = true;
		}
		if ( _latestVisibilityCode > 0 ) {
			String a = _mainActivity.getResources().getText( visibilityPair.getTextResId() ).toString();
			String output = a;
			if ( capLetter ) {
				output = a.substring(0, 1).toUpperCase() + a.substring(1);
				capLetter = false;
			}
			textAlert += output;
			if ( !foundSomething ) 
				textAlert += " ahead. ";
			else
				textAlert += ", ";
			
			foundSomething = true;
		}
		if ( _latestActionCode > 0 ) {
			if ( foundSomething ) {
				// Change prev comma-space to a period-space
				String prev = textAlert.substring(0, textAlert.length() - 2);
				textAlert = prev + ". ";
			}
			
			String a = _mainActivity.getResources().getText( actionPair.getTextResId() ).toString();
			String output = a.substring(0, 1).toUpperCase() + a.substring(1);
			textAlert += output;
			if ( !foundSomething ) 
				textAlert += " ahead.";
			else
				textAlert += ".";
			
			foundSomething = true;
		}
		else {
			// No action code. Fix any trailing comma.
			if ( foundSomething ) {
				// Change prev comma-space to a period-space
				String prev = textAlert.substring(0, textAlert.length() - 2);
				textAlert = prev + ".";
			}
		}
		TextView alertTextView = (TextView) _mainActivity.findViewById(R.id.alertText);
		
		if ( !foundSomething ) textAlert = "No alert.";
		if ( _latestActionCode == -9999 ) textAlert = "Out of range. Device is not near any monitored road segments.";
		
		alertTextView.setText(textAlert);

		// Todo: Take this out!
		// playAudio = true;
		
		boolean foundAudio = false;
		if ( playAudio && foundSomething && _latestActionCode > 0 ) {
			if ( _latestPrecipCode > 0 ) {
				playAlert( precipPair.getAudioResId() );
				playAlert( R.raw.ahead );
				
				foundAudio = true;
			}
			if ( _latestPavementCode > 0 ) {
				playAlert( pavementPair.getAudioResId() );
				if ( !foundAudio ) {
					playAlert( R.raw.ahead );
				}
				
				foundAudio = true;
			}
			if ( _latestVisibilityCode > 0 ) {
				playAlert( visibilityPair.getAudioResId() );
				if ( !foundAudio ) {
					playAlert( R.raw.ahead );
				}
				
				foundAudio = true;
			}
			if ( _latestActionCode > 0 ) {
				playAlert( actionPair.getAudioResId() );
				if ( !foundAudio ) {
					playAlert( R.raw.ahead );
				}
				
				foundAudio = true;
			}
			
			_lastAudioPlayed = new Date();
		}
		
		
		submitAlertConfirmation( _sessionId, _sequenceNo, (playAudio ? "t" : "f") );
	}
	
	protected Location getLocation() {
		// 
		// This does NOT work!!!
		// 
		
//		// Determine the location
//		// _tv = (TextView) findViewById(R.id.locationText1);
//		LocationManager mlocManager = (LocationManager)_mainActivity.getSystemService(Context.LOCATION_SERVICE);
//		Criteria criteria = new Criteria();
//		String provider = mlocManager.getBestProvider(criteria, false);
//		Location location = mlocManager.getLastKnownLocation(provider);
//		
//		// Initialize the location fields
//		if (location != null) {
//			String msg = "Provider " + provider + " has been selected.";
//			Toast.makeText( _mainActivity.getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
//			
//			msg = "Location: Lon: [" + location.getLongitude() + "] Lat: [" + location.getLatitude() + "]";
//			Toast.makeText( _mainActivity.getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
//		} else {
//			Toast.makeText( _mainActivity.getApplicationContext(), "Cannot Determine Location...", Toast.LENGTH_SHORT).show();
//		}
//		
//		return location;
//		
//		// Register listener for location updates...
//		// LocationListener mlocListener = new MyLocationListener();
//		// mlocManager.requestLocationUpdates( LocationManager.GPS_PROVIDER, 0, 0, mlocListener);
		return _location;
	}
	
	public synchronized void playAlert(int alertResourceId) {
		if ( _mediaPlayer == null ) {
			_mediaPlayer = new MediaPlayer();
			_mediaPlayer.setOnCompletionListener( new MyOnCompletionListener(this) );
		}
		
		_pendingAudioResources.add(new Integer(alertResourceId));
		
		if ( !isAudioPlaying() ) {
			
			setAudioPlaying(true);
			try {
				String uriPath="android.resource://" + _mainActivity.getPackageName() + "/" + alertResourceId;
				Uri uri = Uri.parse(uriPath);
				_mediaPlayer.reset();
				_mediaPlayer.setDataSource(_mainActivity, uri);
				_mediaPlayer.prepare();
				_mediaPlayer.setLooping(false);
				_mediaPlayer.start();
				
				
			} catch (IllegalArgumentException iae) {
				Log.i(getClass().getSimpleName(), "Illegal Argument: " + iae);
			} catch (IllegalStateException ise) {
				Log.i(getClass().getSimpleName(), "Illegal State: " + ise);
			} catch (IOException ioe) {
				Log.i(getClass().getSimpleName(), "IOException: " + ioe);
			}
		}
		
	}
	
	public synchronized void clearPendingAudioAlerts() {
		_pendingAudioResources.clear();
	}
	
	public void startLocationUpdates() {
		
		_locManager = (LocationManager)_mainActivity.getSystemService(Context.LOCATION_SERVICE);
		// Criteria criteria = new Criteria();
		// String provider = mlocManager.getBestProvider(criteria, false);
		// Location location = mlocManager.getLastKnownLocation(provider);
		_locListener = new MyLocationListener();
		_locManager.requestLocationUpdates( LocationManager.GPS_PROVIDER, 1000, 0, _locListener); // min update time in msec
	}
	
	public void stopLocationUpdates() {
		
		_locManager.removeUpdates(_locListener);
	}
	
	public synchronized boolean isAudioPlaying() {
		return _isAudioPlaying;
	}

	public synchronized void setAudioPlaying(boolean isAudioPlaying) {
		_isAudioPlaying = isAudioPlaying;
	}
	
	public void showLocationInfo(boolean show) {
		TextView locLabTV = (TextView) _mainActivity.findViewById(R.id.locationLabel);
		TextView lonLabTV = (TextView) _mainActivity.findViewById(R.id.longitudeLabel);
		TextView latLabTV = (TextView) _mainActivity.findViewById(R.id.latitudeLabel);
		TextView lonTV = (TextView) _mainActivity.findViewById(R.id.lon);
		TextView latTV = (TextView) _mainActivity.findViewById(R.id.lat);
		locLabTV.setVisibility(show? View.VISIBLE : View.INVISIBLE );
		lonLabTV.setVisibility(show? View.VISIBLE : View.INVISIBLE );
		latLabTV.setVisibility(show? View.VISIBLE : View.INVISIBLE );
		lonTV.setVisibility(show? View.VISIBLE : View.INVISIBLE );
		latTV.setVisibility(show? View.VISIBLE : View.INVISIBLE );
	}
	
	public class MyOnCompletionListener implements MediaPlayer.OnCompletionListener {
		Controller _controller;
		
		public MyOnCompletionListener() {
			throw new RuntimeException("Default constructor not allowed for MyOnCompletionListener --> Must initialize with a Controller object.");
		}
		
		public MyOnCompletionListener(Controller c) {
			_controller = c;
		}
		
		@Override
		public void onCompletion(MediaPlayer mp) {
			synchronized(_controller) {
				// pop the one that was just played
				Integer resourceId = _pendingAudioResources.remove();
				
				// play any remaining files.
				if ( _pendingAudioResources.size() > 0 ) {
					resourceId = _pendingAudioResources.peek();
					String uriPath="android.resource://" + _mainActivity.getPackageName() + "/" + resourceId.intValue();
					Uri uri = Uri.parse(uriPath);
					try {
						_mediaPlayer.reset();
						_mediaPlayer.setDataSource(_mainActivity, uri);
						_mediaPlayer.prepare();
						_mediaPlayer.setLooping(false);
						_mediaPlayer.start();
					} catch (IllegalArgumentException iae) {
						Log.i(getClass().getSimpleName(), "Illegal Argument: " + iae);
					} catch (IllegalStateException ise) {
						Log.i(getClass().getSimpleName(), "Illegal State: " + ise);
					} catch (IOException ioe) {
						Log.i(getClass().getSimpleName(), "IOException: " + ioe);
					}
				}
				else {
					setAudioPlaying(false);
				}
			}
		}
	}
	
	/**
	 * Class for listening for location change events.
	 * @author paddy
	 *
	 */
	public class MyLocationListener implements LocationListener
	{
		@Override
		public void onLocationChanged(Location loc)
		{
			Log.i(getClass().getSimpleName(), " ");
			Log.i(getClass().getSimpleName(), "Received onLocationChanged...");
			Log.i(getClass().getSimpleName(), " ");
			
			_location = loc;
			String lonText = "Unknown";
			String latText = "Unknown";
			
			if ( _location != null ) {
			
				double lon = loc.getLongitude();
				double lat = loc.getLatitude();
				
				// Todo: Don't post these Toast messages...
				// String Text = "My current location is: " + "Latitude = " + loc.getLatitude() + "Longitude = " + loc.getLongitude();
				// Toast.makeText( _mainActivity.getApplicationContext(), Text, Toast.LENGTH_SHORT).show();
				
				NumberFormat n = NumberFormat.getInstance();
			    n.setMaximumFractionDigits(5);
				lonText = n.format(lon);
				latText = n.format(lat);
			}

			TextView lonTv = (TextView) _mainActivity.findViewById(R.id.lon);
			TextView latTv = (TextView) _mainActivity.findViewById(R.id.lat);
			lonTv.setText(lonText);
			latTv.setText(latText);
		}
	
		@Override
		public void onProviderDisabled(String provider)
		{
			Toast.makeText( _mainActivity.getApplicationContext(), "Gps Disabled", Toast.LENGTH_SHORT ).show();
		}
	
		@Override
		public void onProviderEnabled(String provider)
		{
			Toast.makeText( _mainActivity.getApplicationContext(), "Gps Enabled", Toast.LENGTH_SHORT).show();
		}
	
		@Override
		public void onStatusChanged(String provider, int status, Bundle extras)
		{
		}
	}
	
	private class MyTimerTask extends TimerTask {
		@Override
		public void run() {
			_mainActivity.runOnUiThread(new Runnable() {
				@Override
				public void run() {
					// Perform an alert update
					requestAlertUpdate();
				}
			});
		}
	}
	
	/**
	 * Get the phone number from the appropriate source:
	 *  1) First try the settings, in case the user has overridden the device number
	 *  2) Then try the TelephonyManager
	 *  @return The string representing the phone number, or an empty string if the number can't be found.
	 */
	public String getPhoneNumberString() {
		String no = "";
		
		SharedPreferences mySharedPreferences = PreferenceManager.getDefaultSharedPreferences(_mainActivity);
		String my_login_preference = mySharedPreferences.getString("user_name", "unknown");
		String strippedPhoneId = my_login_preference.replaceAll("\\s","");
		if ( strippedPhoneId.length() > 0 ) {
			// Use the user-specified phone number.
			Log.i(getClass().getSimpleName(), "    Using phone number specified by the user in settings: " + strippedPhoneId);
		}
		else {
			// Use the phone number from the TelephonyManager
			strippedPhoneId = _phoneNumberFromTelephony;
			
			if ( strippedPhoneId == null || strippedPhoneId.length() <= 0 ) {
				Log.i(getClass().getSimpleName(), "    Phone Id not available via settings or telephony");
				strippedPhoneId = "";
			}
			else {
				Log.i(getClass().getSimpleName(), "    Using phone number from telephony: " + strippedPhoneId);
			}
		}
		
		return strippedPhoneId;
	}
	
	/**
	 * Get the phone number source:
	 *  1) First try the settings, in case the user has overridden the device number
	 *  2) Then try the TelephonyManager
	 *  @return The string representing the phone number, or an empty string if the number can't be found.
	 */
	public int getPhoneNumberSource() {
		String no = "";
		
		SharedPreferences mySharedPreferences = PreferenceManager.getDefaultSharedPreferences(_mainActivity);
		String my_login_preference = mySharedPreferences.getString("user_name", "unknown");
		String strippedPhoneId = my_login_preference.replaceAll("\\s","");
		int source = NEITHER_SOURCE;
		if ( strippedPhoneId.length() > 0 ) {
			// Use the user-specified phone number.
			source = SETTINGS_SOURCE;
		}
		else {
			// Use the phone number from the TelephonyManager
			strippedPhoneId = _phoneNumberFromTelephony;
			
			if ( strippedPhoneId == null || strippedPhoneId.length() <= 0 ) {
				Log.i(getClass().getSimpleName(), "    Phone Id not available via settings or telephony");
				source = NEITHER_SOURCE;
			}
			else {
				Log.i(getClass().getSimpleName(), "    Using phone number from telephony: " + strippedPhoneId);
				source = TELEPHONY_SOURCE;
			}
		}
		
		return source;
	}
	
	public String formatPhoneNumberMsg() {
		int source = getPhoneNumberSource();
		String msg;
		if ( source == SETTINGS_SOURCE ) {
			msg = "the phone id in your settings (" + getPhoneNumberString() + ") is not registered.";
		}
		else if ( source == TELEPHONY_SOURCE ) {
			msg = "register your phone or use settings to enter a registered phone id";
		}
		else {
			// Neither
			msg = "use settings to enter a registered phone id";
		}
		
		return msg;
	}
	
	/**
	 * Post a message and shut the app down:
	 *  - Turn off alert requests
	 *  - Post the message as the alert text
	 *  - Hide the location info display
	 * @param msg
	 */
	public void handleAlertError(final String msg) {
		_mainActivity.runOnUiThread((new Runnable() {
			public void run() {
				showLocationInfo(false);
				Switch sw = (Switch) _mainActivity.findViewById(R.id.switch1);
				sw.setChecked(false);
				
				String finalMsg = msg;
				if ( msg.contains("phone") && msg.contains("not registered") ) {
					finalMsg = msg + ". " + formatPhoneNumberMsg() + ".";
				}
				TextView alertTextView = (TextView) _mainActivity.findViewById(R.id.alertText);
				alertTextView.setText(finalMsg);
				Toast.makeText( _mainActivity.getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
			}
		}));
	}
}
