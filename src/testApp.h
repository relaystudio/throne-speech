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
	void gotMessage(ofMessage msg);
	
	ofxAudioUnitInput input;
	ofxAudioUnitFilePlayer ringTone;
	ofxAudioUnitMixer mixer;
	ofxAudioUnitOutput output;
	
	ofxAudioUnitTap debugTap;
	ofPolyline leftWaveform, rightWaveform;
};
