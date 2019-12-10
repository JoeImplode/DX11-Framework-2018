#include "MouseEvent.h"
MouseEvent::MouseEvent()
{
	_mouseX = 0;
	_mouseY = 0;
	
}

void MouseEvent::CreateMouseDevice(HWND _hWnd, HINSTANCE _hInst)
{
	HRESULT hr;
	hr = DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, NULL);

	//Create the mouse device in our direct input object
	hr = _directInput->CreateDevice(GUID_SysMouse, &_mouse, NULL);

	//Set the data format for the mouse using pre defined mouse data format
	hr = _mouse->SetDataFormat(&c_dfDIMouse);

	//Set where the mouse can be used which is within our window
	hr = _mouse->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = _mouse->Acquire();
}

bool MouseEvent::ReadMouse()
{
	HRESULT result;

	//Firstly we read the mouse device
	result = _mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&_mouseState);
	if (FAILED(result))
	{
		//if the mouse lost focus, or was not acquired, then try to get the control back
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			//We then need to re aqcuire the mouse
			_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}
	return true;
}

void MouseEvent::ProcessInput()
{
	//update our found mouse position to the values aqcuired
	_mouseX += _mouseState.lX;
	_mouseY += _mouseState.lY;

	return;
}

void MouseEvent::GetMousePosition(int& mouseX, int& mouseY)
{
	mouseX = _mouseX;
	mouseY = _mouseY;
	return;
}

void MouseEvent::RestrictMouse(HWND _hWnd)
{
	//Firstly create a new rect which will store our bounding area
	RECT rect;
	GetClientRect(_hWnd, &rect);

	POINT ul;
	ul.x = rect.left;
	ul.y = rect.top;

	POINT lr;
	lr.x = rect.right;
	lr.y = rect.bottom;
	MapWindowPoints(_hWnd, nullptr, &ul, 1);
	MapWindowPoints(_hWnd, nullptr, &lr, 1);

	rect.left = ul.x;
	rect.top = ul.y;

	rect.right = lr.x;
	rect.bottom = lr.y;
	ShowCursor(false);
	ClipCursor(&rect);
}

MouseEvent::~MouseEvent()
{
	_mouse->Unacquire();
	_mouse->Release();
	_mouse = 0;

	_directInput->Release();
	_directInput = 0;
}

void MouseEvent::ReleaseMouse()
{
	ShowCursor(true);
	ClipCursor(false);
}