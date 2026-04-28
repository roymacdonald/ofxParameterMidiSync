//
//  ofxParameterMidiSync.cpp
//  emptyExample
//
//  Created by Roy Macdonald on 18-08-15.
//
//

#include "ofxParameterMidiSync.h"
#include "nanoKontrolConstants.h"

#include "ofxParamMidiSyncUtils.h"

//-----------------------------------------------------
ofxParameterMidiSync::ofxParameterMidiSync(){
}
//-----------------------------------------------------
ofxParameterMidiSync::~ofxParameterMidiSync(){
    enableMidi(false);
    learningParameter = nullptr;
}
//-----------------------------------------------------
std::shared_ptr<ofxMidiIn> ofxParameterMidiSync::getMidiIn(){
	if(!midiIn){
		midiIn = std::make_shared<ofxMidiIn>();
	}
	return midiIn;
}
//-----------------------------------------------------
std::shared_ptr<ofxMidiOut> ofxParameterMidiSync::getMidiOut(){
	if(!midiOut){
		midiOut = std::make_shared<ofxMidiOut>();
	}
	return midiOut;
}
//-----------------------------------------------------
void ofxParameterMidiSync::setup(int portNum, ofAbstractParameter & parameters, bool bUseRecorder, bool bUsePlayer){
		setup(portNum, portNum, parameters, bUseRecorder, bUsePlayer);
}
//-----------------------------------------------------
void ofxParameterMidiSync::setup(int portNum, ofParameterGroup & parameters, bool bUseRecorder, bool bUsePlayer){
    setup(portNum, portNum, parameters, bUseRecorder, bUsePlayer);
}
//-----------------------------------------------------
void ofxParameterMidiSync::setup(int inPortNum, int outPortNum, ofAbstractParameter & parameters, bool bUseRecorder, bool bUsePlayer){
	if(ofxParamMidiSync::isParameterGroup(&parameters)){
		setup(inPortNum, outPortNum, static_cast<ofParameterGroup&>(parameters), bUseRecorder, bUsePlayer);
	}	
}
//-----------------------------------------------------
void ofxParameterMidiSync::setup(int inPortNum, int outPortNum, ofParameterGroup & parameters, bool bUseRecorder, bool bUsePlayer){
	setSyncGroup(parameters);
    setup(inPortNum, outPortNum, bUseRecorder, bUsePlayer);
}

//-----------------------------------------------------
void ofxParameterMidiSync::setup(int portNum, bool bUseRecorder, bool bUsePlayer){
	setup(portNum, portNum, bUseRecorder, bUsePlayer);
}

