package edu.ucar.rap.maw;

public class TextAudioResourcePair {

	int _textResId;
	int _audioResId;
	
	public TextAudioResourcePair(int textRes, int audioRes) {
		setTextResId(textRes);
		setAudioResId(audioRes);
	}
	
	public void setTextResId( int id ) {
		_textResId = id;
	}

	public int getTextResId() {
		return _textResId;
	}
	
	public void setAudioResId( int id ) {
		_audioResId = id;
	}

	public int getAudioResId() {
		return _audioResId;
	}
}
