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
import org.json.JSONObject;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

public class DataConfirmTask extends AsyncTask<String, Integer, Long> {

	Activity _mainActivity;
	Controller _controller;
	
	protected Long doInBackground(String... urls) {
        try {
	        // String url = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php?path=/maw_alerts_confirm&session_id=1234asdf1234&prev_sequence_no=10&played_audio=t";
	        
        	String url = urls[0];
        	String paramString = urls[1];
        	
            Log.i(getClass().getSimpleName(), "confirm task - start");

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
            if ( jsonResponse != null ) {
	            JSONObject jsonO = new JSONObject(jsonResponse);
	            String sessionId = jsonO.getString("session_id");
	            String sequenceNo = jsonO.getString("confirmed_sequence_no");
	            String responseText = jsonO.getString("response");

	        	Log.i(getClass().getSimpleName(), "    Received response from confirmation:" + responseText);
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
    	Log.i(getClass().getSimpleName(), "Done submitting confirmation...");
    }

}
