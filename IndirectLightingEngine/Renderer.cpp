#include "Renderer.h"

IDirect3DVertexDeclaration9* Vertex::Decl = NULL;

Renderer::Renderer()
{}

void Renderer::SetUp(HWND hwnd)
{
#pragma region Create Direct3D objects
	_d3d = NULL;
	_d3dDevice = NULL;
	
	_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	
	ZeroMemory(&_d3dpp, sizeof(_d3dpp));

	_d3dpp.Windowed = TRUE;
	_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	_d3dpp.hDeviceWindow = hwnd;
	_d3dpp.BackBufferFormat=D3DFMT_UNKNOWN;
	_d3dpp.EnableAutoDepthStencil = TRUE;
	_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &_d3dpp, &_d3dDevice);
#pragma endregion

#pragma region Load shaders
	D3DXCreateEffectFromFile(_d3dDevice, "Radiosity.fx", NULL,NULL,NULL,NULL, &_effect, NULL);

	//Get Effect Handles
	_eProps.MapTech = _effect->GetTechniqueByName("GenerateShadowMap");
	_eProps.GeometryTech = _effect->GetTechniqueByName("RenderGeometry");
	_eProps.DiffuseTech = _effect->GetTechniqueByName("RenderLight");
	_eProps.ShadowTech = _effect->GetTechniqueByName("RenderShadowedLight");

	_eProps.WVP = _effect->GetParameterByName(NULL, "xWVP");
	_eProps.WM = _effect->GetParameterByName(NULL, "xWM");
	_eProps.WITranspose = _effect->GetParameterByName(NULL, "xWorldInverseTransform");
	_eProps.Material = _effect->GetParameterByName(NULL, "xDiffMaterial");
	_eProps.LightColour = _effect->GetParameterByName(NULL, "xLightDiffColour");
	_eProps.LightPosition = _effect->GetParameterByName(NULL, "xLightPosition");
	_eProps.LightDirection = _effect->GetParameterByName(NULL, "xLightDirection");
	_eProps.LightFalloff = _effect->GetParameterByName(NULL, "xLightFalloff");
	_eProps.LightWVP = _effect->GetParameterByName(NULL, "xLightWVP");
	_eProps.ShadowMap = _effect->GetParameterByName(NULL, "xShadowMap");

	//Vertex Declarations
	D3DVERTEXELEMENT9 VertexElements[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};

	_d3dDevice->CreateVertexDeclaration(VertexElements, &Vertex::Decl);
#pragma endregion

#pragma region Lighting and Font
	//Create sphere to represent light positions
	D3DXCreateSphere(_d3dDevice, 0.02f, 5, 7, &_lightMarker, NULL);

	//Set defaults states for renderer
	_directLightEnabled = true;
	_indirectLightEnabled = true;
	_directShadowsEnabled = true;
	_indirectShadowsEnabled = true;
	_lightMarkersEnabled = false;
	_vplMarkersEnabled = false;

	//D3DXFont setup
	D3DXFONT_DESC fontDesc;
	fontDesc.Height = 14;
	fontDesc.Width = 8;
	fontDesc.Weight = FW_NORMAL;
	fontDesc.MipLevels = 0;
	fontDesc.CharSet = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy(fontDesc.FaceName, _T("Times New Roman"));

	D3DXCreateFontIndirect(_d3dDevice, &fontDesc, &_font);
#pragma endregion
}

#pragma region StateModifiers
//Lighting
void Renderer::EnableDirectLighting(bool enable)
{
	_directLightEnabled = enable;
}
void Renderer::EnableIndirectLighting(bool enable)
{
	_indirectLightEnabled = enable;
}

//Shadows
void Renderer::EnableDirectShadows(bool enable)
{
	_directShadowsEnabled = enable;
}
void Renderer::EnableIndirectShadows(bool enable)
{
	_indirectShadowsEnabled = enable;
}

//Markers
void Renderer::EnableLightMarkers(bool enable)
{
	_lightMarkersEnabled = enable;
}
void Renderer::EnableVPLMarkers(bool enable)
{
	_vplMarkersEnabled = enable;
}

void Renderer::AddLight(Light* light)
{
	_lights.push_back(light);
}
void Renderer::SetCamera(Camera& camera)
{
	_sceneCamera = &camera;
}

