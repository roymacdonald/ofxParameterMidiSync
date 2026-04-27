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
    void setup(int portNum, bool bUseRecorder = true , bool bUsePlayer = true);
	void setup(int portNum, ofAbstractParameter & parameters, bool bUseRecorder = true , bool bUsePlayer = true);
    void setup(int portNum, ofParameterGroup & parameters, bool bUseRecorder = true , bool bUsePlayer = true);
    void setup(int inPortNum, int outPortNum, bool bUseRecorder = true , bool bUsePlayer = true);
	void setup(int inPortNum, int outPortNum, ofAbstractParameter & parameters, bool bUseRecorder = true , bool bUsePlayer = true);
    void setup(int inPortNum, int outPortNum, ofParameterGroup & parameters, bool bUseRecorder = true , bool bUsePlayer = true);




    void setSyncGroup( ofAbstractParameter & parameters);
    void setSyncGroup( ofParameterGroup & parameters);
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
	void setGuiPosition(float x, float y );
	void drawGui();
	
	
    bool isLearning(){return bLearning;}
    bool isUnlearning(){return bUnlearning;}
    
    std::shared_ptr<ofxMidiRecorder> recorder = nullptr;
    std::shared_ptr<ofxMidiPlayer> player = nullptr;
    ofParameter<float> smoothing = {"Smoothing",0.5,0,1};

	void enableSmoothing();
	void disableSmoothing();
	bool isSmoothingEnabled();
		
	std::shared_ptr<ofxMidiIn> getMidiIn();
	std::shared_ptr<ofxMidiOut> getMidiOut();
	
	void setFilePath(std::string path);
	std::string getFilePath();
	ofParameter<int> inPortNum =  {"MIDI out port", -1,-1,0};
	ofParameter<int> outPortNum = {"MIDI in port", -1,-1,0};
	
protected:
	std::shared_ptr<ofxPanel> syncSettingsGui;
	
	void openMidi();
	void closeMidi();
	
	ofParameter<void> bLoad = {"Load"};
	ofParameter<void> bSave = {"Save"};
	ofParameter<void> bReset = {"Reset"};
	ofParameter<bool> bLearning = {"Learn", false};
	ofParameter<bool> bUnlearning = {"Unlearn", false};
	ofParameter<bool> bMidiEnabled = {"MidiEnabled", false};
	ofParameter<bool> bSmoothingEnabled = {"Smoothing Enabled", false};


	std::string filePath;
	
	void update(ofEventArgs& e);
	
    void newMidiMessage(ofxMidiMessage& eventArgs);

    bool linkMidiToOfParameter(ofxMidiMessage& msg, ofAbstractParameter* param);

    std::shared_ptr<ofxMidiIn> midiIn;
	std::shared_ptr<ofxMidiOut> midiOut;
    ofxMidiMessage midiMessage;

    void parameterChanged( ofAbstractParameter & parameter );
    ofParameterGroup syncGroup;
    map<int, shared_ptr<ofParameterMidiInfo> > synced;

	bool bIsSetup = false;
	bool bParameterGroupSetup = false;
	ofAbstractParameter* learningParameter = nullptr;
   
    shared_ptr<ofxMidiNanoKontrolButtons> kontrolButtons;

public:
	ofParameterGroup parameters = {"ofParameterMidiSync", bLoad, bSave, bReset, bLearning, bUnlearning, bMidiEnabled, bSmoothingEnabled, smoothing, inPortNum, outPortNum};

private:
	bool bMidiInOpened = false;
	bool bMidiOutOpened = false;

	ofEventListener updateListener;
	ofEventListeners paramsListeners;
	
};
