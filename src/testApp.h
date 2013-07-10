#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"

class testApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed (int key);
	void setupAudioGraph(const string &ringToneFile);
	
	ofxAudioUnitInput input;
	ofxAudioUnitFilePlayer ringTone;
	ofxAudioUnitMatrixMixer mixer;
	ofxAudioUnitOutput output;
	ofxAudioUnit reverb;
	
	ofxAudioUnitTap inputTap, outputTap;
	
	ofPolyline leftOutWaveform, rightOutWaveform;
	ofPolyline leftInWaveform, rightInWaveform;
	
	float lastActivation;
};