#pragma endregion

void Renderer::BeginFrame()
{
	_models.clear();
	_strings.clear();
	_stringAreas.clear();
}

void Renderer::Draw(Model3D& model)
{
	Model3D* p = &model;
	_models.push_back(p);
}

void Renderer::DrawText(std::string string, RECT area)
{
	_strings.push_back(string);
	_stringAreas.push_back(area);
}

void Renderer::PostFrame(float frameTime)
{
	//Update Camera
	_sceneCamera->Update(frameTime);
	_sceneCamera->UpdateViewMatrix();
	_vpMatrix = _sceneCamera->GetViewMatrix()*_sceneCamera->GetProjection();

#pragma region Update lights
	for(UINT i = 0; i < _lights.size(); i++)
	{
		//Generate shadow map
		if(_directShadowsEnabled)
		{
			GenerateShadowMap(*_lights[i]);
		}

		//Update vpls
		if(_indirectLightEnabled)
		{
			_lights[i]->CreateVPLs(_models, _d3dDevice);

			//Generate vpl shadow maps
			if(_indirectShadowsEnabled)
			{
				for(UINT j = 0; j < _lights[i]->NumVPLs; ++j)
				{
					GenerateShadowMap(*_lights[i]->VPLs[j]);
				}
			}
		}
	}
#pragma endregion

#pragma region Render Geometry
	//Render the scene geometry in black to prevent overdraw with expensive additive shaders
	_effect->SetVector(_eProps.LightColour, &D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f));

	_d3dDevice->BeginScene();
	_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,40,100), 1.0f,0);
	RenderScene(_eProps.GeometryTech);

#pragma endregion
	
#pragma region Render Lighting
	//Choose the correct technique for main lights and vpls
	D3DXHANDLE lightTech, vplTech;
	lightTech = vplTech = _eProps.DiffuseTech;
	if(_directShadowsEnabled)
	{
		lightTech = _eProps.ShadowTech;
	}
	if(_indirectShadowsEnabled)
	{
		vplTech = _eProps.ShadowTech;
	}

	//Render all lights and their associated vpls
	for(UINT i = 0; i < _lights.size(); ++i)
	{		
		if(_directLightEnabled)
		{
			//Render with main light
			SetLightingParameters(*_lights[i]);
			_effect->CommitChanges();

			if(_lights[i]->CastsShadows())
			{
				RenderScene(lightTech);
			}
			else
			{
				RenderScene(_eProps.DiffuseTech);
			}
		}

		if(_indirectLightEnabled)
		{
			//Render with each vpl belonging to this light
			UINT size = _lights[i]->VPLs.size();
			for(UINT j = 0; j < size; ++j)
			{
				SetLightingParameters(*_lights[i]->VPLs[j]);
				_effect->CommitChanges();
				
				if(_lights[i]->VPLs[j]->CastsShadows())
				{
					RenderScene(vplTech);
				}
				else
				{
					RenderScene(_eProps.DiffuseTech);
				}
			}
		}
	}

#pragma endregion

#pragma region Light Markers
	if(_lightMarkersEnabled)
	{
		_effect->SetVector(_eProps.LightColour, &D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f));
		_effect->SetTechnique(_eProps.GeometryTech);
		UINT passes = 0;
		_effect->Begin(&passes,0);

		for(UINT i = 0; i<passes; ++i)
		{
			_effect->BeginPass(i);
			for(UINT i = 0; i < _lights.size(); ++i)
			{
				Render(*_lights[i]);
			}
			_effect->EndPass();
		}
		_effect->End();
	}
#pragma endregion

#pragma region Text

	for(UINT i = 0; i < _strings.size(); ++i)
	{
		_font->DrawTextA(NULL, _strings[i].c_str(), -1, 
			&_stringAreas[i], DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255,255,255));
	}

#pragma endregion

	_d3dDevice->EndScene();
	_d3dDevice->Present(NULL,NULL,NULL,NULL);
}

