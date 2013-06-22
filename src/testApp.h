#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"

class testApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	void setupAudioGraph(string ringToneFile, bool muteInput);
	void setupArduino(int baud);
	
	ofxAudioUnitInput input;
	ofxAudioUnitFilePlayer ringTone;
	ofxAudioUnitMixer mixer;
	ofxAudioUnitOutput output;
	
	ofxAudioUnitTap inputTap, outputTap;
	
	ofPolyline leftOutWaveform, rightOutWaveform;
	ofPolyline leftInWaveform, rightInWaveform;
	
	ofSerial arduino;
};
