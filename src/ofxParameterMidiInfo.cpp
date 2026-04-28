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
#include <sstream>
#include <iomanip>

namespace {
	std::string midiStatusToString(MidiStatus s){
		switch(s){
			case MIDI_NOTE_OFF:         return "NOTE_OFF";
			case MIDI_NOTE_ON:          return "NOTE_ON";
			case MIDI_CONTROL_CHANGE:   return "CONTROL_CHANGE";
			case MIDI_PROGRAM_CHANGE:   return "PROGRAM_CHANGE";
			case MIDI_PITCH_BEND:       return "PITCH_BEND";
			case MIDI_AFTERTOUCH:       return "AFTERTOUCH";
			case MIDI_POLY_AFTERTOUCH:  return "POLY_AFTERTOUCH";
			default:                    return "UNKNOWN(" + ofToString((int)s) + ")";
		}
	}
	std::string multiDimTypeToString(int t){
		switch(t){
			case 0:  return "scalar";
			case 2:  return "glm::vec2";
			case 3:  return "glm::vec3";
			case 4:  return "glm::vec4";
			case 8:  return "ofColor";
			case 16: return "ofShortColor";
			case 32: return "ofFloatColor";
			default: return "unknown(" + ofToString(t) + ")";
		}
	}
}

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
	// Feedback is opt-in per binding so devices that don't accept echoed
	// values (or that would create feedback loops with motorised faders)
	// can disable it. The legacy XML loader sets bSendFeedback=true by
	// default so old files continue to behave as before.
	return param != nullptr && bSendFeedback;
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
bool ofParameterMidiInfo::isIntegerParam() const {
	if(!param) return false;
	const auto t = param->type();
	return t == typeid(ofParameter<int8_t>).name()
		|| t == typeid(ofParameter<uint8_t>).name()
		|| t == typeid(ofParameter<int16_t>).name()
		|| t == typeid(ofParameter<uint16_t>).name()
		|| t == typeid(ofParameter<int32_t>).name()
		|| t == typeid(ofParameter<uint32_t>).name()
		|| t == typeid(ofParameter<int64_t>).name()
		|| t == typeid(ofParameter<uint64_t>).name();
}
void ofParameterMidiInfo::captureIntValueFromParam(){
	if(!param) return;
	const auto t = param->type();
	if      (t == typeid(ofParameter<int8_t>).name())   { storedIntValue = (int)param->cast<int8_t>().get();   bHasStoredValue = true; }
	else if (t == typeid(ofParameter<uint8_t>).name())  { storedIntValue = (int)param->cast<uint8_t>().get();  bHasStoredValue = true; }
	else if (t == typeid(ofParameter<int16_t>).name())  { storedIntValue = (int)param->cast<int16_t>().get();  bHasStoredValue = true; }
	else if (t == typeid(ofParameter<uint16_t>).name()) { storedIntValue = (int)param->cast<uint16_t>().get(); bHasStoredValue = true; }
	else if (t == typeid(ofParameter<int32_t>).name())  { storedIntValue = (int)param->cast<int32_t>().get();  bHasStoredValue = true; }
	else if (t == typeid(ofParameter<uint32_t>).name()) { storedIntValue = (int)param->cast<uint32_t>().get(); bHasStoredValue = true; }
	else if (t == typeid(ofParameter<int64_t>).name())  { storedIntValue = (int)param->cast<int64_t>().get();  bHasStoredValue = true; }
	else if (t == typeid(ofParameter<uint64_t>).name()) { storedIntValue = (int)param->cast<uint64_t>().get(); bHasStoredValue = true; }
}
void ofParameterMidiInfo::applyStoredIntValue(){
	if(!param || !bHasStoredValue) return;
	const auto t = param->type();
	if      (t == typeid(ofParameter<int8_t>).name())   { param->cast<int8_t>()   = (int8_t)storedIntValue;   }
	else if (t == typeid(ofParameter<uint8_t>).name())  { param->cast<uint8_t>()  = (uint8_t)storedIntValue;  }
	else if (t == typeid(ofParameter<int16_t>).name())  { param->cast<int16_t>()  = (int16_t)storedIntValue;  }
	else if (t == typeid(ofParameter<uint16_t>).name()) { param->cast<uint16_t>() = (uint16_t)storedIntValue; }
	else if (t == typeid(ofParameter<int32_t>).name())  { param->cast<int32_t>()  = (int32_t)storedIntValue;  }
	else if (t == typeid(ofParameter<uint32_t>).name()) { param->cast<uint32_t>() = (uint32_t)storedIntValue; }
	else if (t == typeid(ofParameter<int64_t>).name())  { param->cast<int64_t>()  = (int64_t)storedIntValue;  }
	else if (t == typeid(ofParameter<uint64_t>).name()) { param->cast<uint64_t>() = (uint64_t)storedIntValue; }
}
ofParameterMidiInfo::ofParameterMidiInfo(ofAbstractParameter* p){
	
	param = p;
}
ofParameterMidiInfo::ofParameterMidiInfo(ofAbstractParameter* p, ofxMidiMessage& msg, int indexOffset){
	if(p == nullptr) return;

	// Use msg.pitch for note bindings, msg.control for CC.
	int baseNum = ofxParamMidiSync::numberFromMessage(msg);
	controlNum = baseNum + indexOffset;
	channel = msg.channel;

	// Normalise NOTE_OFF -> NOTE_ON so a single binding represents both
	// the press and release of one note.
	inputStatus = (msg.status == MIDI_NOTE_OFF) ? MIDI_NOTE_ON : msg.status;

	int isVec = ofxParamMidiSync::isVecParam(p);
	int isCol = ofxParamMidiSync::isColorParam(p);
	dims = (isVec + isCol);
	if (dims == 0) dims = 1;
	if (dims > 4) dims = 4;
	bIsColor = isCol;
	bIsVec = isVec;
	multiDimType = isVec + isCol;
	multiDimIndex = indexOffset;

	// Treat note bindings and bool parameters as buttons by default so
	// they get LED-style feedback.
	bool isBoolParam   = (p->type() == typeid(ofParameter<bool>).name());
	bool isNoteBinding = (inputStatus == MIDI_NOTE_ON);
	bIsButton = ofxNanoKontrol::isButton(controlNum) || isBoolParam || isNoteBinding;
	bSendFeedback = bIsButton;

	// Feedback defaults: mirror the input message exactly, with full-range
	// on/off values. Override these in the saved XML to drive coloured
	// LEDs (e.g. Launch Control XL: onValue=0x3C green, offValue=0x0C off)
	// without touching code.
	feedbackStatus  = inputStatus;
	feedbackChannel = channel;
	feedbackNum     = controlNum;
	feedbackOnValue  = 127;
	feedbackOffValue = 0;

	param = p;

	// Snapshot the parameter's current value at learn time when binding a
	// MIDI note to a scalar integer parameter. Replayed by setNewValue on
	// each press so each note recalls a specific int value.
	if(inputStatus == MIDI_NOTE_ON && dims == 1 && isIntegerParam()){
		captureIntValueFromParam();
	}
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
		// For note bindings any non-zero "value" (= velocity) is a press;
		// for CC bindings keep the historical "127 = pressed" rule so that
		// existing nanoKontrol-style mappings still behave the same.
		const bool isNote = (inputStatus == MIDI_NOTE_ON);
		const bool isPress = isNote ? (value > 0) : (value == 127);

		if(bUseSmoothing && !bIsButton){
			bNeedSmoothing = true;
			lastValue = value;

		}else if(param->type()==typeid(ofParameter<bool>).name()){
			if(isPress){
				param->cast<bool>() ^=true;
			}
		}else if(param->type()==typeid(ofParameter<void>).name()){
			if(isPress){
				param->cast<void>().trigger();
			}
		}else if(bHasStoredValue){
			// Note-to-int snapshot binding: recall the value captured at
			// learn time instead of mapping velocity to the parameter range.
			if(isPress){
				applyStoredIntValue();
			}
		}
		else{
			mapValueToParameter(value);
		}
		lastValue = value;
	}
}
void ofParameterMidiInfo::saveToXml(ofXml& xml){
	if(param){
		auto x = xml.appendChild("ofParameterMidiInfo");
		x.appendChild("groupHierarchyNames").set(ofxParamMidiSync::joinStrings(param->getGroupHierarchyNames(), "/"));
		x.appendChild("dims").set(dims);
		x.appendChild("inputStatus").set((int)inputStatus);
		x.appendChild("controlNum").set(controlNum);
		x.appendChild("channel").set(channel);
		x.appendChild("bIsColor").set(bIsColor);
		x.appendChild("bIsVec").set(bIsVec);
		x.appendChild("multiDimType").set(multiDimType);
		x.appendChild("multiDimIndex").set(multiDimIndex);
		x.appendChild("bIsButton").set(bIsButton);

		// Feedback block: enabled flag + status + ch/num/onVal/offVal so a
		// user can edit the XML to drive any device's LEDs / motors.
		auto fb = x.appendChild("feedback");
		fb.appendChild("enabled").set(bSendFeedback);
		fb.appendChild("status").set((int)feedbackStatus);
		fb.appendChild("channel").set(feedbackChannel);
		fb.appendChild("controlNum").set(feedbackNum);
		fb.appendChild("onValue").set(feedbackOnValue);
		fb.appendChild("offValue").set(feedbackOffValue);

		// Persist the note-to-int snapshot so reloaded bindings keep their
		// "recall this exact int value" behaviour. Only written when set so
		// legacy files stay legacy.
		if(bHasStoredValue){
			x.appendChild("storedIntValue").set(storedIntValue);
		}
	}
}
void ofParameterMidiInfo::sendFeedback(std::shared_ptr<ofxMidiOut> midiOut){
	if(!midiOut || !midiOut->isOpen()) return;
	if(!isSendFeedback()) return;

	int outValue;
	if(param && param->type() == typeid(ofParameter<bool>).name()){
		outValue = param->cast<bool>().get() ? feedbackOnValue : feedbackOffValue;
	}else{
		// Continuous parameter: map full MIDI range (0-127) into the
		// device's "off..on" range so e.g. encoder rings light up
		// proportionally to the parameter value.
		int v = getParameterAsMidiValue();
		outValue = (int)ofMap(v, 0, 127, feedbackOffValue, feedbackOnValue, true);
	}

	switch(feedbackStatus){
		case MIDI_NOTE_ON:
			midiOut->sendNoteOn(feedbackChannel, feedbackNum, outValue);
			break;
		case MIDI_NOTE_OFF:
			midiOut->sendNoteOff(feedbackChannel, feedbackNum, outValue);
			break;
		case MIDI_PITCH_BEND:
			midiOut->sendPitchBend(feedbackChannel, outValue);
			break;
		case MIDI_CONTROL_CHANGE:
		default:
			midiOut->sendControlChange(feedbackChannel, feedbackNum, outValue);
			break;
	}
}
bool ofParameterMidiInfo::loadFromXml(ofXml& xml){
	bool ret = false;
	if(param){
		if(xml){
			if(ofxParamMidiSync::joinStrings(param->getGroupHierarchyNames(), "/") == xml.getChild("groupHierarchyNames").getValue()){
				dims = xml.getChild("dims").getIntValue();
				controlNum = xml.getChild("controlNum").getIntValue();
				channel = xml.getChild("channel").getIntValue();   // was getBoolValue() — fixed
				bIsColor = xml.getChild("bIsColor").getBoolValue();
				bIsVec = xml.getChild("bIsVec").getBoolValue();
				multiDimType = xml.getChild("multiDimType").getIntValue();
				multiDimIndex = xml.getChild("multiDimIndex").getIntValue();
				bIsButton = xml.getChild("bIsButton").getBoolValue();

				// inputStatus: missing in legacy files -> default to CC.
				if(auto isNode = xml.getChild("inputStatus")){
					inputStatus = (MidiStatus)isNode.getIntValue();
				}else{
					inputStatus = MIDI_CONTROL_CHANGE;
				}

				// feedback block: missing in legacy files -> fall back to
				// flat <bSendFeedback> and mirror the input message.
				if(auto fb = xml.getChild("feedback")){
					bSendFeedback   = fb.getChild("enabled").getBoolValue();
					feedbackStatus  = (MidiStatus)fb.getChild("status").getIntValue();
					feedbackChannel = fb.getChild("channel").getIntValue();
					feedbackNum     = fb.getChild("controlNum").getIntValue();
					feedbackOnValue  = fb.getChild("onValue").getIntValue();
					feedbackOffValue = fb.getChild("offValue").getIntValue();
				}else{
					// Legacy file: there was no per-binding gate, every
					// bound parameter echoed back. Preserve that here so
					// existing settings keep working.
					bSendFeedback = true;
					feedbackStatus  = inputStatus;
					feedbackChannel = channel;
					feedbackNum     = controlNum;
					feedbackOnValue  = 127;
					feedbackOffValue = 0;
				}

				// Note-to-int snapshot: present only when the binding was
				// learned for an integer parameter. Absent in legacy files,
				// in which case the binding falls back to velocity-mapping.
				if(auto siv = xml.getChild("storedIntValue")){
					storedIntValue  = siv.getIntValue();
					bHasStoredValue = true;
				}else{
					bHasStoredValue = false;
				}

				ret = true;
			}
		}
	}
	return ret;
}

