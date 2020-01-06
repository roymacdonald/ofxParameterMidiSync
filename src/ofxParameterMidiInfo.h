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
	ofParameterMidiInfo(ofAbstractParameter* p, ofxMidiMessage& msg, int indexOffset = 0);
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
	
	void sendFeedback(std::shared_ptr<ofxMidiOut> midiOut);
	
	
protected:
	template<typename T>
	void valueToMultiDimParam(float value, int mx = 127){
		T v = param->cast<T>();
		float mp = ofMap(value, 0, mx, param->cast<T>().getMin()[multiDimIndex], param->cast<T>().getMax()[multiDimIndex]);
		if (v[multiDimIndex] != mp) {
			v[multiDimIndex] = mp;
			param->cast<T>() = v;
		}
	}
	
	template<typename T>
	void valueToParam(float value, int mx = 127){
		int mp = ofMap(value, 0, mx, param->cast<T>().getMin(), param->cast<T>().getMax());
		if (param->cast<T>() != mp) {
			param->cast<T>() = mp;
		}
	}
	
	template<typename T>
	int multiDimParamToMidiValue(int mx = 127){
		auto p = param->cast<T>();
		return ofMap(p.get()[multiDimIndex], p.getMin()[multiDimIndex], p.getMax()[multiDimIndex], 0, mx);
	}
	template<typename T>
	int paramToMidiValue(int mx = 127){
		auto p = param->cast<T>();
		return ofMap(p.get(), p.getMin(), p.getMax(), 0, mx);
	}	

private:
	bool bSendFeedback = false;
	bool bIsButton = false;
};
