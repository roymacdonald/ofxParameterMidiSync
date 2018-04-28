//
//  nanoKontrolConstants.h
//  d4nSFPCRunner
//
//  Created by Roy Macdonald on 16-12-15.
//
//

#pragma once
#define NANO_KONTROL_KEY_PLAY 41
#define NANO_KONTROL_KEY_STOP 42
#define NANO_KONTROL_KEY_REW 43
#define NANO_KONTROL_KEY_FFW 44
#define NANO_KONTROL_KEY_REC 45
#define NANO_KONTROL_KEY_CICLE 46
#define NANO_KONTROL_KEY_TRACK_PREV 58
#define NANO_KONTROL_KEY_TRACK_NEXT 59
#define NANO_KONTROL_KEY_MARKER_SET 60
#define NANO_KONTROL_KEY_MARKER_PREV 61
#define NANO_KONTROL_KEY_MARKER_NEXT 62
#define NANO_KONTROL_START_SLIDER 0
#define NANO_KONTROL_START_KNOB 16
#define NANO_KONTROL_START_REC 64
#define NANO_KONTROL_START_MUTE 48
#define NANO_KONTROL_START_SOLO 32


class ofxNanoKontrol{
public:
	inline static bool isRecButton(int cNum){
		return isControlInRange(cNum, NANO_KONTROL_START_REC, 8);
	}
	inline static bool isMuteButton(int cNum){
		return isControlInRange(cNum, NANO_KONTROL_START_MUTE, 8);
	}
	inline static bool isSoloButton(int cNum){
		return isControlInRange(cNum, NANO_KONTROL_START_SOLO, 8);
	}
	inline static bool isSlider(int cNum){
		return isControlInRange(cNum, NANO_KONTROL_START_SLIDER, 8);
	}
	inline static bool isKnob(int cNum){
		return isControlInRange(cNum, NANO_KONTROL_START_KNOB, 8);
	}
inline static bool isButton(int cNum){
	return isRecButton(cNum) ||
		isMuteButton(cNum)||
		isSoloButton(cNum)||
	(cNum >= NANO_KONTROL_KEY_PLAY && cNum <= NANO_KONTROL_KEY_CICLE)||
	(cNum >= NANO_KONTROL_KEY_TRACK_PREV && cNum <= NANO_KONTROL_KEY_MARKER_NEXT);
}
protected:
	inline static bool isControlInRange(int cNum, int start, int totalNum){
		return (cNum >= start && cNum < start + totalNum);
	}
	
};
