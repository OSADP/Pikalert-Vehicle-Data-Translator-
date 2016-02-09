package edu.ucar.rap.maw;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;

import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

public class DataLoginTask extends AsyncTask<String, Integer, Long> {

	Activity _mainActivity;
	Controller _controller;
	boolean _isLogin = true;
	
	protected Long doInBackground(String... urls) {
        try {
	        // String loginUrl = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php?path=/maw_login.json&user=pad&password=padpass&action=login";
	        // String logoutUrl = "http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php?path=/maw_login.json&session_id=1234asdf1234&action=logout";
        	
        	String url = urls[0];
        	String paramString = urls[1];
        	
            Log.i(getClass().getSimpleName(), "send  task - start");
            
            // Add params.
            url += "?" + paramString;
            
            // Login or logout?
            if ( paramString.contains("logout") ) {
            	_isLogin = false;
            }
            
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
            	_controller.handleAlertError("Got null JSON response from login request.");
            }
            else {
	            JSONObject jsonO = new JSONObject(jsonResponse);
	            
	            // Check for error string in response
	            try {
	            	String login_error = jsonO.getString("error_string");
	            	_controller.handleAlertError(login_error);
	            	return null;
	            } catch (JSONException err_ex) {
	            	Log.i(getClass().getSimpleName(), "Login response contained no error string...");
	            }
	            
	            String sessionId = jsonO.getString("session_id");
	            String sequenceNo = jsonO.getString("sequence_no");
	            
	            _controller.setSessionId(sessionId);
	            
	            int parsedInt = -9999;
	            try {
	            	parsedInt = Integer.parseInt(sequenceNo);
	            } catch ( NumberFormatException e ) {
	            	Log.e(getClass().getSimpleName(), "Error. Sequence number from server does not contain a parsable integer: " + sequenceNo);
	            }
	            
	            _controller.setSequenceNo(parsedInt);
	            
	            /* 
	            JSONArray jArray = json.getJSONArray("posts");
	            ArrayList<HashMap<String, String>> mylist = 
	                   new ArrayList<HashMap<String, String>>();
	
	            for (int i = 0; i < jArray.length(); i++) {
	                HashMap<String, String> map = new HashMap<String, String>();
	                JSONObject e = jArray.getJSONObject(i);
	                String s = e.getString("post");
	                JSONObject jObject = new JSONObject(s);
	
	                map.put("idusers", jObject.getString("idusers"));
	                map.put("UserName", jObject.getString("UserName"));
	                map.put("FullName", jObject.getString("FullName"));
	
	                mylist.add(map);
	            }
	            */
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
    	if ( _isLogin ) 
    		_controller.loginComplete(result);
    	else
    		_controller.logoutComplete(result);
    }

}
