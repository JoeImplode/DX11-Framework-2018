#pragma once

#include "Primitives.h"
#include "Structures.h"

class Cube : public Primitives
{
public:
	Cube();
	~Cube();
	SimpleVertex _cubeVertices;
	WORD _cubeIndices;
private:
protected:
};
