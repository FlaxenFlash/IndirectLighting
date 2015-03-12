#include "Engine.h"

Engine::Engine(void)
{
}

void Engine::initialise(HINSTANCE instance, HWND hwnd)
{
	//Set up input manager
	_input = new InputManager(instance, hwnd);

	_renderer.SetUp(hwnd);

	auto d3dDevice = _renderer.GetD3DDevice();
	auto d3dProps = _renderer.GetD3DParameters();

	//Set up camera
	float w = (float)d3dProps.BackBufferWidth;
	float h = (float)d3dProps.BackBufferHeight;
	_camera.SetProjection(*D3DXMatrixPerspectiveFovLH(&_camera.GetProjection(), D3DX_PI*0.25f, w/h, 0.1f, 100));
	_camera.SetPosition(3.5f, 1.0f, 0.0f);
	_camera.Yaw(-90.0f);
	CameraControl* cController = new FirstPersonCControl(_input, 2.0f, 30.0f);
	_camera.SetController(cController);
	_renderer.SetCamera(_camera);

	//Set up light
	_light.SetDiffuse(D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f));
	_light.SetPosition(D3DXVECTOR3(0.2f, 1.90f, 0.0f));
	_light.SetDirection(D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	_light.SetFalloff(0.1f);
	_light.EnableShadowing(_renderer.GetD3DDevice(), 512);

	_renderer.AddLight(&_light);

	//Set up models
	D3DXMATRIX transform;
	D3DXMatrixTranslation(&transform,5,-5,0);
	D3DXMatrixRotationY(&transform,D3DXToRadian(-90));
	_scene.Load(_renderer.GetD3DDevice(), "Assets/CornellBox/Cornell.x", transform);
}

void Engine::update()
{
	_input->Update();

	//Toggles
	if(_input->KeyPressed(DIK_1))
	{
		_renderer.EnableDirectLighting(!_renderer.DirectLightingEnabled());
	}
	if(_input->KeyPressed(DIK_2))
	{
		_renderer.EnableDirectShadows(!_renderer.DirectShadowsEnabled());
	}
	if(_input->KeyPressed(DIK_3))
	{
		_renderer.EnableIndirectLighting(!_renderer.IndirectLightingEnabled());
	}	
	if(_input->KeyPressed(DIK_4))
	{
		_renderer.EnableIndirectShadows(!_renderer.IndirectShadowsEnabled());
	}	
	if(_input->KeyPressed(DIK_5))
	{
		_renderer.EnableLightMarkers(!_renderer.LightMarkersEnabled());
	}	
	if(_input->KeyPressed(DIK_6))
	{
		_renderer.EnableVPLMarkers(!_renderer.VPLMarkersEnabled());
	}

	//Light movement
	static float speed = 0.2f;
	D3DXVECTOR3 translation(0,0,0);
	if(_input->KeyDown(DIK_COMMA))
	{
		translation.z-=speed*_input->GetFrameTime();
	}
	if(_input->KeyDown(DIK_PERIOD))
	{
		translation.z+=speed*_input->GetFrameTime();
	}
	if(_input->KeyDown(DIK_K))
	{
		translation.y+=speed*_input->GetFrameTime();
	}
	if(_input->KeyDown(DIK_M))
	{
		translation.y-=speed*_input->GetFrameTime();
	}
	//VPL count adjustment
	if(_input->KeyDown(DIK_EQUALS))
	{
		_light.NumVPLs++;
	}
	if(_input->KeyDown(DIK_MINUS)&&_light.NumVPLs>0)
	{
		_light.NumVPLs--;
	}

	_light.SetPosition(_light.GetPosition() + translation);
}

void Engine::render()
{
	_renderer.BeginFrame();

	_renderer.Draw(_scene);

	RECT area;
	area.top = 10;
	area.left = 10;
	area.right = 300;
	area.bottom = 600;
	
	auto info = GenerateInfoString();

	_renderer.DrawTextA(info, area);

	_renderer.PostFrame(_input->GetFrameTime());
}

std::string Engine::GenerateInfoString()
{
	static char buffer[256];
	sprintf(buffer, "FPS = %.2f\n""Num VPLs = %d\n\n", 
		_input->GetFrameRate(), _light.NumVPLs);
	std::string info(buffer);

	//Toggle messages
	info += "1: Toggle Direct Lighting -";
	if(_renderer.DirectLightingEnabled())
		info += " ON\n";
	else
		info += " OFF\n";
	info += "2: Toggle Direct Shadows -";
	if(_renderer.DirectShadowsEnabled())
		info += " ON\n";
	else
		info += " OFF\n";
	info += "3: Toggle Indirect Lighting -";
	if(_renderer.IndirectLightingEnabled())
		info += " ON\n";
	else
		info += " OFF\n";
	info += "4: Toggle Indirect Shadows -";
	if(_renderer.IndirectShadowsEnabled())
		info += " ON\n";
	else
		info += " OFF\n";
	info += "5: Toggle Light Markers -";
	if(_renderer.LightMarkersEnabled())
		info += " ON\n";
	else
		info += " OFF\n";
	info += "6: Toggle VPL Markers -";
	if(_renderer.VPLMarkersEnabled())
		info += " ON\n";
	else
		info += " OFF\n";

	//Control messages
	info+="\nCamera Controls\n";
	info+="Move Camera - W, A, S& D\n";
	info+="Rotate Camera - Arrow Keys & Q/E\n";
	info+="Move Camera - W, A, S& D\n";
	
	info+="\nLight Controls - < & > / , & .";

	info+="\nAdjust VPL count - + & -";
	return info;
}

Engine::~Engine(void)
{
}
