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
void ofxParameterMidiSync::setup(int portNum, ofAbstractParameter & parameters){//, bool bAutoLink){
	if(ofxParamMidiSync::isParameterGroup(&parameters)){
		setup(portNum, static_cast<ofParameterGroup&>(parameters));//, bAutoLink);
	}
}
//-----------------------------------------------------
void ofxParameterMidiSync::setup(int portNum, ofParameterGroup & parameters){//, bool bAutoLink){
	setSyncGroup(parameters);//, bAutoLink);
    setup(portNum);
}
//-----------------------------------------------------
void ofxParameterMidiSync::setup(int portNum){
    bIsSetup = true;
	parameters.setName("ofParameterMidiSync");
	parameters.add(filePath.set("Save/Load Path", "ofxParameterMidiSyncSettings.xml"));
	parameters.add(bLoad.set("Load"));
	parameters.add(bSave.set("Save"));
	parameters.add(bReset.set("Reset"));
	parameters.add(bLearning.set("Learn", false));
	parameters.add(bUnlearning.set("Unlearn", false));
	parameters.add(bMidiEnabled.set("MidiEnabled", false));
	parameters.add(bSmoothingEnabled.set("Smoothing Enabled", false));
	parameters.add(smoothing.set("Smoothing",0.5,0,1));
	parameters.add(this->portNum.set("Midi Port", portNum, 0, getMidiIn()->getNumInPorts() -1));
	paramsListeners.push(bLoad.newListener([&](){
		load();
	}));
	paramsListeners.push(bSave.newListener([&](){
		save();
	}));
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
	enableMidi(true);
    player.setup(this);

    kontrolButtons = shared_ptr<ofxMidiNanoKontrolButtons>(new ofxMidiNanoKontrolButtons);
    kontrolButtons->setup(0);
    player.kontrolButtons = kontrolButtons;
    recorder.kontrolButtons = kontrolButtons;
}
//-----------------------------------------------------
void ofxParameterMidiSync::update(ofEventArgs& e){
    for (map<int, shared_ptr<ofParameterMidiInfo> > ::iterator it = synced.begin(); it != synced.end(); ++it) {
        it->second->updateSmoothing(smoothing);
    }
}
//-----------------------------------------------------
void ofxParameterMidiSync::setSyncGroup( ofAbstractParameter & parameters){//, bool bAutoLink){
	if(ofxParamMidiSync::isParameterGroup(&parameters)){
		setSyncGroup(static_cast<ofParameterGroup&>(parameters));//, bAutoLink);
	}
}
//-----------------------------------------------------
void ofxParameterMidiSync::setSyncGroup( ofParameterGroup & parameters){//, bool bAutoLink){
    syncGroup = parameters;
	ofxParamMidiSync::printParamGroupElements(syncGroup);
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
//    enableMidi(false);
    synced.clear();
   // syncGroup.clear();
//    bParameterGroupSetup = false;
  //  bIsSetup = false;
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
			midiIn->addListener(&recorder);
			midiIn->addListener(&player);
			midiOut->openPort(portNum);
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
		midiIn->removeListener(&recorder);
		midiIn->removeListener(&player);
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
	
    cout << "ofxParameterMidiSync::load(" << filePath.get() << ");" << endl;
    if(bIsSetup){
        bLoad = xml.load(filePath.get());
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
//                            vector<string> groupHierarchyNames = ofSplitString( info.getChild("groupHierarchyNames").getValue(), "/");
							ofAbstractParameter* param = ofxParamMidiSync::findParamInGroup(syncGroup, info.getChild("groupHierarchyNames").getValue());
							//                            if(groupHierarchyNames.size()){
//                                param  = static_cast<ofAbstractParameter*> (&syncGroup);
//                                if (param->getName() == groupHierarchyNames[0]) {
//                                    for (int i = 1; i < groupHierarchyNames.size(); i++) {
//                                        if (param->type() == typeid(ofParameterGroup).name()) {
//                                            if (static_cast<ofParameterGroup*>(param)->contains(groupHierarchyNames[i])) {
//                                                param = &static_cast<ofParameterGroup*>(param)->get(groupHierarchyNames[i]);
//                                            }else{
//                                                param = nullptr;
//                                                break;
//                                                cout << "ruta incorrecta" << endl;
//                                            }
//                                        }
//                                    }
//                                }
//                            }
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
	
    xml.save(filePath.get());
    
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
					synced[message.control]->sendFeedback(midiOut);
					
                }
            }
        }
        midiMessage = message;
    }
}
//--------------------------------------------------------------
void ofxParameterMidiSync::enableSmoothing(){
//	if(!bSmoothingEnabled){
		bSmoothingEnabled = true;
//		updateListener = ofEvents().update.newListener(this, &ofxParameterMidiSync::update);
//	}
}
//--------------------------------------------------------------
void ofxParameterMidiSync::disableSmoothing(){
//	if(bSmoothingEnabled){
		bSmoothingEnabled = false;
//		updateListener.unsubscribe();
//	}
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
void ofxParameterMidiSync::drawGui(){
	if(syncSettingsGui){
		syncSettingsGui->draw();
	}
}