std::string ofParameterMidiInfo::toString() const {
	std::ostringstream o;
	const std::string sep = "----------------------------------------";
	o << sep << "\n";
	o << "ofParameterMidiInfo @ " << this << "\n";
	o << sep << "\n";

	o << "  parameter:\n";
	if(param){
		o << "    path           : " << ofxParamMidiSync::joinStrings(param->getGroupHierarchyNames(), "/") << "\n";
		o << "    name           : " << param->getName() << "\n";
		o << "    type           : " << param->type() << "\n";
		o << "    isInteger      : " << (const_cast<ofParameterMidiInfo*>(this)->isIntegerParam() ? "yes" : "no") << "\n";
	}else{
		o << "    <null>\n";
	}

	o << "  input:\n";
	o << "    status         : " << midiStatusToString(inputStatus) << "\n";
	o << "    channel        : " << channel << "\n";
	o << "    controlNum     : " << controlNum << "\n";

	o << "  shape:\n";
	o << "    dims           : " << dims << "\n";
	o << "    bIsColor       : " << (bIsColor ? "true" : "false") << "\n";
	o << "    bIsVec         : " << (bIsVec   ? "true" : "false") << "\n";
	o << "    multiDimType   : " << multiDimType << " (" << multiDimTypeToString(multiDimType) << ")\n";
	o << "    multiDimIndex  : " << multiDimIndex << "\n";
	o << "    isMultiDim     : " << (const_cast<ofParameterMidiInfo*>(this)->isMultiDim() ? "yes" : "no") << "\n";

	o << "  flags:\n";
	o << "    bIsButton      : " << (bIsButton      ? "true" : "false") << "\n";
	o << "    bSendFeedback  : " << (bSendFeedback  ? "true" : "false") << "\n";
	o << "    bNeedSmoothing : " << (bNeedSmoothing ? "true" : "false") << "\n";

	o << "  runtime:\n";
	o << "    lastValue      : " << lastValue   << "\n";
	o << "    smoothValue    : " << smoothValue << "\n";
	if(param){
		o << "    midiValueNow   : " << const_cast<ofParameterMidiInfo*>(this)->getParameterAsMidiValue() << "\n";
	}

	o << "  feedback:\n";
	o << "    enabled        : " << (bSendFeedback ? "true" : "false") << "\n";
	o << "    status         : " << midiStatusToString(feedbackStatus) << "\n";
	o << "    channel        : " << feedbackChannel << "\n";
	o << "    controlNum     : " << feedbackNum << "\n";
	o << "    onValue        : " << feedbackOnValue  << "\n";
	o << "    offValue       : " << feedbackOffValue << "\n";

	o << "  storedInt:\n";
	o << "    bHasStoredValue: " << (bHasStoredValue ? "true" : "false") << "\n";
	if(bHasStoredValue){
		o << "    storedIntValue : " << storedIntValue << "\n";
	}
	o << sep;
	return o.str();
}

void ofParameterMidiInfo::print() const {
	ofLogNotice("ofParameterMidiInfo") << "\n" << toString();
}
