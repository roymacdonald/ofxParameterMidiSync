//
//  ofxMidiRecorder.h
//  d4nSFPCRunner
//
//  Created by Roy Macdonald on 16-12-15.
//
//
#pragma once
#include "ofMain.h"
#include "ofxMidi.h"
#include "nanoKontrolConstants.h"
class ofxMidiNanoKontrolButtons{
public:
    ~ofxMidiNanoKontrolButtons(){
        if(bIsSetup)midi.closePort();
    }
    ofxMidiOut midi;
    bool bIsSetup;
    void setup(int device){
        bIsSetup = midi.openPort(device);
    }
    void setRec(bool on = true){
        if(bIsSetup)
            midi.sendControlChange(1, NANO_KONTROL_KEY_REC, on?127:0);
    }
    void setStop(bool on = true){
        if(bIsSetup)
            midi.sendControlChange(1, NANO_KONTROL_KEY_STOP, on?127:0);
    }
    void setPlay(bool on = true){
        if(bIsSetup)
            midi.sendControlChange(1, NANO_KONTROL_KEY_PLAY, on?127:0);
    }
    void setRew(bool on = true){
        if(bIsSetup)
            midi.sendControlChange(1, NANO_KONTROL_KEY_REW, on?127:0);
    }
    void setFfw(bool on = true){
        if(bIsSetup)
            midi.sendControlChange(1, NANO_KONTROL_KEY_FFW, on?127:0);
    }
};

///--------RECORDING & PLAYBACK
class ofxMidiRecordingEvent{
public:
    
    ofxMidiRecordingEvent(){
        time = 0;
    }
    ofxMidiRecordingEvent(const ofxMidiMessage& from, uint64_t time){
        this->time = time;
        this->message = from;
    }
    ofxMidiRecordingEvent(const ofxMidiRecordingEvent& from){
        this->time = from.getTime();
        this->message = from.getMessage();
    }
    ofxMidiRecordingEvent& operator=(const ofxMidiRecordingEvent& from){
        this->time = from.getTime();
        this->message = from.getMessage();
    }
    
    uint64_t getTime() const{return time;}
    const ofxMidiMessage& getMessage() const {return message;}
    //    ofxMidiMessage getMessage;
    //protected:
    uint64_t time;
    ofxMidiMessage message;
};

class ofxMidiRecorder: public ofxMidiListener{
public:
    ofxMidiRecorder();
    void record();
    void stop();
    void clear();
    bool isRecording(){return bRecording;}
    
    void newMidiMessage(ofxMidiMessage& msg);
    
    const vector<ofxMidiRecordingEvent>& getData(){return recData;}
    
    vector<ofxMidiRecordingEvent> recData;
    bool bRecording;
    uint64_t recordingStartTime;
    ofEvent<void> recStartE, recEndE;
    shared_ptr<ofxMidiNanoKontrolButtons> kontrolButtons;
};
class ofxMidiPlayer: public ofxMidiListener{
public:
    ofxMidiPlayer();
    ~ofxMidiPlayer();
    void setup(ofxMidiListener *l);
    void update(ofEventArgs& e);
    void play();
    void stop();
    void clear();
    void setData(const vector<ofxMidiRecordingEvent>& data);
    vector<ofxMidiRecordingEvent> data;
    ofxMidiListener* listener;
    bool bPlaying, bLoopPlayback;
    uint64_t  playingStartTime, playbackIndex;
    ofEvent<void> playE, stopE;
    
    void newMidiMessage(ofxMidiMessage& msg);
    shared_ptr<ofxMidiNanoKontrolButtons> kontrolButtons;
};

static void saveMidi(const vector<ofxMidiRecordingEvent>& data, float duration, string path){
    cout << __PRETTY_FUNCTION__ << endl;
    ofXml xml;
    auto midi = xml.appendChild("Midi");
	
    auto meta = midi.appendChild("Meta");
    meta.appendChild("duration").set(duration);


	for (int i =0; i < data.size(); i++) {
		auto x = midi.appendChild("midiEvent");
		x.appendChild("time").set(data[i].time);
		x.appendChild("control").set(data[i].message.control);
		x.appendChild("value").set(data[i].message.value);
		x.appendChild("deltatime").set(data[i].message.deltatime);
		x.appendChild("status").set((int)data[i].message.status);
		x.appendChild("channel").set(data[i].message.channel);
		x.appendChild("pitch").set(data[i].message.pitch);
		x.appendChild("velocity").set(data[i].message.velocity);
	}
    if(xml.save(path)){
        cout << "save midi to  " << path << endl;
    }else{
        cout << "couldnt save xml" << endl;
    }
}
static void loadMidi(vector<ofxMidiRecordingEvent>& data, float *duration, string path){
    cout << "loadMidi " << path << endl;
    ofXml xml;
    xml.load(path);
	auto meta = xml.getChild("Meta");
    if(meta) {
        *duration = meta.getChild("duration").getFloatValue();
    }
	auto midiEvents = xml.find("//Midi/midiEvent");
	for(auto & me: midiEvents){
		ofxMidiRecordingEvent e;
		e.time = me.getChild("time").getIntValue();
		e.message.control = me.getChild("control").getIntValue();
		e.message.value = me.getChild("value").getIntValue();
		e.message.channel = me.getChild("channel").getIntValue();
		e.message.pitch = me.getChild("pitch").getIntValue();
		e.message.velocity = me.getChild("velocity").getIntValue();
		e.message.deltatime = me.getChild("deltatime").getFloatValue();
		
		e.message.status = MidiStatus(me.getChild("status").getIntValue());
           //     cout << "New Midi Event: " << e.time << " control: " << e.message.control << " value: " << e.message.value << endl;
		data.push_back(e);
	}
}
