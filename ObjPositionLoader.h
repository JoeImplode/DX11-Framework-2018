#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <iostream>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class ObjPositionLoader
{
private:
	ifstream inFile;
	float x;
	float y;
	float z;
public:
	void OpenFile(char* fileName);
	XMFLOAT3 ReadFile();
	void CloseFile();

};