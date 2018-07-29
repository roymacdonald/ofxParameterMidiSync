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
			if (dims > multiDimIndex) {
					 if (multiDimType == 2) {  return multiDimParamToMidiValue<glm::vec2>();}
				else if (multiDimType == 3) {  return multiDimParamToMidiValue<glm::vec3>();}
				else if (multiDimType == 4) {  return multiDimParamToMidiValue<glm::vec4>();}
				else if (multiDimType == 8) {  return multiDimParamToMidiValue<ofColor>();}
				else if (multiDimType == 16) { return multiDimParamToMidiValue<ofShortColor>();}
				else if (multiDimType == 32 ){ return multiDimParamToMidiValue<ofFloatColor>();}

			}
		}else{

				 if(param->type()==typeid(ofParameter<int8_t>).name()){   return paramToMidiValue<int8_t>  ();}
			else if(param->type()==typeid(ofParameter<uint8_t>).name()){  return paramToMidiValue<uint8_t> ();}
			else if(param->type()==typeid(ofParameter<int16_t>).name()){  return paramToMidiValue<int16_t> ();}
			else if(param->type()==typeid(ofParameter<uint16_t>).name()){ return paramToMidiValue<uint16_t>();}
			else if(param->type()==typeid(ofParameter<int32_t>).name()){  return paramToMidiValue<int32_t> ();}
			else if(param->type()==typeid(ofParameter<uint32_t>).name()){ return paramToMidiValue<uint32_t>();}
			else if(param->type()==typeid(ofParameter<int64_t>).name()){  return paramToMidiValue<int64_t> ();}
			else if(param->type()==typeid(ofParameter<uint64_t>).name()){ return paramToMidiValue<uint64_t>();}
			else if(param->type()==typeid(ofParameter<float>).name()){    return paramToMidiValue<float>   ();}
			else if(param->type()==typeid(ofParameter<double>).name()){   return paramToMidiValue<double>  ();}
			else if(param->type()==typeid(ofParameter<bool>).name()){	  return (param->cast<bool>().get()?mx:0);}

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
			if (dims > multiDimIndex) {
				      if (multiDimType == 2) { valueToMultiDimParam<glm::vec2>(value);}
				else if (multiDimType == 3) { valueToMultiDimParam<glm::vec3>(value);}
				else if (multiDimType == 4) { valueToMultiDimParam<glm::vec4>(value);}
				else if (multiDimType == 8) { valueToMultiDimParam<ofColor>(value);}
				else if (multiDimType == 16) {valueToMultiDimParam<ofShortColor>(value);}
				else if (multiDimType == 32 ){valueToMultiDimParam<ofFloatColor>(value);}
			}
		}else{	
			      if(param->type()==typeid(ofParameter<int8_t>).name()){  valueToParam<int8_t>  (value);}
			else if(param->type()==typeid(ofParameter<uint8_t>).name()){  valueToParam<uint8_t> (value);}
			else if(param->type()==typeid(ofParameter<int16_t>).name()){  valueToParam<int16_t> (value);}
			else if(param->type()==typeid(ofParameter<uint16_t>).name()){ valueToParam<uint16_t>(value);}
			else if(param->type()==typeid(ofParameter<int32_t>).name()){  valueToParam<int32_t> (value);}
			else if(param->type()==typeid(ofParameter<uint32_t>).name()){ valueToParam<uint32_t>(value);}
			else if(param->type()==typeid(ofParameter<int64_t>).name()){  valueToParam<int64_t> (value);}
			else if(param->type()==typeid(ofParameter<uint64_t>).name()){ valueToParam<uint64_t>(value);}
			else if(param->type()==typeid(ofParameter<float>).name()){    valueToParam<float>   (value);}
			else if(param->type()==typeid(ofParameter<double>).name()){   valueToParam<double>  (value);}
			else if(param->type()==typeid(ofParameter<bool>).name()){
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
void ofParameterMidiInfo::sendFeedback(std::shared_ptr<ofxMidiOut> midiOut){
	if(midiOut){
		if(isSendFeedback()){
			midiOut->sendControlChange(channel, controlNum, getParameterAsMidiValue());
		}
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
