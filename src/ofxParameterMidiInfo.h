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

    // Status of the incoming MIDI message that drives this parameter.
    // Defaults to CC for back-compat with old XML files. NOTE_OFF is
    // normalised to NOTE_ON (one binding represents press + release of
    // a single note).
    MidiStatus inputStatus = MIDI_CONTROL_CHANGE;

    // ---- feedback configuration -----------------------------------------
    // What to send BACK to the controller (e.g. to drive an LED or a
    // motorised fader). The feedback message is independent of the input
    // message: a Launch Control XL pad, for example, listens on NoteOn
    // velocity > 0 but lights its LED green by sending NoteOn back with
    // velocity 0x3C (= 60). All five fields are persisted to XML so any
    // device with feedback semantics can be supported just by editing
    // the settings file — no code changes required.
    MidiStatus feedbackStatus = MIDI_CONTROL_CHANGE;
    int feedbackChannel = 0;     // MIDI channel (1-16) for outgoing feedback
    int feedbackNum     = 0;     // CC number / note pitch for outgoing feedback
    int feedbackOnValue  = 127;  // value sent for "true" (or continuous max)
    int feedbackOffValue = 0;    // value sent for "false" (or continuous min)
    // ---------------------------------------------------------------------

    // ---- note-to-int snapshot binding -----------------------------------
    // When a MIDI note is learned to a scalar integer ofParameter, the
    // parameter's current value is captured here so that pressing the note
    // later restores that exact value (e.g. "this pad recalls cue index 5").
    // This turns note bindings into recall triggers instead of velocity-to-
    // range mappings.
    int  storedIntValue   = 0;
    bool bHasStoredValue  = false;
    // ---------------------------------------------------------------------

    void setNewValue(int value, bool bUseSmoothing);
    void updateSmoothing(float smoothFactor);
    void saveToXml(ofXml& xml);
    bool loadFromXml(ofXml& xml);
	void mapValueToParameter(float value);
	int  getParameterAsMidiValue();
	bool isSendFeedback();
	bool isMultiDim();
	bool isIntegerParam() const;
	void captureIntValueFromParam();
	void applyStoredIntValue();
	
	void sendFeedback(std::shared_ptr<ofxMidiOut> midiOut);

	// Human-readable dump of every field, formatted as an aligned multi-line
	// block. `print()` sends it to ofLogNotice; `toString()` returns it so
	// callers can route it elsewhere (UI, file, ostream).
	std::string toString() const;
	void print() const;

	friend std::ostream& operator<<(std::ostream& os, const ofParameterMidiInfo& info){
		return os << info.toString();
	}

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
