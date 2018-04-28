//
//  ofxParameterMidiInfo.cpp
//  d4nSFPCRunner
//
//  Created by Roy Macdonald on 18-12-15.
//
//

#include "ofxParameterMidiInfo.h"
#include "nanoKontrolConstants.h"
#include "ofxParamMidiSyncUtils.h"

int ofParameterMidiInfo::getParameterAsMidiValue(){
//Use templated function
	int mx = 127;
	if (param) {
		if (dims > 1 ) {
			int & ind = multiDimIndex;
			if (dims > multiDimIndex) {
				if (multiDimType == 2) {
					auto p = param->cast<glm::vec2>();
					return ofMap(p.get()[ind], p.getMin()[ind], p.getMax()[ind], 0, mx);
				}else if (multiDimType == 3) {
					auto p = param->cast<glm::vec3>();
					return ofMap(p.get()[ind], p.getMin()[ind], p.getMax()[ind], 0, mx);
				}else if (multiDimType == 4) {
					auto p = param->cast<glm::vec4>();
					return ofMap(p.get()[ind], p.getMin()[ind], p.getMax()[ind], 0, mx);
				}else if (multiDimType == 8) {
					auto p = param->cast<ofColor>();
					return ofMap(p.get()[ind], p.getMin()[ind], p.getMax()[ind], 0, mx);
				}else if (multiDimType == 16) {
					auto p = param->cast<ofShortColor>();
					return ofMap(p.get()[ind], p.getMin()[ind], p.getMax()[ind], 0, mx);
				}else if (multiDimType == 32 ) {
					auto p = param->cast<ofFloatColor>();
					return ofMap(p.get()[ind], p.getMin()[ind], p.getMax()[ind], 0, mx);
				}
			}
		}else{
			if(param->type()==typeid(ofParameter<int>).name()){
				auto p = param->cast<int>();
				return ofMap(p.get(), p.getMin(), p.getMax(), 0, mx);
			}else if(param->type()==typeid(ofParameter<float>).name()){
				auto p = param->cast<float>();
				return ofMap(p.get(), p.getMin(), p.getMax(), 0, mx);
			}else if(param->type()==typeid(ofParameter<double>).name()){
				auto p = param->cast<double>();
				return ofMap(p.get(), p.getMin(), p.getMax(), 0, mx);
			}else if(param->type()==typeid(ofParameter<bool>).name()){				
				return (param->cast<bool>().get()?mx:0);
			}
		}
	}
	return 0;
}
bool ofParameterMidiInfo::isSendFeedback(){
	return param || bSendFeedback;
}
void ofParameterMidiInfo::mapValueToParameter(float value){
	int mx = 127;
	if (param) {
		if (dims > 1 ) {
			int & ind = multiDimIndex;
			if (dims > multiDimIndex) {
				if (multiDimType == 2) {
					glm::vec2 v = param->cast<glm::vec2>();
					float mp = ofMap(value, 0, mx, param->cast<glm::vec2>().getMin()[ind], param->cast<glm::vec2>().getMax()[ind]);
					if (v[ind] != mp) {
						v[ind] = mp;
						param->cast<glm::vec2>() = v;
					}
				}else if (multiDimType == 3) {
					glm::vec3 v = param->cast<glm::vec3>();
					float mp = ofMap(value, 0, mx, param->cast<glm::vec3>().getMin()[ind], param->cast<glm::vec3>().getMax()[ind]);
					if (v[ind] != mp) {
						v[ind] = mp;
						param->cast<glm::vec3>() = v;
					}
				}else if (multiDimType == 4) {
					glm::vec4 v = param->cast<glm::vec4>();
					float mp = ofMap(value, 0, mx, param->cast<glm::vec4>().getMin()[ind], param->cast<glm::vec4>().getMax()[ind]);
					if (v[ind] != mp) {
						v[ind] = mp;
						param->cast<glm::vec4>() = v;
					}
				}else if (multiDimType == 8) {
					ofColor v = param->cast<ofColor>();
					unsigned char mp = ofMap(value, 0, mx, param->cast<ofColor>().getMin()[ind], param->cast<ofColor>().getMax()[ind]);
					if (v[ind] != mp) {
						v[ind] = mp;
						param->cast<ofColor>() = v;
					}
				}else if (multiDimType == 16) {
					ofShortColor v = param->cast<ofShortColor>();
					unsigned short mp = ofMap(value, 0, mx, param->cast<ofShortColor>().getMin()[ind], param->cast<ofShortColor>().getMax()[ind]);
					if (v[ind] != mp) {
						v[ind] = mp;
						param->cast<ofShortColor>() = v;
					}
				}else if (multiDimType == 32 ) {
					ofFloatColor v = param->cast<ofFloatColor>();
					float mp = ofMap(value, 0, mx, param->cast<ofFloatColor>().getMin()[ind], param->cast<ofFloatColor>().getMax()[ind]);
					if (v[ind] != mp) {
						v[ind] = mp;
						param->cast<ofFloatColor>() = v;
					}
				}
			}
		}else{
			if(param->type()==typeid(ofParameter<int>).name()){
				int mp = ofMap(value, 0, mx, param->cast<int>().getMin(), param->cast<int>().getMax());
				if (param->cast<int>() != mp) {
					param->cast<int>() = mp;
				}
			}else if(param->type()==typeid(ofParameter<float>).name()){
				
				float mp = ofMap(value, 0, mx, param->cast<float>().getMin(), param->cast<float>().getMax());
				if (param->cast<float>() != mp) {
					param->cast<float>() = mp;
				}
			}else if(param->type()==typeid(ofParameter<double>).name()){
				
				double mp = ofMap(value, 0, mx, param->cast<double>().getMin(), param->cast<double>().getMax());
				if (param->cast<double>() != mp) {
					param->cast<double>() = mp;
				}
			}else if(param->type()==typeid(ofParameter<bool>).name()){
				bool bVal = value != 0;
				if (param->cast<bool>() != bVal) {
					param->cast<bool>() = bVal;
				}
			}
		}
	}
}
bool ofParameterMidiInfo::isMultiDim(){
	return bIsColor || bIsVec;
}
ofParameterMidiInfo::ofParameterMidiInfo(ofAbstractParameter* p){

	param = p;
}
ofParameterMidiInfo::ofParameterMidiInfo(ofAbstractParameter* p, ofxMidiMessage& msg, int indexOffset){// int d, bool bCol, bool bVec, int cNum, int mdType, int mdIndex):dims(d), bIsColor(bCol), bIsVec(bVec), controlNum(cNum),multiDimType(mdType), bNeedSmoothing(false), multiDimIndex(mdIndex){
	if(p == nullptr) return;
	controlNum = msg.control + indexOffset;
	channel = msg.channel;
	
	int isVec = ofxParamMidiSync::isVecParam(p);
	int isCol = ofxParamMidiSync::isColorParam(p);
	dims = (isVec + isCol);
	if (dims == 0) dims = 1;
	if (dims > 4) dims = 4;
	bIsColor = isCol;
	bIsVec = isVec;
	multiDimType = isVec + isCol;
	multiDimIndex = indexOffset;
	
	bIsButton = ofxNanoKontrol::isButton(controlNum);
	bSendFeedback = bIsButton;
	param = p;
}
void ofParameterMidiInfo::updateSmoothing(float smoothFactor){
	if (bNeedSmoothing) {
		smoothValue = smoothValue* smoothFactor + lastValue*(1 - smoothFactor);
		if (fabs(lastValue - smoothValue) < 0.01 ) {
			smoothValue = lastValue;
			bNeedSmoothing = false;
		}
		mapValueToParameter(smoothValue);
	}
}
void ofParameterMidiInfo::setNewValue(int value, bool bUseSmoothing){
	if(lastValue != value){
		if(bUseSmoothing && !bIsButton){
			bNeedSmoothing = true;
			lastValue = value;
			
		}else if(param->type()==typeid(ofParameter<bool>).name()){
			if(value == 127){
				param->cast<bool>() ^=true;
			}
		}else{
			mapValueToParameter(value);
		}
	}
}
void ofParameterMidiInfo::saveToXml(ofXml& xml){
	if(param){
		auto x = xml.appendChild("ofParameterMidiInfo");
		x.appendChild("groupHierarchyNames").set(ofxParamMidiSync::joinStrings(param->getGroupHierarchyNames(), "/"));
		x.appendChild("dims").set(dims);
		x.appendChild("controlNum").set(controlNum);
		x.appendChild("channel").set(channel);
		x.appendChild("bIsColor").set(bIsColor);
		x.appendChild("bIsVec").set(bIsVec);
		x.appendChild("multiDimType").set(multiDimType);
		x.appendChild("multiDimIndex").set(multiDimIndex);
		x.appendChild("bSendFeedback").set(bSendFeedback);
		x.appendChild("bIsButton").set(bIsButton);
	}
}
void ofParameterMidiInfo::sendFeedback(ofxMidiOut & midiOut){
	if(isSendFeedback()){
		midiOut.sendControlChange(channel, controlNum, getParameterAsMidiValue());
	}
}
bool ofParameterMidiInfo::loadFromXml(ofXml& xml){
	bool ret = false;
	if(param){
		if(xml){
			if(ofxParamMidiSync::joinStrings(param->getGroupHierarchyNames(), "/") == xml.getChild("groupHierarchyNames").getValue()){
				dims = xml.getChild("dims").getIntValue();
				controlNum = xml.getChild("controlNum").getIntValue();
				channel = xml.getChild("channel").getBoolValue();
				bIsColor = xml.getChild("bIsColor").getBoolValue();
				bIsVec = xml.getChild("bIsVec").getBoolValue();
				multiDimType = xml.getChild("multiDimType").getIntValue();
				multiDimIndex = xml.getChild("multiDimIndex").getIntValue();
				bSendFeedback = xml.getChild("bSendFeedback").getBoolValue();
				bIsButton = xml.getChild("bIsButton").getBoolValue();
				ret = true;
			}
		}
	}
	return ret;
}
