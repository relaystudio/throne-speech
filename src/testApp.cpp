#include "testApp.h"
#include "ofxAudioUnitUtils.h"

enum {
	kMicBus  = 0,
	kRingBus = 1
};

const float interactionTimeout = 15;
const float threshold = 0.1;

const string ringTonePath = "sound/nokia.wav";
const string reverbPreset = "reverb";
const string eqPreset = "eq";

void testApp::setup() {
	setupAudioGraph(ringTonePath);
	ofSetVerticalSync(true);
	ofBackground(50);
}

void testApp::setupAudioGraph(const string &ringTonePath) {
	reverb = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
	ringTone.setFile(ofFilePath::getAbsolutePath(ringTonePath));
	ringTone.loop();
	
	output.setDevice("Built-in Output");
	
	mixer.setChannelLayout(2, 1);
	input.connectTo(inputTap).connectTo(mixer, kMicBus);
	ringTone.connectTo(mixer, kRingBus);
	
	mixer.connectTo(outputTap).connectTo(output);
	
	inputTap.setBufferLength(1024);
	outputTap.setBufferLength(512);
	
	input.start();
	output.start();
}

void testApp::update() {
	
	// get debug waveforms
	ofVec2f waveSize(ofGetWidth() / 2., ofGetHeight() / 2.);
	inputTap.getStereoWaveform(leftInWaveform, rightInWaveform, waveSize.x, waveSize.y);
	outputTap.getStereoWaveform(leftOutWaveform, rightOutWaveform, waveSize.x, waveSize.y);
	
	// to make sense of the following, see the docs on kAudioUnitProperty_MatrixLevels
	const size_t inChannels  = 4;
	const size_t outChannels = 2;
	float levels[inChannels + 1][outChannels + 1] = {0};
	
	// setting global volume to 1
	levels[inChannels][outChannels] = 1;
	
	float leftRMS  = inputTap.getLeftChannelRMS();
	float rightRMS = inputTap.getRightChannelRMS();
	
	// setting input of every channel to 1
	for(size_t i = 0; i < inChannels + 1; i++) {
		levels[i][outChannels] = 1;
	}
	
	// setting output of every channel to 1
	for(size_t i = 0; i < outChannels + 1; i++) {
		levels[inChannels][i] = 1;
	}
	
	const float currentTime = ofGetElapsedTimef();
	
	if(leftRMS > threshold || rightRMS > threshold)
	{
		lastActivation = currentTime;
	}
	
	bool shouldRing = (currentTime - lastActivation) > interactionTimeout;
	
	// set levels of mic / ring tone
	levels[kMicBus * 2][0]     = shouldRing ? 0 : 1;
	levels[kMicBus * 2 + 1][1] = shouldRing ? 0 : 1;
	
	levels[kRingBus * 2][0] = levels[kRingBus * 2][1] = shouldRing ? 2 : 0;
	
	OFXAU_PRINT(AudioUnitSetProperty(mixer,
									 kAudioUnitProperty_MatrixLevels,
									 kAudioUnitScope_Global,
									 0,
									 levels,
									 sizeof(levels)),
				"setting matrix mixer levels");
}

void testApp::draw() {
	ofSetLineWidth(3);
	
	// draw input waveforms
	ofSetColor(100, 100, 255);
	ofPushMatrix();
	{
		leftInWaveform.draw();
		ofTranslate(0, ofGetHeight() / 2.);
		rightInWaveform.draw();
	}
	ofPopMatrix();
	
	// draw output waveforms
	ofSetColor(255, 100, 100);
	ofPushMatrix();
	{
		ofTranslate(ofGetWidth() / 2., 0);
		leftOutWaveform.draw();
		ofTranslate(0, ofGetHeight() / 2.);
		rightOutWaveform.draw();
	}
	ofPopMatrix();
	
	float curTime = ofGetElapsedTimef();
	string last = "Last trigger : ";
	last.append(ofToString(lastActivation));
	
	string current = "Current time : ";
	current.append(ofToString(curTime));
	
	string timeLeftString = "Time til shit's ringin : "; // it's a pun
	float timeLeft = interactionTimeout - (curTime - lastActivation);
	timeLeftString.append( timeLeft > 0 ? ofToString(timeLeft) : "NOW");
	
	ofDrawBitmapStringHighlight(last, 10, 30);
	ofDrawBitmapStringHighlight(current, 10, 50);
	ofDrawBitmapStringHighlight(timeLeftString, 10, 70);
}

void testApp::exit() {
	output.stop();
	input.stop();
}

void testApp::keyPressed(int key) {
	if(key == 's') {
//		eq.saveCustomPreset(eqPreset);
	}
}
