#include "Camera.h"



Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, XMFLOAT3 to, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_eye = position;
	_at = at;
	_up = up;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
	_to = to;
}

Camera::~Camera()
{
}

void Camera::Update()
{

	//firstly we need to change our floats to vectors
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMVECTOR at = XMLoadFloat3(&_at);
	XMVECTOR up = XMLoadFloat3(&_up);
	XMVECTOR to = XMLoadFloat3(&_to);
	//Afterwards we store the floats in some 4x4 matrices
	if (atSelection == true)
	{
		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(eye, at, up));
	}
	else if (atSelection == false)
	{
		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(eye, to, up));
	}
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / (FLOAT)_windowHeight, _nearDepth, _farDepth));
}

void Camera::SetPosition(XMFLOAT3 eye)
{
	_eye = eye;
}

void Camera::SetLookat(XMFLOAT3 at)
{
	_at = at;
}

void Camera::SetUp(XMFLOAT3 up)
{
	_up = up;
}

void Camera::SetTo(XMFLOAT3 to)
{
	_to = to;
}

XMFLOAT3 Camera::GetAt()
{
	return _at;
}

XMFLOAT3 Camera::GetTo()
{
	return _to;
}

XMFLOAT3 Camera::GetEye()
{
	return _eye;
}

XMMATRIX Camera::GetViewMtrx()
{
	XMMATRIX view = XMLoadFloat4x4(&_view);
	return view;
}

XMMATRIX Camera::GetProjectionMtrx()
{
	XMMATRIX projection = XMLoadFloat4x4(&_projection);
	return projection;
}

XMMATRIX Camera::GetCombinedMatrix()
{
	XMMATRIX projection = XMLoadFloat4x4(&_projection);
	XMMATRIX view = XMLoadFloat4x4(&_view);
	return projection * view;
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}
