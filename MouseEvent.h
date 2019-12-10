#pragma once
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>
class MouseEvent
{
public:
	MouseEvent();
	~MouseEvent();
	bool ReadMouse();
	void ProcessInput();
	void GetMousePosition(int& mouseX, int& mouseY);
	void RestrictMouse(HWND _hWnd);
	void CreateMouseDevice(HWND _hWnd, HINSTANCE _hInst);
	void ReleaseMouse();
private:
	int _mouseX, _mouseY;
	IDirectInput8* _directInput;
	IDirectInputDevice8* _mouse;
	DIMOUSESTATE	_mouseState;

protected:
};

