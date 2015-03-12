#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <vector>
#include "Model3D.h"
#include "Ray.h"

class Light
{
private:

	//Data Members
	D3DXVECTOR4 _diffuse;
	D3DXVECTOR3 _position;
	D3DXVECTOR3 _direction;
	float _falloff;

	D3DXMATRIX _viewMatrix;
	D3DXMATRIX _projMatrix;
	D3DXMATRIX _worldMatrix;

	LPDIRECT3DTEXTURE9 _shadowMap;
	
	bool _castsShadows;

public:
	static float AmbientMultiplier;
	
	//Data Members
	UINT ShadowMapSize;
	UINT NumVPLs;
	std::vector<Light*> VPLs;

	//Accessors
	D3DXVECTOR4 GetDiffuse(){return _diffuse;}
	D3DXVECTOR3 GetPosition(){return _position;}
	D3DXVECTOR3 GetDirection(){return _direction;}
	float GetFalloff(){return _falloff;}

	D3DXMATRIX GetViewMatrix(){return _viewMatrix;}
	D3DXMATRIX GetProjMatrix(){return _projMatrix;}
	D3DXMATRIX GetWorldMatrix(){return _worldMatrix;}
	D3DXMATRIX GetViewProjection();

	bool CastsShadows(){return _castsShadows;}
	LPDIRECT3DTEXTURE9 GetShadowMap(){return _shadowMap;}

	//Mutators
	void SetDiffuse(D3DXVECTOR4 diffuse);
	void SetPosition(D3DXVECTOR3 position);
	void SetDirection(D3DXVECTOR3 direction);
	void SetFalloff(float falloff);

	void SetProjMatrix(D3DXMATRIX projection);

	//Methods
	Light(void);
	~Light(void);

	void EnableShadowing(LPDIRECT3DDEVICE9 device, 
		int shadowMapRes = 256);
	void CreateVPLs(std::vector<Model3D*>& _models, LPDIRECT3DDEVICE9 device);
	void DisableShadowing();
	void ReleaseShadowMap();
};

