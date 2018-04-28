//
//  ofxParameterMidiInfo.h
//  d4nSFPCRunner
//
//  Created by Roy Macdonald on 18-12-15.
//
//

#pragma once
#include "ofMain.h"
#include "ofxMidi.h"

class ofParameterMidiInfo {
public:
    ofParameterMidiInfo(ofAbstractParameter* p = nullptr);
	ofParameterMidiInfo(ofAbstractParameter* p, ofxMidiMessage& msg, int indexOffset = 0);// int d, bool bCol, bool bVec, int cNum, int mdType, int mdIndex = 0);
    int dims = 0;
    int controlNum = 0;
	int channel = 0;
    bool bIsColor = false;
    bool bIsVec = false;
    int multiDimType = 0;
	int multiDimIndex = 0;
	ofAbstractParameter * param = nullptr;
	
	float smoothValue;
	bool bNeedSmoothing = false;
    int lastValue;

    void setNewValue(int value, bool bUseSmoothing);
    void updateSmoothing(float smoothFactor);
    void saveToXml(ofXml& xml);
    bool loadFromXml(ofXml& xml);
	void mapValueToParameter(float value);
	int  getParameterAsMidiValue();
	bool isSendFeedback();
	bool isMultiDim();
	
	void sendFeedback(ofxMidiOut & midiOut);
	
	

private:
	bool bSendFeedback = false;
	bool bIsButton = false;
};
