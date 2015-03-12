#include "Light.h"

float Light::AmbientMultiplier = 1.0f;

Light::Light(void)
{
	_castsShadows = false;
	_shadowMap = NULL;
	NumVPLs = 100;

	D3DXMatrixPerspectiveFovLH(&_projMatrix, D3DX_PI*0.5f, 1.0f, 0.1f, 1000);
}

void Light::EnableShadowing(LPDIRECT3DDEVICE9 device, int shadowMapRes)
{
	//Replace shadow map if size requirements have changed
	if(shadowMapRes != ShadowMapSize)
	{
		ReleaseShadowMap();
	}
	
	if(_shadowMap == NULL)
	{
		device->CreateTexture(shadowMapRes, shadowMapRes, 1, D3DUSAGE_RENDERTARGET,
			D3DFMT_R32F, D3DPOOL_DEFAULT, &_shadowMap, NULL);
	}
	_castsShadows = true;
	ShadowMapSize = shadowMapRes;
}

void Light::CreateVPLs(std::vector<Model3D*>& _models, LPDIRECT3DDEVICE9 device)
{
#pragma region Prepare VPLs
	//Seed the rand() function so that vpls are always cast on the same paths
	srand(142);
	UINT num = VPLs.size();

	//Add or remove VPLs from the list as necessary to match the value in NumVPLs
	if(num < NumVPLs)
	{
		for(UINT i = 0; i < NumVPLs - num; ++i)
		{
			Light* vpl = new Light();
			vpl->EnableShadowing(device);
			vpl->SetFalloff(_falloff);
			VPLs.push_back(vpl);			
		}
	}
	else
	{
		for(UINT i = VPLs.size()-1; i > NumVPLs; --i)
		{
			VPLs.pop_back();	
		}
	}
#pragma endregion

#pragma region Raytrace scene
	//Set virtual point light information for this frame
	for(UINT i = 0; i < NumVPLs; ++i)
	{
		//Trace ray into scene in random direction to determine VPL position
		//Currently limited to hemispherical casts about the y axis and downwards
		Ray ray(_position, D3DXVECTOR3(float(rand()%100-50), float(-rand()%50), float(rand()%100-50)));
		
		D3DXVECTOR3 position, normal;
		float distance, currentDist;
		currentDist = std::numeric_limits<float>::infinity();
		D3DMATERIAL9 material;
		bool successfull = false;

		//Check for ray intersection with all models in the scene
		for(UINT j = 0; j < _models.size(); ++j)
		{
			//Set light's parameters if new closest intersection found
			if(ray.IntersectsMesh(*_models[j], position, distance, normal, material) && distance<currentDist)
			{
				VPLs[i]->SetPosition(position);
				VPLs[i]->SetDirection(normal);
				//Calculate the amount of light from the source light that this vpl represents
				D3DXVECTOR4 diffuse = _diffuse*(2*D3DX_PI/NumVPLs)*AmbientMultiplier;
				diffuse.w = 1;
				D3DXVECTOR4 materialDiffuse (material.Diffuse.r, material.Diffuse.g, material.Diffuse.b, material.Diffuse.a);
				VPLs[i]->SetDiffuse(D3DXVECTOR4(material.Diffuse.r*diffuse.x,material.Diffuse.g*diffuse.y,
					material.Diffuse.b*diffuse.z,material.Diffuse.a*diffuse.w));
				successfull = true;
			}
		}
	}
#pragma endregion
}

void Light::DisableShadowing()
{
	_castsShadows = false;
}

void Light::ReleaseShadowMap()
{
	if(_shadowMap!=NULL)
	{
		_shadowMap->Release();
		_shadowMap = NULL;
	}
}

D3DXMATRIX Light::GetViewProjection()
{
	return _viewMatrix*_projMatrix;
}

Light::~Light(void)
{
	ReleaseShadowMap();
	for(UINT i = 0; i < VPLs.size(); ++i)
	{
		delete VPLs[i];
	}
}

#pragma region Mutators
void Light::SetDiffuse(D3DXVECTOR4 diffuse)
{
	_diffuse = diffuse;
}

void Light::SetPosition(D3DXVECTOR3 position)
{
	_position = position;
	D3DXMatrixTranslation(&_worldMatrix, position.x, position.y, position.z);

	if(D3DXVec3Length(&_direction)!=1)
	{
		D3DXMatrixLookAtLH(&_viewMatrix, &position, &D3DXVECTOR3(0.0f,0.0f,0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	}
	if(_direction.x == 0 && _direction.z ==0)
	{
		D3DXMatrixLookAtLH(&_viewMatrix, &_position, &(_position+_direction), &D3DXVECTOR3(-1.0f,0.0f,0.0f));
	}
	else
		D3DXMatrixLookAtLH(&_viewMatrix, &_position, &(_position+_direction), &D3DXVECTOR3(0.0f,1.0f,0.0f));
}

void Light::SetDirection(D3DXVECTOR3 direction)
{
	D3DXVec3Normalize(&_direction, &direction);
	if(_direction.x == 0 && _direction.z ==0)
	{
		D3DXMatrixLookAtLH(&_viewMatrix, &_position, &(_position+_direction), &D3DXVECTOR3(-1.0f,0.0f,0.0f));
	}
	else
		D3DXMatrixLookAtLH(&_viewMatrix, &_position, &(_position+_direction), &D3DXVECTOR3(0.0f,1.0f,0.0f));
}

void Light::SetFalloff(float falloff)
{
	_falloff = falloff;
}

#pragma endregion