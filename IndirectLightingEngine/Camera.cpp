#include "Camera.h"


Camera::Camera()
{
	_controller = NULL;

	_up = D3DXVECTOR3(0.0f,1.0f,0.0f);	
	_right = D3DXVECTOR3(1.0f,0.0f,0.0f);
	_look = D3DXVECTOR3(0.0f,0.0f,1.0f);
	_position.x = 0;
	_position.y = 0;
	_position.z = 0;
	_yaw = 0;
	_pitch = 0;
	_roll = 0;
}

void Camera::SetController(CameraControl* control)
{
	_controller = control;
}

void Camera::Update(float frameTime)
{
	if(_controller != NULL)
		_controller->Update(this, frameTime);
}

void Camera::UpdateViewMatrix()
{
	//Prevent degrdation of axis due to floating point error
	D3DXVec3Normalize(&_up,&_up);
	D3DXVec3Normalize(&_right,&_right);
	D3DXVec3Normalize(&_look,&_look);

	D3DXMatrixIdentity(&_viewMatrix);

	//Rebuild matrix
	_viewMatrix._11 = _right.x; _viewMatrix._12 = _up.x; _viewMatrix._13 = _look.x;
	_viewMatrix._21 = _right.y; _viewMatrix._22 = _up.y; _viewMatrix._23 = _look.y;
	_viewMatrix._31 = _right.z; _viewMatrix._32 = _up.z; _viewMatrix._33 = _look.z;

	_viewMatrix._41 = - D3DXVec3Dot( &_position, &_right ); 
	_viewMatrix._42 = - D3DXVec3Dot( &_position, &_up );
	_viewMatrix._43 = - D3DXVec3Dot( &_position, &_look );
}

D3DXMATRIX Camera::GetViewMatrix()
{
	return _viewMatrix;
}

void Camera::Translate(float x, float y, float z)
{
	D3DXVECTOR3 gx, gy, gz;

	gx = _right*x;
	gy = _up*y;
	gz = _look*z;

	_position+=gx+gy+gz;
}

void Camera::Translate(D3DXVECTOR3 translation)
{
	D3DXVECTOR3 gx, gy, gz;

	gx = _right*translation.x;
	gy = _up*translation.y;
	gz = _look*translation.z;

	_position+=gx+gy+gz;
}

void Camera::SetPosition(float x, float y, float z)
{
	_position.x = x;
	_position.y = y;
	_position.z = z;
}

void Camera::SetPosition(D3DXVECTOR3 translation)
{
	_position = translation;
}

void Camera::Yaw(float increment)
{
	D3DXMATRIX yaw;
	D3DXMatrixRotationAxis(&yaw,&_up,D3DXToRadian(increment));

	D3DXVec3TransformNormal(&_look, &_look, &yaw);
	D3DXVec3TransformNormal(&_right, &_right, &yaw);
}

void Camera::Pitch(float increment)
{
	D3DXMATRIX pitch;
	D3DXMatrixRotationAxis(&pitch,&_right,D3DXToRadian(increment));

	D3DXVec3TransformNormal(&_look, &_look, &pitch);
	D3DXVec3TransformNormal(&_up, &_up, &pitch);
}

void Camera::Roll(float increment)
{
	D3DXMATRIX roll;
	D3DXMatrixRotationAxis(&roll,&_look,D3DXToRadian(increment));

	D3DXVec3TransformNormal(&_up, &_up, &roll);
	D3DXVec3TransformNormal(&_right, &_right, &roll);
}

void Camera::SetProjection(D3DXMATRIX projection)
{
	_projection = projection;
}

D3DXMATRIX Camera::GetProjection()
{
	return _projection;
}

Camera::~Camera(void)
{
	delete _controller;
}
