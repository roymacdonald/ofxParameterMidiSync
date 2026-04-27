//
//  Utils.h
//  example
//
//  Created by Roy Macdonald on 4/28/18.
//
//

#pragma once
#include "ofxMidiConstants.h"
#include "ofxMidiMessage.h"

namespace ofxParamMidiSync{
	//------------------------------------------------------------------------------------------
	// Compact "type" tag used to disambiguate identifier collisions in the
	// synced map. Without this, CC#36 and Note#36 would map to the same key.
	enum MessageType {
		TYPE_CC         = 0,
		TYPE_NOTE       = 1,   // covers NoteOn / NoteOff (treated as a single binding)
		TYPE_PITCH_BEND = 2,
		TYPE_UNKNOWN    = 0xF
	};
	//------------------------------------------------------------------------------------------
	static inline MessageType typeFromStatus(MidiStatus s){
		switch(s){
			case MIDI_NOTE_ON:
			case MIDI_NOTE_OFF:		return TYPE_NOTE;
			case MIDI_CONTROL_CHANGE:	return TYPE_CC;
			case MIDI_PITCH_BEND:		return TYPE_PITCH_BEND;
			default:			return TYPE_UNKNOWN;
		}
	}
	//------------------------------------------------------------------------------------------
	// The "control number" depends on the message type:
	// - CC: msg.control
	// - NoteOn/Off: msg.pitch
	// - PitchBend: 0 (the entire channel is one bender)
	static inline int numberFromMessage(const ofxMidiMessage& m){
		switch(m.status){
			case MIDI_NOTE_ON:
			case MIDI_NOTE_OFF:		return m.pitch;
			case MIDI_CONTROL_CHANGE:	return m.control;
			default:			return 0;
		}
	}
	//------------------------------------------------------------------------------------------
	// Pack (type, number) into a single int key so we can keep one synced map
	// without CC#7 and Note#7 colliding. Channel is intentionally NOT part of
	// the key so the same binding works regardless of channel reassignment;
	// channel is recorded inside ofParameterMidiInfo for feedback only.
	static inline int makeKey(MessageType type, int num){
		return (int(type) << 8) | (num & 0xFF);
	}
	static inline int makeKey(MidiStatus status, int num){
		return makeKey(typeFromStatus(status), num);
	}
	static inline int makeKey(const ofxMidiMessage& m){
		return makeKey(typeFromStatus(m.status), numberFromMessage(m));
	}
	//------------------------------------------------------------------------------------------
	static inline bool isParameterGroup(ofAbstractParameter* param){
		if(param){
			return (param->type() == typeid(ofParameterGroup).name());
		}
		return false;
	}
//------------------------------------------------------------------------------------------
static inline string joinStrings(vector<string> s, string delim){
	string r = "";
	for(int i = 0; i < s.size(); i++){
		r += s[i];
		if (i < s.size() -1) {
			r += delim;
		}
	}
	return r;
}
//------------------------------------------------------------------------------------------
static inline int isVecParam(ofAbstractParameter* param){
		if(param == nullptr) return 0;
		int ret = 0;
		if (param->type()==typeid(ofParameter<glm::vec2>).name()) {
			ret = 2;
		}else if (param->type()==typeid(ofParameter<glm::vec3>).name()) {
			ret = 3;
		}else if (param->type()==typeid(ofParameter<glm::vec4>).name()) {
			ret = 4;
		}
		
		return  ret;
	}
//------------------------------------------------------------------------------------------
static inline int isColorParam(ofAbstractParameter* param){
		if(param == nullptr) return 0;
		int ret = 0;
		if (param->type()==typeid(ofParameter<ofColor>).name()){
			ret = 8;
		}else if(param->type()==typeid(ofParameter<ofShortColor>).name()){
			ret = 16;
		}else if(param->type()==typeid(ofParameter<ofFloatColor>).name()) {
			ret = 32;
		}
		return  ret;
	}
//------------------------------------------------------------------------------------------
static inline void printParamGroupHierarchy(shared_ptr<ofAbstractParameter> param){
		if(param){
			auto  names = param->getGroupHierarchyNames();
			for(auto& n: names){
				cout << n << "/";
			}
			cout << endl;
		}
	}
//------------------------------------------------------------------------------------------
static inline void printParamGroupElements(ofParameterGroup& group, const std::string& prefix = ""){
		cout << prefix << "---- " << group.getName() << endl;
		for(auto& g: group){
			if(g->type() == typeid(ofParameterGroup).name()){
				printParamGroupElements(*static_pointer_cast<ofParameterGroup>(g), prefix+"    ");
			}else{
				cout<< "    " << prefix << g->getName() << endl;
				printParamGroupHierarchy(g);
			}
		}
	}
//------------------------------------------------------------------------------------------
static inline ofAbstractParameter* findParamInGroup(ofParameterGroup& group, const std::string& paramPath){
		ofAbstractParameter* param = nullptr;
		vector<string> groupHierarchyNames = ofSplitString( paramPath, "/");
		if(groupHierarchyNames.size()){
			param  = static_cast<ofAbstractParameter*> (&group);
			if (group.getName() == groupHierarchyNames[0]) {
				for (int i = 1; i < groupHierarchyNames.size(); i++) {
					if (param->type() == typeid(ofParameterGroup).name()) {
						if (static_cast<ofParameterGroup*>(param)->contains(groupHierarchyNames[i])) {
							param = &static_cast<ofParameterGroup*>(param)->get(groupHierarchyNames[i]);
						}else{
							//						printParamGroupHierarchy(group);
							param = nullptr;
							break;
							cout << "ruta incorrecta" << endl;
						}
					}
				}
			}
		}
		
		return param;
	}
}
