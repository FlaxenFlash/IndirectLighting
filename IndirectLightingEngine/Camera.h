#pragma once
#include <windows.h>
#include <windowsX.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <Dxerr.h>

class CameraControl;

class Camera
{
private:
	//Camera location and orientation
	D3DXVECTOR3 _position;
	float _yaw, _pitch, _roll;
	D3DXVECTOR3 _up,_right,_look;

	//Camera perspective
	D3DXMATRIX _projection;
	D3DXMATRIX _viewMatrix;

	//Camera controller
	CameraControl* _controller;

public:
	Camera();

	void SetController(CameraControl* control);

	void Update(float frameTime);
	void UpdateViewMatrix();
	D3DXMATRIX GetViewMatrix();

	void Translate(float x, float y, float z);
	void Translate(D3DXVECTOR3 translation);
	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 translation);

	void Yaw(float increment);
	void Pitch(float increment);
	void Roll(float increment);

	void SetProjection(D3DXMATRIX projection);
	D3DXMATRIX GetProjection();

	~Camera(void);
};

class CameraControl
{
public:
	CameraControl(void){};

	virtual void Update(Camera* camera, float frameTime)=0;

	~CameraControl(void){};
};

