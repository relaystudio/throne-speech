#include "testApp.h"
#include "ofxAudioUnitUtils.h"

enum {
	kMicBus  = 0,
	kRingBus = 1
};

const float interactionTimeout = 15;
const float threshold = 0.25;

void testApp::setup() {
	setupAudioGraph("sound/nokia.wav");
	ofSetVerticalSync(true);
	ofBackground(50);
}

void testApp::setupAudioGraph(string ringToneFile) {
	reverb = ofxAudioUnit('aufx', 'mrev', 'appl');
	
	ringTone.setFile(ofFilePath::getAbsolutePath(ringToneFile));
	ringTone.loop();
	
	mixer.setChannelLayout(2, 1);
	input.connectTo(inputTap).connectTo(mixer, kMicBus);
	ringTone.connectTo(mixer, kRingBus);
	
	mixer.connectTo(outputTap).connectTo(reverb).connectTo(output);
	
	inputTap.setBufferLength(512);
	outputTap.setBufferLength(512);
	
	input.start();
	output.start();
}

void testApp::update(){
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
	
	// setting input of every channel to 1
	for(size_t i = 0; i < inChannels + 1; i++) {
		levels[i][outChannels] = 1;
	}
	
	// setting output of every channel to 1
	for(size_t i = 0; i < outChannels + 1; i++) {
		levels[inChannels][i] = 1;
	}
	
	const float currentTime = ofGetElapsedTimef();
	
	if(inputTap.getLeftChannelRMS()  > threshold ||
	   inputTap.getRightChannelRMS() > threshold)
	{
		lastActivation = currentTime;
	}
	
	bool shouldRing = (currentTime - lastActivation) > interactionTimeout;
	
	// set levels of mic / ring tone per channel
	levels[kMicBus  * 2][0] = shouldRing ? 0 : 1;
	levels[kRingBus * 2][0] = shouldRing ? 1 : 0;
	
	levels[kMicBus  * 2 + 1][1] = shouldRing ? 0 : 1;
	levels[kRingBus * 2][1] = shouldRing ? 1 : 0;
	
	OFXAU_PRINT(AudioUnitSetProperty(mixer,
									 kAudioUnitProperty_MatrixLevels,
									 kAudioUnitScope_Global,
									 0,
									 levels,
									 sizeof(levels)),
				"setting matrix mixer levels");
}

void testApp::draw(){
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
	
	ofDrawBitmapStringHighlight(ofToString(lastActivation), 10, 30);
	ofDrawBitmapStringHighlight(ofToString(ofGetElapsedTimef()), 10, 50);
}

void testApp::exit(){
	output.stop();
	input.stop();
}

void testApp::keyPressed(int key){

}