void Renderer::RenderScene(D3DXHANDLE technique)
{
	_effect->SetTechnique(technique);

	UINT passes = 0;

	_d3dDevice->SetVertexDeclaration(Vertex::Decl);
	_effect->Begin(&passes,0);

	for(UINT i = 0; i<passes; ++i)
	{
		_effect->BeginPass(i);
		for(unsigned int i =0; i < _models.size(); ++i)
		{
			Render(*_models[i]);
		}
		_effect->EndPass();
	}
	_effect->End();

}

void Renderer::Render(Model3D& model)
{
	_effect->SetMatrix(_eProps.WVP, &(model.WorldMatrix*_vpMatrix));			//World View Projection
	_effect->SetMatrix(_eProps.LightWVP, &(model.WorldMatrix*_lightVPMatrix));	//Light World View Projection
	_effect->SetMatrix(_eProps.WM, &model.WorldMatrix);							//World Matrix

	//Create a world inverse transpose matrix for handling any non uniform scaling
	D3DXMATRIX worldInverseTranspose;
	D3DXMatrixInverse(&worldInverseTranspose, 0, &model.WorldMatrix);
	D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);
	_effect->SetMatrix(_eProps.WITranspose, &worldInverseTranspose);

	for(DWORD i = 0; i<model.NumMaterials; i++)
	{
		_effect->SetVector(_eProps.Material, (D3DXVECTOR4*)&model.Materials[i].Diffuse);
		_effect->CommitChanges();

		model.Mesh->DrawSubset(i);
	}
}

void Renderer::Render(Light& light)
{
	D3DXMATRIX worldMatrix;
	D3DXVECTOR3 lightPos = light.GetPosition();
	D3DXMatrixTranslation(&worldMatrix, lightPos.x,lightPos.y,lightPos.z);
	
	_effect->SetMatrix(_eProps.WVP, &(worldMatrix*_vpMatrix));
	_effect->SetMatrix(_eProps.WM, &worldMatrix);

	D3DXMATRIX worldInverseTranspose;
	D3DXMatrixInverse(&worldInverseTranspose, 0, &worldMatrix);
	D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);
	_effect->SetMatrix(_eProps.WITranspose, &worldInverseTranspose);

	_effect->SetVector(_eProps.Material, &(light.GetDiffuse()*(float)_lights[0]->NumVPLs));
	_effect->CommitChanges();

	_lightMarker->DrawSubset(0);

	if(_vplMarkersEnabled)
	{
		for(UINT i = 0; i < light.VPLs.size(); ++i)
		{
			Render(*light.VPLs[i]);
		}
	}
}

void Renderer::GenerateShadowMap(Light& light)
{	
	if(light.CastsShadows())
	{
		//Set shadow map texture as render target		
		auto texture = light.GetShadowMap();

		LPDIRECT3DSURFACE9 surface = NULL;
		texture->GetSurfaceLevel(0,&surface);

		LPDIRECT3DSURFACE9 backBuffer = NULL;
		_d3dDevice->GetRenderTarget(0, &backBuffer);
		_d3dDevice->SetRenderTarget(0, surface);

		//Store original view projection matrix and replace with the light's matrix
		D3DXMATRIX oVP = _vpMatrix;
		_vpMatrix = light.GetViewProjection();

		//Render Scene
		_d3dDevice->BeginScene();

		_d3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		
		RenderScene(_eProps.MapTech);

		_d3dDevice->EndScene();

		//Restore original matrix
		_vpMatrix = oVP;

		_d3dDevice->SetRenderTarget(0, backBuffer);

	}
}

void Renderer::SetLightingParameters(Light& light)
{
	_lightVPMatrix = light.GetViewProjection();
	_effect->SetVector(_eProps.LightColour, &light.GetDiffuse());
	_effect->SetVector(_eProps.LightPosition, (D3DXVECTOR4*)&light.GetPosition());
	_effect->SetVector(_eProps.LightDirection, (D3DXVECTOR4*)&light.GetDirection());
	_effect->SetFloat(_eProps.LightFalloff, light.GetFalloff());
	_effect->SetTexture(_eProps.ShadowMap, (light.GetShadowMap()));
}

Renderer::~Renderer(void)
{
	_font->Release();
	_lightMarker->Release();
	Vertex::Decl->Release();
	_effect->Release();
	_d3dDevice->Release();
	_d3d->Release();
}
