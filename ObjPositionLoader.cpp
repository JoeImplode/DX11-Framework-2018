#include "ObjPositionLoader.h"

XMFLOAT3 ObjPositionLoader::ReadFile()
{
	inFile >> x;
	inFile >> y;
	inFile >> z;
	return XMFLOAT3(x, y, z);
}

void ObjPositionLoader::OpenFile(char* fileName)
{
	inFile.open(fileName, ios::in);
}

void ObjPositionLoader::CloseFile()
{
	inFile.close();
}