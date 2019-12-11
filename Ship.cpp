#include "Ship.h"

Ship::Ship(char* fileName, ID3D11Device* device, bool invertTextCoords) : Object(fileName, device, invertTextCoords)
{
	velocity = 0.002;
	rotate = 0;
	position = XMFLOAT3(0.0f, 15.0f, 0.0f);
	this->Update(XMMatrixRotationY(3.141) * XMMatrixScaling(7, 7, 7) * XMMatrixTranslation(position.x, position.y, position.z));
}

void Ship::Move()
{
	if (GetAsyncKeyState(VK_SHIFT))
		velocity = 0.007;
	else
		velocity = 0.002;
	XMFLOAT3 currPos = GetPosition();
	tempMatrix = XMLoadFloat4x4(&_objectMatrix);
	if (GetAsyncKeyState(0x57))
		SetPosition(XMFLOAT3(currPos.x - (this->_objectMatrix._33) * velocity, currPos.y - (this->_objectMatrix._23) * velocity, currPos.z - (this->_objectMatrix._13 * velocity)));
	if (GetAsyncKeyState(0x53))
		SetPosition(XMFLOAT3(currPos.x + this->_objectMatrix._33 * velocity, currPos.y + this->_objectMatrix._23 * velocity, currPos.z + this->_objectMatrix._13 * velocity));

	if (GetAsyncKeyState(0x41))
		rotate = rotate - 0.0003;
	if (GetAsyncKeyState(0x44))
		rotate = rotate + 0.0003;
	
	this->Update(XMMatrixRotationY(3.141) * XMMatrixRotationY(rotate) * XMMatrixScaling(7, 7, 7) * XMMatrixTranslation(position.x, position.y, position.z));
}

XMFLOAT3 Ship::GetPosition()
{
	return position;
}

void Ship::SetPosition(XMFLOAT3 newPosition)
{
	position = newPosition;
}
