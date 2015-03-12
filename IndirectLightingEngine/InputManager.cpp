#include "InputManager.h"

InputManager::InputManager(HINSTANCE h, HWND hwnd)
{
	DirectInput8Create(h, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&inputObject, NULL);
	inputObject->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	keyboard->SetDataFormat(&c_dfDIKeyboard);
	keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	keyboard->Acquire();

	_frameTime = 0;
	__int64 li = 0;
	QueryPerformanceCounter((LARGE_INTEGER*) &li);
	_previousTime = li;
    QueryPerformanceFrequency((LARGE_INTEGER*) &li);
	_secondsPerTick = 1.0f/(float)li;

	//Might as well be optimistic
	_frameRate = 60.0f;
}

void InputManager::Update()
{
	//Update time
	__int64 li;
	QueryPerformanceCounter((LARGE_INTEGER*) &li);
	_frameTime = (li -_previousTime)*_secondsPerTick;
	_previousTime = li;

	//Store framerate
	_frameRates.push_back(1.0f/_frameTime);
	//Calculate average framerate every 6 frames
	if(_frameRates.size()>5)
	{
		_frameRate = 0;
		for(UINT i = 0; i < 6; ++i)
		{
			_frameRate += _frameRates[i];
		}
		_frameRate/=6.0f;
		_frameRates.clear();
	}

	//Store previous keyboard state
	memcpy(&prevKeys, &keyBuffer, sizeof(prevKeys));

	//Retrieve new keyboard state
	ZeroMemory(keyBuffer, sizeof(keyBuffer));
	HRESULT hr=keyboard->GetDeviceState( sizeof(keyBuffer), keyBuffer );

	if (FAILED(hr))
	{
		   // If input is lost then acquire and keep trying until we get it back 
		   hr=keyboard->Acquire();
		   while( hr == DIERR_INPUTLOST ) 
		   {          
				 hr = keyboard->Acquire();
		   }
		   // Could be we failed for some other reason
		   if (FAILED(hr))
			 return;
		   // Now read the state again
		   keyboard->GetDeviceState( sizeof(keyBuffer), keyBuffer );
	}
}

bool InputManager::KeyDown(int key)
{
	return (keyBuffer[key]& 0x80) != 0;
}

bool InputManager::KeyPressed(int key)
{
	//Return if pressed this frame but not last frame (on key down)
	bool prev = (prevKeys[key]& 0x80) != 0;
	bool current = (keyBuffer[key]& 0x80) != 0;
	return !prev && current;
}


InputManager::~InputManager(void)
{
	inputObject->Release();
	keyboard->Release();
}
