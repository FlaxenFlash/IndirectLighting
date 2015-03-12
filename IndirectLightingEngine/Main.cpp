#include <windows.h>
#include <windowsX.h>
#include "Engine.h"

#pragma comment (lib, "d3d9.lib")

bool running;

HWND CreateGameWindow(LPCTSTR Title, int XPos, int YPos, int Width, int Height);

void destroy();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpcmdline, int nCmdShow)
{
	MSG message;
	running = true;

	ZeroMemory(&message, sizeof(message));
	
	//Create a new window for the game to be displayed in
	HWND gameWindow = CreateGameWindow("IndirectLightingEngine",100,100,1240,700);
	Engine gameEngine = Engine();

	gameEngine.initialise(hInstance, gameWindow);

	while(running)
	{
		//Check for Windows Messages
		while(PeekMessage(&message,gameWindow,0,0,PM_REMOVE))
        {
             TranslateMessage(&message);
             DispatchMessage(&message);
		}

		gameEngine.update();

		gameEngine.render();
	}

	destroy();

	return 0;
}

void destroy()
{
	
}

LRESULT CALLBACK WndProc(HWND window,UINT message,WPARAM parameter1,LPARAM parameter2)
 {
     switch(message)
    {
        case WM_CLOSE:
			running = false;
        break;
        case WM_DESTROY:
			DestroyWindow(window);
            PostQuitMessage(WM_QUIT);
			running = false;
        break;
        default:
            return DefWindowProc(window, message, parameter1, parameter2);
    }
    return 0;
}

HWND CreateGameWindow(LPCTSTR Title, int XPos, int YPos, int Width, int Height)
{
	WNDCLASSEX windowStructure;

	windowStructure.cbSize = sizeof(WNDCLASSEX);
	windowStructure.style = CS_HREDRAW | CS_VREDRAW;
	windowStructure.lpfnWndProc = WndProc;
	windowStructure.cbClsExtra = 0;
	windowStructure.cbWndExtra = 0;
	windowStructure.hInstance = GetModuleHandle(NULL);
	windowStructure.hIcon = NULL;
	windowStructure.hCursor = NULL;
	windowStructure.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowStructure.lpszMenuName = NULL;
	windowStructure.lpszClassName = "IndirectLightingEngine";
	windowStructure.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	RegisterClassEx(&windowStructure);

	return CreateWindowEx(WS_EX_CONTROLPARENT, "IndirectLightingEngine", Title, 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, 
		XPos, YPos, Width, Height, NULL, NULL, GetModuleHandle(NULL), NULL);

}