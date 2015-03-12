#pragma once
#include <vector>
#include <d3dx9.h>
#include <d3d9.h>
#include <Dxerr.h>
#include <tchar.h>
#include "Camera.h"
#include "FirstPersonCControl.h"
#include "InputManager.h"
#include "Model3D.h"
#include "Light.h"


struct EffectProperties
{
	D3DXHANDLE MapTech;
	D3DXHANDLE GeometryTech;
	D3DXHANDLE DiffuseTech;
	D3DXHANDLE ShadowTech;
	D3DXHANDLE WVP;
	D3DXHANDLE WM;
	D3DXHANDLE WITranspose;
	D3DXHANDLE Material;
	D3DXHANDLE LightColour;
	D3DXHANDLE LightPosition;
	D3DXHANDLE LightDirection;
	D3DXHANDLE LightFalloff;
	D3DXHANDLE LightWVP;
	D3DXHANDLE ShadowMap;
};

class Renderer
{
private:
	//DirectX
	LPDIRECT3D9 _d3d;
	LPDIRECT3DDEVICE9 _d3dDevice;
	D3DPRESENT_PARAMETERS _d3dpp;

	//Lighting
	std::vector<Light*> _lights;
	LPD3DXMESH _lightMarker;
	bool _directLightEnabled;
	bool _indirectLightEnabled;
	bool _directShadowsEnabled;
	bool _indirectShadowsEnabled;
	bool _lightMarkersEnabled;
	bool _vplMarkersEnabled;
	
	//Viewpoint
	Camera* _sceneCamera;
	D3DXMATRIX _vpMatrix;
	D3DXMATRIX _lightVPMatrix;
	D3DXVECTOR3 _cameraPos;

	//Geometry
	std::vector<Model3D*> _models;

	//Text
	LPD3DXFONT _font;
	std::vector<std::string> _strings;
	std::vector<RECT> _stringAreas;

	//Shaders
	LPD3DXEFFECT _effect;
	EffectProperties _eProps;

	//Methods
	void Render(Model3D& model);
	void Render(Light& light);
	void GenerateShadowMap(Light& light);
	void RenderScene(D3DXHANDLE technique);
	void SetLightingParameters(Light& light);
	
public:
	Renderer(void);

	void SetUp(HWND hwnd);

	//Accessors
	LPDIRECT3DDEVICE9 GetD3DDevice(){ return _d3dDevice; }
	D3DPRESENT_PARAMETERS GetD3DParameters(){ return _d3dpp; }
	
	bool DirectLightingEnabled(){ return _directLightEnabled; }
	bool IndirectLightingEnabled(){ return _indirectLightEnabled; }

	bool DirectShadowsEnabled(){ return _directShadowsEnabled; }
	bool IndirectShadowsEnabled(){ return _indirectShadowsEnabled; }

	bool LightMarkersEnabled(){ return _lightMarkersEnabled; }
	bool VPLMarkersEnabled(){ return _vplMarkersEnabled; }

	//State Modifiers
	void EnableDirectLighting(bool enable);
	void EnableIndirectLighting(bool enable);

	void EnableDirectShadows(bool enable);
	void EnableIndirectShadows(bool enable);

	void EnableLightMarkers(bool enable);
	void EnableVPLMarkers(bool enable);

	void SetCamera(Camera& camera);
	void AddLight(Light* light);

	//Drawing interfaces
	void BeginFrame();	
	void Draw(Model3D& model);
	void DrawText(std::string string, RECT area);
	void PostFrame(float frameTime);

	~Renderer(void);	
};

