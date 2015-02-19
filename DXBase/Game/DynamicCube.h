
#ifndef _DYNAMICCUBE_H_
#define _DYNAMICCUBE_H_
#include "DynamicVB.h"
#include "vertex.h"
#include <math.h> 
#include "gamedata.h"

//procedurally generate a VBGO Cube
//each side be divided in to _size * _size squares (2 triangles per square)

class DynamicCube : public DynamicVB
{
public:

	float amp = 10.0f;
	float time = 0;
	float damp;

	DynamicCube(){};
	virtual ~DynamicCube(){
		delete[] vertexOriginalHeight;
		delete[] vertexNewHeight;
	};

	//initialise the Veretx and Index buffers for the cube
	void init(int _size, ID3D11Device* _GD);
	void Draw(DrawData*_DD);
	void Tick(GameData* GD);


protected:
	//this is to allow custom versions of this which create the basic cube and then distort it
	//see Spirla, SpikedVB and Pillow

	virtual void Transform(GameData* GD);
	virtual void TransformSize();
	virtual void Ripple(GameData* GD);
	int getLocation(int i, int j);
	void updateNormals();


	int m_size;
	int m_numVertices;
	int m_centre;
	myVertex* m_vertices;
	
	// arrays for verlet integration
	float* vertexOriginalHeight;
	float* vertexNewHeight;
	float* dummyVertex;
	// vertex positions for original array
	float XprevVertex;
	float XnextVertex;
	float ZprevVertex;
	float ZnextVertex;
	// vertex positions for new array
	float newXprevVertex;
	float newXnextVertex;
	float newZprevVertex;
	float newZnextVertex;
};

#endif