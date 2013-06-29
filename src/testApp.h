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
	
	void setupAudioGraph(string ringToneFile);
	void setupArduino(int baud);
	
	int getReading(int sensor);
	
	ofxAudioUnitInput input;
	ofxAudioUnitFilePlayer ringTone;
	ofxAudioUnitMatrixMixer mixer;
	ofxAudioUnitOutput output;
	
	ofxAudioUnitTap inputTap, outputTap;
	
	ofPolyline leftOutWaveform, rightOutWaveform;
	ofPolyline leftInWaveform, rightInWaveform;
	
	ofSerial arduino;
	dispatch_queue_t serialQueue;
	dispatch_source_t serialTimer;
	
	uint8_t arduinoReading;
};
