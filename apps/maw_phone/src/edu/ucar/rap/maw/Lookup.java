package edu.ucar.rap.maw;

import android.app.Activity;

public class Lookup {
	
	/**
	 * @param args
	 */
	public static int getColorResForAction(int actionCode) {
		int colorResource = 0;
		switch(actionCode) {
			case -9999:
				colorResource = R.color.unknownColor;
				break;
			case 0:
				colorResource = R.color.greenAlertColor;
				break;
			case 1:
				colorResource = R.color.yellowAlertColor;
				break;
			case 2:
				colorResource = R.color.orangeAlertColor;
				break;
			case 3:
				colorResource = R.color.redAlertColor;
				break;
			case 10:
				colorResource = R.color.redAlertColor;
				break;
			default:
				colorResource = R.color.disabledColor;
				break;
		}
		
		return colorResource;
	}

	/**
	 * @param args
	 */
	public static boolean getPlayAudioForAction(int actionCode) {
		boolean playAudio = false;
		switch(actionCode) {
			case -9999:
				playAudio = false;
				break;
			case 0:
				playAudio = false;
				break;
			case 1:
				playAudio = true;
				break;
			case 2:
				playAudio = true;
				break;
			case 3:
				playAudio = true;
				break;
			case 10:
				playAudio = true;
				break;
			default:
				playAudio = false;
				break;
		}
		
		return playAudio;
	}
	
	/**
	 * @param args
	 */
	public static TextAudioResourcePair getResourcesForPrecip(int precipCode) {
		switch(precipCode) {
			case -9999:return new TextAudioResourcePair(R.string.unknownAlert, R.raw.unknown);
			case 0: return new TextAudioResourcePair(R.string.precip0, R.raw.precip0);
			case 1: return new TextAudioResourcePair(R.string.precip1, R.raw.precip1);
			case 2: return new TextAudioResourcePair(R.string.precip2, R.raw.precip2);
			case 3: return new TextAudioResourcePair(R.string.precip3, R.raw.precip3);
			case 4: return new TextAudioResourcePair(R.string.precip4, R.raw.precip4);
			case 5: return new TextAudioResourcePair(R.string.precip5, R.raw.precip5);
			case 6: return new TextAudioResourcePair(R.string.precip6, R.raw.precip6);
			case 7: return new TextAudioResourcePair(R.string.precip7, R.raw.precip7);
			case 8: return new TextAudioResourcePair(R.string.precip8, R.raw.precip8);
			case 9: return new TextAudioResourcePair(R.string.precip9, R.raw.precip9);
			case 10: return new TextAudioResourcePair(R.string.precip10, R.raw.precip10);
			default: return new TextAudioResourcePair(R.string.precip0, R.raw.unknown);
		}
	}
	
	/**
	 * @param args
	 */
	public static TextAudioResourcePair getResourcesForPavement(int pavementCode) {
		switch(pavementCode) {
			case -9999:return new TextAudioResourcePair(R.string.unknownAlert, R.raw.unknown);
			case 0: return new TextAudioResourcePair(R.string.pavement0, R.raw.pavement0);
			case 1: return new TextAudioResourcePair(R.string.pavement1, R.raw.pavement1);
			case 2: return new TextAudioResourcePair(R.string.pavement2, R.raw.pavement2);
			case 3: return new TextAudioResourcePair(R.string.pavement3, R.raw.pavement3);
			case 4: return new TextAudioResourcePair(R.string.pavement4, R.raw.pavement4);
			case 5: return new TextAudioResourcePair(R.string.pavement5, R.raw.pavement5);
			case 6: return new TextAudioResourcePair(R.string.pavement6, R.raw.pavement6);
			case 7: return new TextAudioResourcePair(R.string.pavement7, R.raw.pavement7);
			case 8: return new TextAudioResourcePair(R.string.pavement8, R.raw.pavement8);
			case 9: return new TextAudioResourcePair(R.string.pavement9, R.raw.pavement9);
			default: return new TextAudioResourcePair(R.string.pavement0, R.raw.unknown);
		}
	}
	
	/**
	 * @param args
	 */
	public static TextAudioResourcePair getResourcesForVisibility(int visibilityCode) {
		switch(visibilityCode) {
			case -9999:return new TextAudioResourcePair(R.string.unknownAlert, R.raw.unknown);
			case 0: return new TextAudioResourcePair(R.string.visibility0, R.raw.visibility0);
			case 1: return new TextAudioResourcePair(R.string.visibility1, R.raw.visibility1);
			case 2: return new TextAudioResourcePair(R.string.visibility2, R.raw.visibility2);
			case 3: return new TextAudioResourcePair(R.string.visibility3, R.raw.visibility3);
			case 4: return new TextAudioResourcePair(R.string.visibility4, R.raw.visibility4);
			case 5: return new TextAudioResourcePair(R.string.visibility5, R.raw.visibility5);
			case 6: return new TextAudioResourcePair(R.string.visibility6, R.raw.visibility6);
			default: return new TextAudioResourcePair(R.string.visibility0, R.raw.unknown);
		}
	}
	
	/**
	 * @param args
	 */
	public static TextAudioResourcePair getResourcesForAction(int actionCode) {
		switch(actionCode) {
			case -9999:return new TextAudioResourcePair(R.string.unknownAlert, R.raw.unknown);
			case 0: return new TextAudioResourcePair(R.string.action0, R.raw.action0);
			case 1: return new TextAudioResourcePair(R.string.action1, R.raw.action1);
			case 2: return new TextAudioResourcePair(R.string.action2, R.raw.action2);
			case 3: return new TextAudioResourcePair(R.string.action3, R.raw.action3);
			case 10: return new TextAudioResourcePair(R.string.action10, R.raw.action10);
			default: return new TextAudioResourcePair(R.string.action0, R.raw.unknown);
		}
	}
	
	
}
