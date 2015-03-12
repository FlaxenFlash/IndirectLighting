#include "FirstPersonCControl.h"


FirstPersonCControl::FirstPersonCControl(InputManager* input, float MovementSpeed, float RotationSpeed)
{
	_input = input;
	_fMovementSpeed = MovementSpeed;
	_lMovementSpeed = MovementSpeed;
	_rMovementSpeed = MovementSpeed;
	_bMovementSpeed = MovementSpeed;
	_uMovementSpeed = MovementSpeed;
	_dMovementSpeed = MovementSpeed;
	_pURotationSpeed = RotationSpeed;
	_pDRotationSpeed = RotationSpeed;
	_yLRotationSpeed = RotationSpeed;
	_yRRotationSpeed = RotationSpeed;
	_rLRotationSpeed = RotationSpeed;
	_rRRotationSpeed = RotationSpeed;
}

FirstPersonCControl::FirstPersonCControl(InputManager* input, float forwardSpeed, float backwardSpeed, float leftSpeed, 
	float rightSpeed, float upSpeed, float downSpeed, float pitchUpSpeed, float pitchDownSpeed, float yawLeftSpeed, 
	float yawRightSpeed, float rollLeftSpeed, float rollRightSpeed)
{
	_input = input;
	_fMovementSpeed = forwardSpeed;
	_lMovementSpeed = backwardSpeed;
	_rMovementSpeed = leftSpeed;
	_bMovementSpeed = leftSpeed;
	_uMovementSpeed = upSpeed;
	_dMovementSpeed = downSpeed;
	_pURotationSpeed = pitchUpSpeed;
	_pDRotationSpeed = pitchDownSpeed;
	_yLRotationSpeed = yawLeftSpeed;
	_yRRotationSpeed = yawRightSpeed;
	_rLRotationSpeed = rollLeftSpeed;
	_rRRotationSpeed = rollRightSpeed;
}

void FirstPersonCControl::Update(Camera* camera, float frameTime)
{
#pragma region Movement
	D3DXVECTOR3 translation(0,0,0);
	if(_input->KeyDown(DIK_W))
	{
		translation.z+=_fMovementSpeed*frameTime;
	}
	if(_input->KeyDown(DIK_A))
	{
		translation.x-=_lMovementSpeed*frameTime;
	}
	if(_input->KeyDown(DIK_S))
	{
		translation.z-=_bMovementSpeed*frameTime;
	}
	if(_input->KeyDown(DIK_D))
	{
		translation.x+=_rMovementSpeed*frameTime;
	}
	if(_input->KeyDown(DIK_R))
	{
		translation.y+=_uMovementSpeed*frameTime;
	}
	if(_input->KeyDown(DIK_F))
	{
		translation.y-=_dMovementSpeed*frameTime;
	}

	camera->Translate(translation);
#pragma endregion

#pragma region Rotation
	if(_input->KeyDown(DIK_UP))
	{
		camera->Pitch(-_pURotationSpeed*frameTime);
	}
	if(_input->KeyDown(DIK_DOWN))
	{
		camera->Pitch(_pURotationSpeed*frameTime);
	}
	if(_input->KeyDown(DIK_LEFT))
	{
		camera->Yaw(-_yLRotationSpeed*frameTime);
	}
	if(_input->KeyDown(DIK_RIGHT))
	{
		camera->Yaw(_yRRotationSpeed*frameTime);
	}
	if(_input->KeyDown(DIK_Q))
	{
		camera->Roll(-_rLRotationSpeed*frameTime);
	}
	if(_input->KeyDown(DIK_E))
	{
		camera->Roll(_rRRotationSpeed*frameTime);
	}
#pragma endregion
}

FirstPersonCControl::~FirstPersonCControl(void)
{
}
