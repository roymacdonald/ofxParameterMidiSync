//
//  ofxParameterMidiSync.h
//  emptyExample
//
//  Created by Roy Macdonald on 18-08-15.
//
//


#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxMidiRecorder.h"
#include "ofxParameterMidiInfo.h"
#include "nanoKontrolConstants.h"
#include "ofxGui.h"


class ofxParameterMidiSync:  public ofxMidiListener {
public:
    ofxParameterMidiSync();
    ~ofxParameterMidiSync();
    void setup(int portNum);
	void setup(int portNum, ofAbstractParameter & parameters);//, bool bAutoLink);
    void setup(int portNum, ofParameterGroup & parameters);//, bool bAutoLink);
    void setSyncGroup( ofAbstractParameter & parameters);//, bool bAutoLink);
    void setSyncGroup( ofParameterGroup & parameters);//, bool bAutoLink);
    void enableMidi(bool b = true);

    bool linkMidiToOfParameter(ofxMidiMessage& msg, ofAbstractParameter& param);
    
    void learn(bool bLearn = true);
    void unlearn(bool bUnlearn = true);
    
	bool load();
	void save();
	bool load(string path);
    void save(string path);
    
    void reset();
    
    void drawDebug();
	
	void setupGui(float x = 10, float y = 10);
	void drawGui();
	
	
    bool isLearning(){return bLearning;}
    bool isUnlearning(){return bUnlearning;}
    
    ofxMidiRecorder recorder;
    ofxMidiPlayer player;
    ofParameter<float> smoothing;

//    ofEvent<void>ffwKeyPressed;
	
	void enableSmoothing();
	void disableSmoothing();
	bool isSmoothingEnabled();
	
	ofParameterGroup parameters;
	
	
	std::shared_ptr<ofxMidiIn> getMidiIn();
	std::shared_ptr<ofxMidiOut> getMidiOut();
	
protected:
	std::shared_ptr<ofxPanel> syncSettingsGui;
	
	void openMidi();
	void closeMidi();
	
	ofParameter<void> bLoad, bSave, bReset;
	ofParameter<bool> bLearning, bUnlearning, bMidiEnabled, bSmoothingEnabled;
	ofParameter<int> portNum;
	ofParameter<std::string>filePath;
	
	void update(ofEventArgs& e);
	
    void newMidiMessage(ofxMidiMessage& eventArgs);

    bool linkMidiToOfParameter(ofxMidiMessage& msg, ofAbstractParameter* param);

    std::shared_ptr<ofxMidiIn> midiIn;
	std::shared_ptr<ofxMidiOut> midiOut;
    ofxMidiMessage midiMessage;

    void parameterChanged( ofAbstractParameter & parameter );
    ofParameterGroup syncGroup;
    map<int, shared_ptr<ofParameterMidiInfo> > synced;
//    bool bLearning, bUnlearning;
//    int portNum;
//    bool bMidiEnabled;
    bool bIsSetup;
    bool bParameterGroupSetup;
    ofAbstractParameter * learningParameter;
   
    shared_ptr<ofxMidiNanoKontrolButtons> kontrolButtons;

//	bool bSmoothingEnabled = false;

private:
	bool bMidiOpened = false;
	ofEventListener updateListener;
	ofEventListeners paramsListeners;
	
};
