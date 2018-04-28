//
//  Utils.h
//  example
//
//  Created by Roy Macdonald on 4/28/18.
//
//

#pragma once
namespace ofxParamMidiSync{
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
