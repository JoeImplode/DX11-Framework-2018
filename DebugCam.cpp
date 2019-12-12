#include "DebugCam.h"

DebugCam::DebugCam(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, XMFLOAT3 to, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth) : Camera(position,at,up,to,windowWidth,windowHeight,nearDepth,farDepth)
{
	_restrictMouse = false;
	prevXPos = 0;
	prevYPos = 0;
	prevAngX = 0;
	prevAngY = 0;
	tempView = GetViewMtrx();
}

void DebugCam::HandleMouse(int mousePosX, int mousePosY)
{
	int x = prevXPos;
	int y = prevYPos;

	XMFLOAT4X4 temp;
	tempView = GetViewMtrx();
	int dx = (mousePosX - prevXPos);
	int dy = (mousePosY - prevYPos);

	prevAngX = prevAngX - dx;
	prevAngY = prevAngY - dy;

	tempView = GetViewMtrx();

	if (prevAngY < -450)
		prevAngY = -450;
	if (prevAngY > 450)
		prevAngY = 450;

	float finalAngleX = prevAngX / 300;
	float finalAngleY = prevAngY / 300;

	tempView = tempView * XMMatrixRotationY((finalAngleX));
	tempView = tempView * XMMatrixRotationX((finalAngleY));
	XMStoreFloat4x4(&temp, tempView);

	UpdateView(tempView);

	if (GetAsyncKeyState(0x57))
	{
		XMFLOAT3 pos = GetEye();
		SetPosition(XMFLOAT3(pos.x + temp._13 * velocity, pos.y + temp._23 * velocity, (pos.z + temp._33 * velocity)));
	}
	if (GetAsyncKeyState(0x53))
	{
		XMFLOAT3 pos = GetEye();
		SetPosition(XMFLOAT3(pos.x - temp._13 * velocity, pos.y - temp._23 * velocity, pos.z - temp._33 * velocity));
	}

	if (GetAsyncKeyState(0x41))
	{
		XMFLOAT3 pos = GetEye();
		SetPosition(XMFLOAT3(pos.x - temp._11 * velocity, pos.y - temp._21 * velocity, pos.z - temp._31 * velocity));
	}

	if (GetAsyncKeyState(0x44))
	{
		XMFLOAT3 pos = GetEye();
		SetPosition(XMFLOAT3(pos.x + temp._11 * velocity, pos.y + temp._21 * velocity, pos.z + temp._31 * velocity));
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		XMFLOAT3 pos = GetEye();
		SetPosition(XMFLOAT3(pos.x + temp._12 * velocity, pos.y + temp._22 * velocity, pos.z + temp._32 * velocity));
	}
	if (GetAsyncKeyState(VK_LSHIFT))
	{
		XMFLOAT3 pos = GetEye();
		SetPosition(XMFLOAT3(pos.x - temp._12 * velocity, pos.y - temp._22 * velocity, pos.z - temp._32 * velocity));
	}

	prevXPos = mousePosX;
	prevYPos = mousePosY;

	_eyePos = GetEye();

}

