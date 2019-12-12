#pragma once
#include "Camera.h"
#include "MouseEvent.h"
#include <string>
using namespace std;

class DebugCam : public Camera
{
public:
	DebugCam(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, XMFLOAT3 to, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	void HandleMouse(int x, int y);
	bool			_restrictMouse;

private:
	int				prevXPos;
	int				prevYPos;
	float			prevAngX;
	float			prevAngY;

	const float			velocity = 0.01;

	XMMATRIX		tempView;
	XMFLOAT3		_eyePos = XMFLOAT3(0.0f, 6.0f, -15.0f);



protected:
};

