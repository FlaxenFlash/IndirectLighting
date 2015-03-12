#pragma once

#include <d3dx9.h>
#include <d3d9.h>
#include <Dxerr.h>
#include "Camera.h"
#include "FirstPersonCControl.h"
#include "InputManager.h"
#include "Model3D.h"
#include "Renderer.h"
#include "Light.h"

class Engine
{
private:
	Renderer _renderer;

	Model3D _scene;

	Light _light;

	Camera _camera;

	InputManager* _input;

	std::string Engine::GenerateInfoString();
	
public:
	Engine(void);
	
	void initialise(HINSTANCE instance, HWND hwnd);
	void update();
	void render();
	
	~Engine(void);
};

