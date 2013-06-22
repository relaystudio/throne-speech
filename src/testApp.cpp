#include "testApp.h"

void testApp::setup(){
	ringTone.setFile(ofFilePath::getAbsolutePath("sound/nokia.wav"));
	ringTone.loop();
	
	mixer.setInputBusCount(2);
	input.connectTo(inputTap).connectTo(mixer, 0);
	ringTone.connectTo(mixer, 1);
	
	mixer.connectTo(outputTap).connectTo(output);
	
	inputTap.setBufferLength(512);
	outputTap.setBufferLength(512);
	
	input.start();
	output.start();
	
	mixer.setInputVolume(0, 0); // mute input for now
	
	ofSetVerticalSync(true);
	ofBackground(50);
}

void testApp::update(){
	ofVec2f waveSize(ofGetWidth() / 2., ofGetHeight() / 2.);
	inputTap.getStereoWaveform(leftInWaveform, rightInWaveform, waveSize.x, waveSize.y);
	outputTap.getStereoWaveform(leftOutWaveform, rightOutWaveform, waveSize.x, waveSize.y);
}

void testApp::draw(){

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
