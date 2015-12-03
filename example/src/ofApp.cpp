
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	ofBackground(255, 255, 255);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
    gui.setup("gui");
    gui.add(floatColor.set("FloatColor", ofFloatColor::white, ofFloatColor(0,0), ofFloatColor::white));
    gui.add(param2.set("param2", 0, 0, 10));
    groups[0].setName("0");
    groups[0].add(boton.set("boton", true));
    
    groups[1].setName("1");
    groups[1].add(param1.set("param1", 0, 0, 1000));
    groups[0].add(groups[1]);
    
    groups[2].setName("2");
    groups[2].add(color.set("color", ofColor(0), ofColor(0,0), ofColor(255, 255)));
    groups[1].add(groups[2]);
    
    gui.add(groups[0]);
    
    
    sync.setup(0, gui.getParameter());
    
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(0);
    gui.draw();
    
    sync.drawDebug();
}

//--------------------------------------------------------------
void ofApp::exit() {
	
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	switch(key) {
		case ' ':
            sync.learn();
            break;
        case 's':
            sync.save();
            break;
        case 'l':
            sync.load();
            break;
        case OF_KEY_BACKSPACE:
            sync.unlearn();
            break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased() {
}