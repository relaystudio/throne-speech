#include "testApp.h"

void testApp::setup(){

	ringTone.setFile(ofFilePath::getAbsolutePath("sound/nokia.wav"));
	ringTone.loop();
	
	mixer.setInputBusCount(2);
	input.connectTo(mixer, 0);
	ringTone.connectTo(mixer, 1);
	
	mixer.connectTo(debugTap).connectTo(output);
	
	input.start();
	output.start();
	
	ofSetVerticalSync(true);
	ofBackground(50);
}

void testApp::update(){
	debugTap.getLeftWaveform(leftWaveform, ofGetWidth(), ofGetHeight() / 2.);
	debugTap.getRightWaveform(rightWaveform, ofGetWidth(), ofGetHeight() / 2.);
}

void testApp::draw(){
	
	// draw left / right waveforms
	ofSetColor(255, 100, 100);
	ofPushMatrix();
	{
		leftWaveform.draw();
		ofTranslate(0, ofGetHeight() / 2.);
		rightWaveform.draw();
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
