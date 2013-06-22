#include "testApp.h"

typedef enum {
	kMicBus      = 0,
	kRingToneBus = 1
}
BusName;

void testApp::setup(){
	setupAudioGraph("sound/nokia.wav", true);
	setupArduino(9600);
	ofSetVerticalSync(true);
	ofBackground(50);
}

void testApp::setupAudioGraph(string ringToneFile, bool muteInput) {
	ringTone.setFile(ofFilePath::getAbsolutePath(ringToneFile));
	ringTone.loop();
	
	mixer.setInputBusCount(2);
	input.connectTo(inputTap).connectTo(mixer, kMicBus);
	ringTone.connectTo(mixer, kRingToneBus);
	
	mixer.connectTo(outputTap).connectTo(output);
	
	inputTap.setBufferLength(512);
	outputTap.setBufferLength(512);
	
	input.start();
	output.start();
	
	if(muteInput) {
		mixer.setInputVolume(0, kMicBus);
	}
}

void testApp::setupArduino(int baud) {
	vector<ofSerialDeviceInfo> devices = arduino.getDeviceList();
	bool found = false;
	bool success = false;
	for(size_t i = 0; i < devices.size(); ++i) {
		if(devices[i].getDeviceName().find("tty.usbmodem") != string::npos) {
			success = arduino.setup(devices[i].getDevicePath(), baud);
			found = true;
			break;
		}
	}
	
	if(!found) {
		ofLog(OF_LOG_WARNING, "couldn't find arduino");
	}
}

void testApp::update(){
	ofVec2f waveSize(ofGetWidth() / 2., ofGetHeight() / 2.);
	inputTap.getStereoWaveform(leftInWaveform, rightInWaveform, waveSize.x, waveSize.y);
	outputTap.getStereoWaveform(leftOutWaveform, rightOutWaveform, waveSize.x, waveSize.y);
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
}

void testApp::exit(){
	output.stop();
	input.stop();
}

void testApp::keyPressed(int key){

}

void testApp::keyReleased(int key){ }
void testApp::mouseMoved(int x, int y ){ }
void testApp::mouseDragged(int x, int y, int button){ }
void testApp::mousePressed(int x, int y, int button){ }
void testApp::mouseReleased(int x, int y, int button){ }
void testApp::windowResized(int w, int h){ }
