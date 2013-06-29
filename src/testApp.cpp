#include "testApp.h"
#include "ofxAudioUnitUtils.h"

enum {
	kMicBus  = 0,
	kRingBus = 1
};

void testApp::setup() {
	setupAudioGraph("sound/nokia.wav");
	setupArduino(9600);
	ofSetVerticalSync(true);
	ofBackground(50);
	
	arduino.flush();
}

void testApp::setupAudioGraph(string ringToneFile) {
	ringTone.setFile(ofFilePath::getAbsolutePath(ringToneFile));
	ringTone.loop();
	
	mixer.setChannelLayout(2, 1);
	input.connectTo(inputTap).connectTo(mixer, kMicBus);
	ringTone.connectTo(mixer, kRingBus);
	
	mixer.connectTo(outputTap).connectTo(output);
	
	inputTap.setBufferLength(512);
	outputTap.setBufferLength(512);
	
	input.start();
	output.start();
}

// sets up serial stuff on its own little private queue
void testApp::setupArduino(int baud) {
	serialQueue = dispatch_queue_create("com.relaystudio.arduino-queue", DISPATCH_QUEUE_SERIAL);
	
	// attempt to connect to arduino
	dispatch_sync(serialQueue, ^{
		vector<ofSerialDeviceInfo> devices = arduino.getDeviceList();
		bool found = false;
		for(size_t i = 0; i < devices.size(); ++i) {
			if(devices[i].getDeviceName().find("tty.usbmodem") != string::npos) {
				arduino.setup(devices[i].getDevicePath(), baud);
				found = true;
				break;
			}
		}
		
		if(!found) {
			ofLog(OF_LOG_WARNING, "couldn't find arduino");
		}
	});
	
	// setup hardware reading on the serial queue
	serialTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, serialQueue);
	if(serialTimer) {
		const dispatch_time_t interval = 20 * NSEC_PER_MSEC;
		const dispatch_time_t leeway = 5 * NSEC_PER_MSEC;
		
		dispatch_source_set_timer(serialTimer, dispatch_walltime(NULL, 0), interval, leeway);
		dispatch_source_set_event_handler(serialTimer, ^{
			if(!arduino.isInitialized()) return;
			
			const int availableBytes = arduino.available();
			if(availableBytes > 0) {
				uint8_t buffer[availableBytes];
				arduino.readBytes(buffer, availableBytes);
				arduinoReading = buffer[availableBytes - 1];
			}
		});
	}
	
	dispatch_resume(serialTimer);
}

void testApp::update(){
	// get debug waveforms
	ofVec2f waveSize(ofGetWidth() / 2., ofGetHeight() / 2.);
	inputTap.getStereoWaveform(leftInWaveform, rightInWaveform, waveSize.x, waveSize.y);
	outputTap.getStereoWaveform(leftOutWaveform, rightOutWaveform, waveSize.x, waveSize.y);
	
	float readings[2] = {
		ofMap(getReading(0), 0, 40, 0, 1, true),
		ofMap(getReading(1), 0, 40, 0, 1, true)
	};
	
	// to make sense of this, see the docs on kAudioUnitProperty_MatrixLevels
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
	
	levels[kMicBus * 2][0]  = 1 - readings[0];
	levels[kRingBus * 2][0] = readings[0];
	
	levels[kMicBus * 2][1]  = 1 - readings[1];
	levels[kRingBus * 2][1] = readings[1];
	
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
	
	ofSetColor(255);
	string displayString = "Sensors [";
	displayString.append(ofToString(getReading(0)));
	displayString.append(",");
	displayString.append(ofToString(getReading(1)));
	displayString.append("]");
	ofDrawBitmapString(displayString, 10, 20);
}

void testApp::exit(){
	output.stop();
	input.stop();
	
	dispatch_suspend(serialTimer);
	dispatch_sync(serialQueue, ^{arduino.close();});
}

int testApp::getReading(int sensor) {
	__block int reading = 0;
	dispatch_sync(serialQueue, ^{reading = arduinoReading;});
	return reading;
}

void testApp::keyPressed(int key){

}

void testApp::keyReleased(int key){ }
void testApp::mouseMoved(int x, int y ){ }
void testApp::mouseDragged(int x, int y, int button){ }
void testApp::mousePressed(int x, int y, int button){ }
void testApp::mouseReleased(int x, int y, int button){ }
void testApp::windowResized(int w, int h){ }
