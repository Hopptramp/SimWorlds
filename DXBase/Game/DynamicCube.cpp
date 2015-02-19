#include "DynamicCube.h"
#include "drawdata.h"
#include "gamedata.h"
#include <list>


void DynamicCube::init(int _size, ID3D11Device* GD)
{
	m_size = _size;

	//calculate number of vertices and primitives
	int numVerts = 6 * (m_size -1) * (m_size -1);
	m_numVertices = numVerts;
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = 0; i < (m_size - 1); i++)
	{
		for (int j = 0; j < (m_size - 1); j++)
		{
			//top
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)j);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)(j + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.0f, (float)j);

			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.0f, (float)j);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)(j + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.0f, (float)(j + 1));
				
		}
	}


	//carry out some kind of transform on these vertices to make this object more interesting	
	//TransformSize();
		
	//calculate the normals for the basic lighting in the base shader
	for (int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}

	BuildIB(GD, indices);
	BuildVB(GD, numVerts, m_vertices);

	vertexOriginalHeight = new float[m_size *m_size];
	vertexNewHeight = new float[m_size *m_size];

	memset(vertexOriginalHeight, 1, sizeof(float)*m_size *m_size);
	memset(vertexNewHeight, 0, sizeof(float)*m_size *m_size);

	vertexOriginalHeight[getLocation(m_size / 2, m_size / 2)] = 10;
}

void DynamicCube::Tick(GameData* GD)
{
	time = time + (GD->dt);
	//Transform(GD);  // creates ripple with sin wave function
	Ripple(GD);	 // creates ripple using verlet integration
	updateNormals(); // updates normals after each transformation.
	DynamicVB::Tick(GD);
}

int DynamicCube::getLocation(int i, int j)
{
	//but with wrapping around stuff!
	if (i == -1)
		i = m_size - 1;
	if (i == m_size - 1)
		i = 0;
	
	if (j == -1)
		j = m_size - 1;
	if (j == m_size - 1)
		j = 0;


	return i * m_size + j;
}

void DynamicCube::Ripple(GameData* GD)
{
	//position of hit
	//if distance away from hit < radius push down

	/*for (int i = 0; i < m_size; i++)
	{
		for (int j = 0; j < m_size; j++)
		{
			float pos = i * m_size + j;
		}
	}*/


	for (int i = 0; i < (m_size); i++)
	{
		for (int j = 0; j < (m_size); j++)
		{
			damp = -0.5 * GD->dt;
			
			// making sure only vertices that are mapped in the array are used
			ZprevVertex = vertexOriginalHeight[getLocation(i, j - 1)]; // working out the vertex above
			ZnextVertex = vertexOriginalHeight[getLocation(i, j + 1)]; // working out the vertex below

			XprevVertex = vertexOriginalHeight[getLocation(i - 1, j)]; // working out the vertex to the left
			XnextVertex = vertexOriginalHeight[getLocation(i + 1, j)]; // working out the vertex to the right

			// applying verlet integration to each X.axis adjacent vertices
			newXnextVertex = (2 * vertexOriginalHeight[getLocation(i, j)]) - XprevVertex + (damp * time * time * vertexOriginalHeight[getLocation(i, j)]);
			newXprevVertex = (2 * vertexOriginalHeight[getLocation(i, j)]) - XnextVertex + (damp * time * time * vertexOriginalHeight[getLocation(i, j)]);

			// applying verlet integration to each Z.axis adjacent vertices
			newZnextVertex = (2 * vertexOriginalHeight[getLocation(i, j)]) - ZprevVertex + (damp * time * time * vertexOriginalHeight[getLocation(i, j)]);
			newZprevVertex = (2 * vertexOriginalHeight[getLocation(i, j)]) - ZnextVertex + (damp * time * time * vertexOriginalHeight[getLocation(i, j)]);

			vertexNewHeight[getLocation(i, j - 1)] = newZprevVertex; // applying the new vertex positions to the newArray
			vertexNewHeight[getLocation(i, j + 1)] = newZnextVertex; //

			vertexNewHeight[getLocation(i - 1, j)] = newXprevVertex; 
			vertexNewHeight[getLocation(i + 1, j)] = newXnextVertex; 
			//vertexNewHeight[getLocation(i, j)] = damp * vertexOriginalHeight[getLocation(i, j)];
		}
	}

	for (int i = 0; i < m_numVertices; i++)
	{
		m_vertices[i].Pos.y = vertexNewHeight[getLocation((int)(m_vertices[i].Pos.x), (int)(m_vertices[i].Pos.z))];
	}
	
	dummyVertex = vertexOriginalHeight;
	vertexOriginalHeight = vertexNewHeight;
	vertexNewHeight = dummyVertex;
}

void DynamicCube::Draw(DrawData* _DD)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));


	//Disable GPU access to the vertex buffer data.
	_DD->pd3dImmediateContext->Map(m_dynamicVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	myVertex* p_vertices = (myVertex*)mappedResource.pData;

	//Update the vertex buffer here.
	memcpy(p_vertices, (void*)m_vertices, sizeof(myVertex) * m_numVertices);

	//Reenable GPU access to the vertex buffer data.
	_DD->pd3dImmediateContext->Unmap(m_dynamicVertexBuffer, 1);

	DynamicVB::Draw(_DD);
}


void DynamicCube::Transform(GameData* gd)
{
	
	for (int i = 0; i < m_numVertices; i++)
		{
			float newPos = amp * sin(time + m_vertices[i].Pos.x)* sin(time + m_vertices[i].Pos.z);

			m_vertices[i].Pos.y = newPos;

		}
};

void DynamicCube::updateNormals()
{
	for (int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}
}

void DynamicCube::TransformSize()
{
	for (int i = 0; i < m_numVertices; i++)
	{
		Vector3 vertPos = m_vertices[i].Pos;

		Matrix scaleMat = Matrix::CreateScale(5.0f, 1.0f, 5.0f);

		Matrix transMat = Matrix::CreateTranslation(Vector3(-200.0f, 0.0f, -250.0f));

		Vector3 newScale = Vector3::Transform(vertPos, scaleMat);
		Vector3 newPos = Vector3::Transform(newScale, transMat);
		m_vertices[i].Pos = newPos;
	}
}