//-----------------------------------------------------
void ofxParameterMidiSync::setup(int inPortNum, int outPortNum, bool bUseRecorder, bool bUsePlayer){

    bIsSetup = true;
	filePath = "ofxParameterMidiSyncSettings.xml";
	
	// parameters.add(bLoad.set("Load"));
	// parameters.add(bSave.set("Save"));
	// parameters.add(bReset.set("Reset"));
	// parameters.add(bLearning.set("Learn", false));
	// parameters.add(bUnlearning.set("Unlearn", false));
	// parameters.add(bMidiEnabled.set("MidiEnabled", false));
	// parameters.add(bSmoothingEnabled.set("Smoothing Enabled", false));
	// parameters.add(smoothing.set("Smoothing",0.5,0,1));
	this->inPortNum.setMax(std::max(1, getMidiIn()->getNumInPorts()) -1);
	this->outPortNum.setMax(std::max(1, getMidiOut()->getNumOutPorts()) -1);
	this->inPortNum = inPortNum;
	this->outPortNum = outPortNum;
	 
	paramsListeners.push(bLoad.newListener([&](){ load(); }));
	paramsListeners.push(bSave.newListener([&](){ save(); }));
	paramsListeners.push(bReset.newListener(this, &ofxParameterMidiSync::reset));
	
	paramsListeners.push(bLearning.newListener([&](bool &){
		if(bLearning && bIsSetup ) {
			bUnlearning = false;
		}
	}));
	paramsListeners.push(bUnlearning.newListener([&](bool &){
		if ( bUnlearning && bIsSetup ) {
			if (bLearning) {
				learningParameter = nullptr;
				bLearning = false;
			}
		}
	}));
	paramsListeners.push(bMidiEnabled.newListener([&](bool &){
		if(bMidiEnabled){
			openMidi();
		}else{
			closeMidi();
		}
	}));
	paramsListeners.push(bSmoothingEnabled.newListener([&](bool &){
		if(bSmoothingEnabled){
			updateListener = ofEvents().update.newListener(this, &ofxParameterMidiSync::update);
		}else{
			updateListener.unsubscribe();
		}
	}));
	paramsListeners.push(this->inPortNum.newListener([&](int &){
		if(bMidiInOpened){
			//this will close the current opened midi and reopen using the new port number
			bMidiEnabled = false;
			bMidiEnabled = true;
		}
	}));
	paramsListeners.push(this->outPortNum.newListener([&](int &){
		if(bMidiOutOpened){
			//this will close the current opened midi and reopen using the new port number
			bMidiEnabled = false;
			bMidiEnabled = true;
		}
	}));

	if(bUsePlayer || bUseRecorder){
		kontrolButtons = std::make_shared<ofxMidiNanoKontrolButtons>();
		kontrolButtons->setup(getMidiOut());
	}
	if(bUsePlayer){
		player = std::make_shared<ofxMidiPlayer>();
		player->setup(this);
		player->kontrolButtons = kontrolButtons;
	}
	if(bUseRecorder){
		recorder = std::make_shared<ofxMidiRecorder>();
		recorder->kontrolButtons = kontrolButtons;
	}
	
	enableMidi(true);
    
}
//-----------------------------------------------------
void ofxParameterMidiSync::update(ofEventArgs& e){
    for (map<int, shared_ptr<ofParameterMidiInfo> > ::iterator it = synced.begin(); it != synced.end(); ++it) {
        it->second->updateSmoothing(smoothing);
    }
}
//-----------------------------------------------------
void ofxParameterMidiSync::setSyncGroup( ofAbstractParameter & parameters){
	if(ofxParamMidiSync::isParameterGroup(&parameters)){
		setSyncGroup(static_cast<ofParameterGroup&>(parameters));
	}
}
//-----------------------------------------------------
void ofxParameterMidiSync::setSyncGroup( ofParameterGroup & parameters){
    syncGroup = parameters;
//	ofxParamMidiSync::printParamGroupElements(syncGroup);
    bParameterGroupSetup = true;
    voidParamListeners.unsubscribeAll();
    scanForVoidParams(syncGroup);
//    if (bAutoLink) {
//        synced.clear();
//        for (int i = 0; i < parameters.size(); i++) {
//            linkMidiToOfParameter(i, parameters.get(i));
//        }
//    }
}
//-----------------------------------------------------
void ofxParameterMidiSync::scanForVoidParams(ofParameterGroup & group){
    for(auto & p : group){
        if(p->type() == typeid(ofParameterGroup).name()){
            scanForVoidParams(*std::static_pointer_cast<ofParameterGroup>(p));
        }else if(p->type() == typeid(ofParameter<void>).name()){
            auto voidParam = std::static_pointer_cast<ofParameter<void>>(p);
            voidParamListeners.push(voidParam->newListener([this, voidParam](){
                parameterChanged(*voidParam);
            }));
        }
    }
}
//-----------------------------------------------------
void ofxParameterMidiSync::reset(){
    synced.clear();
    bLearning = false;
    bUnlearning = false;
    learningParameter = nullptr;
}
//--------------------------------------------------------------
void ofxParameterMidiSync::openMidi(){
	if (!bIsSetup) return;

	if (!bMidiInOpened && inPortNum >= 0) {
		getMidiIn()->listInPorts();
		bMidiInOpened = midiIn->openPort(inPortNum);
		if (bMidiInOpened) {
			midiIn->ignoreTypes(false, false, false);
			midiIn->addListener(this);
			if (bParameterGroupSetup) {
				ofAddListener(syncGroup.parameterChangedE(), this, &ofxParameterMidiSync::parameterChanged);
			}

			if(recorder)midiIn->addListener(recorder.get());
			if(player)midiIn->addListener(player.get());
		}
	}

	if (!bMidiOutOpened && outPortNum >= 0) {
		bMidiOutOpened = getMidiOut()->openPort(outPortNum);
	}

	if (!bMidiInOpened && !bMidiOutOpened) {
		bMidiEnabled.disableEvents();
		bMidiEnabled = false;
		bMidiEnabled.enableEvents();
	}
}
//--------------------------------------------------------------
void ofxParameterMidiSync::closeMidi(){
	if (!bIsSetup) return;

	if (bMidiInOpened) {
		if(midiIn){
			midiIn->closePort();
			midiIn->removeListener(this);
			if(recorder)midiIn->removeListener(recorder.get());
			if(player)  midiIn->removeListener(player.get());
		}
		if (bParameterGroupSetup) {
			ofRemoveListener(syncGroup.parameterChangedE(), this, &ofxParameterMidiSync::parameterChanged);
		}
		bMidiInOpened = false;
	}

	if (bMidiOutOpened) {
		if (midiOut) {
			midiOut->closePort();
		}
		bMidiOutOpened = false;
	}
}
//-----------------------------------------------------
void ofxParameterMidiSync::enableMidi(bool b){
	bMidiEnabled = b;
}
//-----------------------------------------------------
void ofxParameterMidiSync::learn(bool bLearn){
            bLearning = bLearn;
}
//-----------------------------------------------------
void ofxParameterMidiSync::unlearn(bool bUnlearn){
            bUnlearning = bUnlearn;
}
//-----------------------------------------------------
void ofxParameterMidiSync::parameterChanged( ofAbstractParameter & parameter ){
    if (bLearning ) {
        learningParameter = &parameter;
	}else{
		for(auto&s: synced){
			if(s.second && s.second->param->isReferenceTo(parameter)){
				s.second->sendFeedback(midiOut);
			}
		}
	}
}
//--------------------------------------------------------------
bool ofxParameterMidiSync::linkMidiToOfParameter(ofxMidiMessage& msg, ofAbstractParameter& param){
    return linkMidiToOfParameter(msg, &param);
}
//--------------------------------------------------------------
bool ofxParameterMidiSync::linkMidiToOfParameter(ofxMidiMessage& msg, ofAbstractParameter* param){
    if (param ==  nullptr) return false;

    int baseNum = ofxParamMidiSync::numberFromMessage(msg);
    int key = ofxParamMidiSync::makeKey(msg.status, baseNum);

    if (synced.count(key) == 0) {
        auto s = std::make_shared<ofParameterMidiInfo>(param, msg);
        synced[key] = s;
        if (s->isMultiDim()) {
            for (int i = 1; i < s->dims ; i++) {
                int subKey = ofxParamMidiSync::makeKey(msg.status, baseNum + i);
                synced[subKey] = std::make_shared<ofParameterMidiInfo>(param, msg, i);
            }
        }
        return true;
    }
    return false;
}
//--------------------------------------------------------------
bool ofxParameterMidiSync::load(string path){
	filePath = path;
	return load();
}
//--------------------------------------------------------------
bool ofxParameterMidiSync::load(){
    ofXml xml;
    bool bLoad = false;
	
    cout << "ofxParameterMidiSync::load(" << filePath << ");" << endl;
    if(bIsSetup){
        bLoad = xml.load(filePath);
        if (bLoad) {
			auto sync = xml.getChild("ofxParameterMidiSync");
			if(!syncGroup.getName().empty()){
				auto sg = sync.getChild(syncGroup.getName());
				if(sg){
					   sync = sg;
				}
			}
			auto PMinfo = sync.find("ofParameterMidiInfo");
			for(auto & info: PMinfo){
				int controlNum = info.getChild("controlNum").getIntValue();
				MidiStatus status = MIDI_CONTROL_CHANGE;
				if(auto isNode = info.getChild("inputStatus")){
					status = (MidiStatus)isNode.getIntValue();
				}
				int key = ofxParamMidiSync::makeKey(status, controlNum);
				if(synced.count(key) == 0){
					ofAbstractParameter* param = ofxParamMidiSync::findParamInGroup(syncGroup, info.getChild("groupHierarchyNames").getValue());
					if (param) {
						auto s = std::make_shared<ofParameterMidiInfo>(param);
						if(s->loadFromXml(info)){
							synced[key] = s;
						}
					}
				}
			}
        }
    }
    
    return bLoad;
}
//--------------------------------------------------------------
void ofxParameterMidiSync::save(string path){
	filePath = path;
	save();
}
//--------------------------------------------------------------
void ofxParameterMidiSync::save(){
    ofXml xml;
    auto sync = xml.appendChild("ofxParameterMidiSync");
    if(!syncGroup.getName().empty()) sync = sync.appendChild(syncGroup.getName());
	
    for (map<int, shared_ptr<ofParameterMidiInfo> > ::iterator i = synced.begin(); i != synced.end() ; i++) {
        i->second.get()->saveToXml(sync);
    }
	
    xml.save(filePath);
    
}
//--------------------------------------------------------------
void ofxParameterMidiSync::drawDebug(){
	
	string str = "Learning: " + (string)(bLearning?"YES":"NO")+"\n";
	str += "learningParameter: " + (string)((learningParameter==nullptr)?"nullptr": learningParameter->getName())+"\n";
	str += "controlNum: " + ofToString(midiMessage.control) + "\n";
	str += "lastMidiMessage: " + midiMessage.toString() + "\n";
	//	str += "is Recording: " +(string)(?"YES":"NO");
	
	ofDrawBitmapStringHighlight(str, 20, ofGetHeight() - 60);
	
}
//--------------------------------------------------------------
void ofxParameterMidiSync::drawInstructions(){
	if(!bLearning && !bUnlearning) return;

	std::string str;
	if(bLearning){
		if(learningParameter == nullptr){
			str  = "MIDI LEARN enabled\n";
			str += "1. Click or drag a parameter in the GUI to select it.\n";
			str += "2. Move a knob/slider or press a pad on your MIDI controller to bind it.\n";
			str += "Toggle Learn off to cancel.";
		}else{
			str  = "MIDI LEARN enabled\n";
			str += "Selected parameter: " + learningParameter->getName() + "\n";
			str += "Move a knob/slider or press a pad on your MIDI controller to bind it.\n";
			str += "Click another parameter to change selection, or toggle Learn off to cancel.";
		}
	}else if(bUnlearning){
		str  = "MIDI UNLEARN enabled\n";
		str += "Move a knob/slider or press a pad on your MIDI controller to remove its binding.\n";
		str += "Toggle Unlearn off to cancel.";
	}

	float w = 0;
	{
		size_t maxLine = 0;
		size_t cur = 0;
		for(char c : str){
			if(c == '\n'){ maxLine = std::max(maxLine, cur); cur = 0; }
			else cur++;
		}
		maxLine = std::max(maxLine, cur);
		w = maxLine * 8.0f;
	}
	float x = (ofGetWidth()  - w) * 0.5f;
	float y = 40;

	drawInstructions(x, y);
}
//--------------------------------------------------------------
void ofxParameterMidiSync::drawInstructions(float x, float y){
	if(!bLearning && !bUnlearning) return;

	std::string str;
	if(bLearning){
		if(learningParameter == nullptr){
			str  = "MIDI LEARN enabled\n";
			str += "1. Click or drag a parameter in the GUI to select it.\n";
			str += "2. Move a knob/slider or press a pad on your MIDI controller to bind it.\n";
			str += "Toggle Learn off to cancel.";
		}else{
			str  = "MIDI LEARN enabled\n";
			str += "Selected parameter: " + learningParameter->getName() + "\n";
			str += "Move a knob/slider or press a pad on your MIDI controller to bind it.\n";
			str += "Click another parameter to change selection, or toggle Learn off to cancel.";
		}
	}else if(bUnlearning){
		str  = "MIDI UNLEARN enabled\n";
		str += "Move a knob/slider or press a pad on your MIDI controller to remove its binding.\n";
		str += "Toggle Unlearn off to cancel.";
	}

	ofColor bg = bLearning ? ofColor(0, 90, 160, 220) : ofColor(160, 60, 0, 220);
	ofDrawBitmapStringHighlight(str, x, y, bg, ofColor::white);
}
//--------------------------------------------------------------
void ofxParameterMidiSync::newMidiMessage(ofxMidiMessage& msg) {
    if (!bIsSetup) return;

    ofxMidiMessage message = msg;
    midiMessage = message;

    // Only CC and Note On/Off drive parameter bindings. (Pitch bend,
    // sysex, clock, etc. are still surfaced via midiMessage for debug
    // but are not auto-routed.)
    const bool isCC   = (message.status == MIDI_CONTROL_CHANGE);
    const bool isNote = (message.status == MIDI_NOTE_ON || message.status == MIDI_NOTE_OFF);
    if (!isCC && !isNote) return;

    int baseNum = ofxParamMidiSync::numberFromMessage(message);
    int key     = ofxParamMidiSync::makeKey(message.status, baseNum);

    if (learningParameter != nullptr && bLearning) {
        // Don't learn from a NoteOff (vel=0 release) — wait for the press.
        if (message.status == MIDI_NOTE_OFF) return;
        if (isNote && message.value == 0)  return;

        if (bParameterGroupSetup) {
            if (linkMidiToOfParameter(message, learningParameter)) {
                cout << "learned  " << endl;
                learningParameter = nullptr;
                bLearning = false;
            }
        }
    }
    else if (bUnlearning) {
        if (synced.count(key) > 0) {
            int dims = synced[key]->dims;
            if (dims == 0) dims = 1;
            if (dims > 4) dims = 4;
            for (int i = 0; i < dims; i++) {
                int k = ofxParamMidiSync::makeKey(message.status, baseNum + i);
                if (synced.count(k)) synced.erase(k);
            }
            bUnlearning = false;
            cout << "unlearned  " << endl;
        }
    }
    else {
        if (synced.count(key)) {
            synced[key]->setNewValue(message.value, bSmoothingEnabled);
        }
    }
}
//--------------------------------------------------------------
void ofxParameterMidiSync::enableSmoothing(){
	bSmoothingEnabled = true;
}
//--------------------------------------------------------------
void ofxParameterMidiSync::disableSmoothing(){
	bSmoothingEnabled = false;
}
//--------------------------------------------------------------
bool ofxParameterMidiSync::isSmoothingEnabled(){
	return bSmoothingEnabled;
}
//--------------------------------------------------------------
void ofxParameterMidiSync::setupGui(float x, float y){
	syncSettingsGui = std::make_shared<ofxPanel>();
	syncSettingsGui->setup("syncSettingsGui");
	syncSettingsGui->setPosition(x,y);
	syncSettingsGui->add(parameters);
}
//--------------------------------------------------------------
void ofxParameterMidiSync::setGuiPosition(float x, float y ){
	if(syncSettingsGui)syncSettingsGui->setPosition(x,y);
}
//--------------------------------------------------------------
void ofxParameterMidiSync::drawGui(){
	if(syncSettingsGui){
		syncSettingsGui->draw();
	}
}
//--------------------------------------------------------------
void ofxParameterMidiSync::setFilePath(std::string path){
	filePath = path;
}
//--------------------------------------------------------------
std::string ofxParameterMidiSync::getFilePath(){
	return filePath;
}
