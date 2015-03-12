#pragma once
#include "Camera.h"
#include "InputManager.h"
class FirstPersonCControl :
	public CameraControl
{
private:
	InputManager* _input;
	float _fMovementSpeed;
	float _lMovementSpeed;
	float _rMovementSpeed;
	float _bMovementSpeed;
	float _uMovementSpeed;
	float _dMovementSpeed;
	float _pURotationSpeed;
	float _pDRotationSpeed;
	float _yLRotationSpeed;
	float _yRRotationSpeed;
	float _rLRotationSpeed;
	float _rRRotationSpeed;
public:
	FirstPersonCControl(InputManager* input, float MovementSpeed, float RotationSpeed);
	FirstPersonCControl(InputManager* input, float forwardSpeed, float backwardSpeed, float leftSpeed, 
		float rightSpeed, float upSpeed, float downSpeed, float pitchUpSpeed, float pitchDownSpeed, 
		float yawLeftSpeed, float yawRightSpeed, float rollLeftSpeed, float rollRightSpeed);

	virtual void Update(Camera* camera, float frameTime);

	~FirstPersonCControl(void);
};

