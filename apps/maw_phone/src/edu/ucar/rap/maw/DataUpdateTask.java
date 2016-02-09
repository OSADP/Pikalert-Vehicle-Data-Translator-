package edu.ucar.rap.maw;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.utils.URLEncodedUtils;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

public class DataUpdateTask extends AsyncTask<String, Integer, Long> {

	Activity _mainActivity;
	Controller _controller;
	
	protected Long doInBackground(String... urls) {
		try {
			// String url = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php?path=/maw_alerts&lat=40.035372&lon=-105.2538113&heading=null&speed=null&altitude=null&accuracy=42&prev_lat=40.0325912&prevLon=-105.2539772&session_id=1234asdf1234&prev_sequence_no=10";
			
			String url = urls[0];
			String paramString = urls[1];
			
			Log.i(getClass().getSimpleName(), "send  task - start");
			

//            maw_alerts&state=minnesota
//            		&lat=40.035372
//            		&lon=-105.2538113
//            		&heading=null
//            		&speed=null
//            		&altitude=null
//            		&accuracy=42
//            		&prev_lat=40.0325912
//            		&prevLon=-105.2539772
//            		&session_id=1234asdf1234
//            		&
//            		prev_sequence_no=10
            // Add params.
//			List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(4);
//			nameValuePairs.add(new BasicNameValuePair("path", "/maw_alerts"));
//			nameValuePairs.add(new BasicNameValuePair("lat", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("lon", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("heading", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("speed", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("altitude", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("accuracy", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("prev_lat", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("prev_lon", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("session_id", "xxx"));
//			nameValuePairs.add(new BasicNameValuePair("prev_sequence_no", "xxx"));
//			String paramString = URLEncodedUtils.format(nameValuePairs, "UTF-8");
			url += "?" + paramString;
			
			Log.i(getClass().getSimpleName(), "Requesting url: " + url);
			// Toast.makeText(_mainActivity, url, Toast.LENGTH_LONG).show();
			
			// Get the response...
			HttpGet httpget = new HttpGet(url);
			HttpClient httpclient = new DefaultHttpClient();
			HttpResponse response = httpclient.execute(httpget);
			HttpEntity entity = response.getEntity();
			String jsonResponse = null;
			if ( entity == null) {
				Log.i(getClass().getSimpleName(), "Could not get HttpEntity from response...");
				// Toast.makeText(_mainActivity, "Could not get HttpEntity from response...", Toast.LENGTH_LONG).show();
			}
			else {
				InputStream instream = entity.getContent();
				try {
					StringWriter writer = new StringWriter();
					IOUtils.copy(instream, writer, "UTF-8");
					jsonResponse = writer.toString();
			        
					Log.i(getClass().getSimpleName(), "Received: " + jsonResponse);
			        // Toast.makeText(_mainActivity, theString, Toast.LENGTH_LONG).show();
			        
				} catch (IOException ioe) {
					ioe.printStackTrace();
					// Toast.makeText(_mainActivity, "IOException: " + ioe, Toast.LENGTH_LONG).show();
				} finally {
					instream.close();
				}
			}
			
			// Parse
			if ( jsonResponse == null ) {
				_controller.handleAlertError("Got null JSON response from server.");
			}
			else {
				JSONObject jsonO = new JSONObject(jsonResponse);
				
				// Check for error string in response
	            try {
	            	String alert_error = jsonO.getString("error_string");
	            	_controller.handleAlertError(alert_error);
	            	return null;
	            } catch (JSONException err_ex) {
	            	Log.i(getClass().getSimpleName(), "Alert response contained no error string...");
	            }
	            
				String sessionIdRaw = jsonO.getString("session_id");
				String sequenceNoRaw = jsonO.getString("sequence_no");
				// String playAudioFlagRaw = jsonO.getString("play_audio_flag");
				// String alertIdRaw = jsonO.getString("alert_id");
				String alertGenTimeRaw = jsonO.getString("alert_gen_time");
				String alertRequestTimeRaw = jsonO.getString("alert_request_time");
				// String alertColorRaw = jsonO.getString("alert_color");
				// String dataTimeRaw = jsonTop.getString("data_time");
				int precipCode = jsonO.getInt("alert_code_precip");
				int pavementCode = jsonO.getInt("alert_code_pavement");
				int visibilityCode = jsonO.getInt("alert_code_visibility");
				int actionCode = jsonO.getInt("alert_action_code");
				
				int sequenceNo = -9999;
				try {
					sequenceNo = Integer.parseInt(sequenceNoRaw);
				} catch ( NumberFormatException e ) {
					Log.e(getClass().getSimpleName(), "Error. Sequence number from server does not contain a parsable integer: " + sequenceNo);
				}
				
				_controller.setSequenceNo(sequenceNo);
				_controller.setSessionId(sessionIdRaw);
				_controller.setLatestAlertGenTime(alertGenTimeRaw);
				_controller.setLatestAlertRequestTime(alertRequestTimeRaw);
				_controller.setLatestPrecipCode(precipCode);
				_controller.setLatestPavementCode(pavementCode);
				_controller.setLatestVisibilityCode(visibilityCode);
				_controller.setLatestActionCode(actionCode);
		    }
		
		} catch (ClientProtocolException e) {
		    // TODO Auto-generated catch block
		    e.printStackTrace();
		    return null;
		} catch (IOException e) {
		    // TODO Auto-generated catch block
		    e.printStackTrace();
		    return null;
		} catch (Throwable t) {
			t.printStackTrace();
//	        Toast.makeText(_mainActivity, "Request failed: " + t.toString(),
//	                Toast.LENGTH_LONG).show();
	    	return null;
	    
	    }
		
		return null;
	}
	
	public void setMainActivity(Activity act) {
		_mainActivity = act;
	}
	
	public void setController(Controller controller) {
		_controller = controller;
	}

	protected void onProgressUpdate(Integer... progress) {
        // setProgressPercent(progress[0]);
    }

    protected void onPostExecute(Long result) {
        // showDialog("Downloaded " + result + " bytes");
    	_controller.alertUpdateComplete(result);
    }

}
