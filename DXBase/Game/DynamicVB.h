#ifndef _DYNAMICVB_H_
#define _DYNAMICVB_H_
#include "gameobject.h"
#include "VBGO.h"


class DynamicVB :public VBGO
{
public:
	DynamicVB()
	
	{
		m_dynamicVertexBuffer = NULL;
	};
	virtual ~DynamicVB(){};

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD);

protected:

	void Render();

	ID3D11Buffer* m_dynamicVertexBuffer;

	//once populated build an Index Buffer
	void BuildIB(ID3D11Device* _GD, void* _indices);

	//once populated build a Vertex Buffer
	void BuildVB(ID3D11Device* _GD, int _numVerts, void* _vertices);

};

#endif