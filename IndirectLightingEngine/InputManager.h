#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>


class InputManager
{
private:
	LPDIRECTINPUT8 inputObject;
	LPDIRECTINPUTDEVICE8 keyboard;
	
	__int64 _previousTime;
	float _frameTime;
	float _secondsPerTick;

	std::vector<float> _frameRates;
	float _frameRate;

	BYTE keyBuffer[256];
	BYTE prevKeys[256];

public:
	InputManager(HINSTANCE h, HWND hwnd);
	
	void Update(void);

	bool KeyDown(int key);
	bool KeyPressed(int key);
	float GetFrameTime(){return _frameTime;}
	float GetFrameRate(){return _frameRate;}

	~InputManager(void);
};

