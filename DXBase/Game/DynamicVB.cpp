#include "DynamicVB.h"
#include <Windows.h>
#include <d3dcompiler.h>
#include "DDSTextureLoader.h"
#include "drawdata.h"
#include "vertex.h"
#include "camera.h"
#include "light.h"
#include "VBGO.h"



void DynamicVB::Tick(GameData* _GD)
{
	VBGO::Tick(_GD);
}

void DynamicVB::Draw(DrawData* _DD)
{
	//Set vertex buffer
	UINT stride = sizeof(myVertex);
	UINT offset = 0;
	_DD->pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_dynamicVertexBuffer, &stride, &offset);
	
	VBGO::Draw(_DD);
}

void DynamicVB::BuildIB(ID3D11Device* _GD, void* _indices)
{
	//structures from creating buffers
	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	HRESULT hr = S_OK;

	//build index buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 3 * m_numPrims;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = _indices;
	hr = _GD->CreateBuffer(&bd, &InitData, &m_IndexBuffer);
}

void DynamicVB::BuildVB(ID3D11Device* _GD, int _numVerts, void* _vertices)
{
	//structures from creating buffers
	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	HRESULT hr = S_OK;

	//build vertex buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(myVertex) * _numVerts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = _vertices;
	hr = _GD->CreateBuffer(&bd, &InitData, &m_dynamicVertexBuffer);
}

void Render(DrawData*_DD)
{

}