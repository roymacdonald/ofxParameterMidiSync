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
ofxParameterMidiSync::ofxParameterMidiSync():bMidiEnabled(false), portNum(-1), bLearning(false), learningParameter(nullptr), bIsSetup(false), bUnlearning(false), bParameterGroupSetup(false){
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
	if(ofxParamMidiSync::isParameterGroup(&parameters)){
		setup(portNum, static_cast<ofParameterGroup&>(parameters), bUseRecorder, bUsePlayer);
	}
}
//-----------------------------------------------------
void ofxParameterMidiSync::setup(int portNum, ofParameterGroup & parameters, bool bUseRecorder, bool bUsePlayer){
	setSyncGroup(parameters);
    setup(portNum, bUseRecorder, bUsePlayer);
}
//-----------------------------------------------------
void ofxParameterMidiSync::setup(int portNum, bool bUseRecorder, bool bUsePlayer){
    bIsSetup = true;
	filePath = "ofxParameterMidiSyncSettings.xml";
	parameters.setName("ofParameterMidiSync");

	parameters.add(bLoad.set("Load"));
	parameters.add(bSave.set("Save"));
	parameters.add(bReset.set("Reset"));
	parameters.add(bLearning.set("Learn", false));
	parameters.add(bUnlearning.set("Unlearn", false));
	parameters.add(bMidiEnabled.set("MidiEnabled", false));
	parameters.add(bSmoothingEnabled.set("Smoothing Enabled", false));
	parameters.add(smoothing.set("Smoothing",0.5,0,1));
	parameters.add(this->portNum.set("Midi Port", portNum, 0, getMidiIn()->getNumInPorts() -1));
	
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
	paramsListeners.push(this->portNum.newListener([&](int &){
		if(bMidiOpened){
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
//    if (bAutoLink) {
//        synced.clear();
//        for (int i = 0; i < parameters.size(); i++) {
//            linkMidiToOfParameter(i, parameters.get(i));
//        }
//    }
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
	if (bIsSetup && bParameterGroupSetup && !bMidiOpened) {
		getMidiIn()->listInPorts();
		bMidiOpened = midiIn->openPort(portNum);
		if (bMidiOpened) {
			midiIn->ignoreTypes(true, true, false);
			midiIn->addListener(this);
			ofAddListener(syncGroup.parameterChangedE(),this,&ofxParameterMidiSync::parameterChanged);
			
			if(recorder)midiIn->addListener(recorder.get());
			if(player)midiIn->addListener(player.get());
			getMidiOut()->openPort(portNum);
		}else{
			bMidiEnabled.disableEvents();
			bMidiEnabled = false;
			bMidiEnabled.enableEvents();
		}
	}
}
//--------------------------------------------------------------
void ofxParameterMidiSync::closeMidi(){
	if (bIsSetup && bParameterGroupSetup && bMidiOpened) {
		if(midiIn){
		midiIn->closePort();
		midiIn->removeListener(this);
		if(recorder)midiIn->removeListener(recorder.get());
		if(player)  midiIn->removeListener(player.get());
		}
		
		ofRemoveListener(syncGroup.parameterChangedE(),this,&ofxParameterMidiSync::parameterChanged);
		if(midiOut){
			midiOut->closePort();
		}
		bMidiOpened = false;
	}
}
//-----------------------------------------------------
void ofxParameterMidiSync::enableMidi(bool b){
	bMidiEnabled = b;
	//    if (bIsSetup && bParameterGroupSetup) {
//        if (b != bMidiEnabled) {
//            if (b) {
//				return openMidi();
//			}else{
//				closeMidi();
//                return true;
//            }
//            bMidiEnabled = b;
//        }
//    }
//    return false;
}
//-----------------------------------------------------
void ofxParameterMidiSync::learn(bool bLearn){
//    if ( bIsSetup ) {
//        if (bLearning != bLearn) {
            bLearning = bLearn;
//            bUnlearning = false;
//        }
//    }
}
//-----------------------------------------------------
void ofxParameterMidiSync::unlearn(bool bUnlearn){
//    if ( bIsSetup ) {
//        if (bUnlearning != bUnlearn) {
//            if (bLearning) {
//                learningParameter = nullptr;
//            }
//            bLearning = false;
            bUnlearning = bUnlearn;
//        }
//    }
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
    
    if (synced.count(msg.control) == 0) {
		auto s = std::make_shared<ofParameterMidiInfo>(param, msg);
		synced[msg.control] = s;
        if (s->isMultiDim()) {
            for (int i = 1; i < s->dims ; i++) {
                synced[msg.control+i]  = std::make_shared<ofParameterMidiInfo>(param, msg,i);
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
                        if(synced.count(controlNum) == 0){
							ofAbstractParameter* param = ofxParamMidiSync::findParamInGroup(syncGroup, info.getChild("groupHierarchyNames").getValue());
                            if (param) {
								auto s = std::make_shared<ofParameterMidiInfo>(param);
								if(s->loadFromXml(info)){
									synced[controlNum] = s;
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
void ofxParameterMidiSync::newMidiMessage(ofxMidiMessage& msg) {
    if (bIsSetup) {
        ofxMidiMessage message = msg;
        if (message.status == MIDI_CONTROL_CHANGE) {
            if (learningParameter!= nullptr && bLearning) {
                if (bParameterGroupSetup) {
                    if (linkMidiToOfParameter(message, learningParameter)){
                        cout << "learned  " << endl;
                        learningParameter=nullptr;
                        bLearning = false;
                    }
                }
            }else if(bUnlearning){
                if (synced.count(message.control) > 0) {
                    int dims = synced[message.control]->dims;
                    if (dims == 0) dims = 1;
                    if (dims > 4) dims = 4;
                    for (int i = 0; i < dims; i++) {
                        if (synced.count(message.control+i)) {
                            synced.erase(message.control+i);
                        }
                    }
                    bUnlearning  = false;
                    cout << "unlearned  " << endl;
                }
            }else{
//                if (msg.control == NANO_KONTROL_KEY_FFW) {
////                    if (msg.value == 127) {
////                        ofNotifyEvent(ffwKeyPressed, this);
////                    }
//                }else if(msg.control == NANO_KONTROL_KEY_REW)  {
//                }else if(msg.control == NANO_KONTROL_KEY_STOP) {
//                }else if(msg.control == NANO_KONTROL_KEY_PLAY) {
//                }else if(msg.control == NANO_KONTROL_KEY_REC)  {
//				
				if (synced.count(message.control)) {
                    synced[message.control]->setNewValue(message.value, bSmoothingEnabled);
//					synced[message.control]->sendFeedback(midiOut);
					
                }
            }
        }
        midiMessage = message;
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
