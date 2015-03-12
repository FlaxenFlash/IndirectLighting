#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <Dxerr.h>
#include <string>
#include "DebugController.h"

class Vertex
{
public:
	Vertex():position(0.0f,0.0f,0.0f),
		normal(0.0f,0.0f,0.0f) {}
	Vertex(float x, float y, float z, 
		float nx, float ny, float nz)
		:position(x,y,z), normal(nx,ny,nz) {}
	Vertex(const D3DXVECTOR3 &v, const D3DXVECTOR3 &n)
		:position(v), normal(n) {}

	D3DXVECTOR3 position;
	D3DXVECTOR3	normal;

	static IDirect3DVertexDeclaration9* Decl;
};

class Model3D
{
public:
	//Data Members
	LPD3DXMESH Mesh;
	LPD3DXBUFFER D3DXMtrlBuffer;
	DWORD NumMaterials;
	D3DMATERIAL9* Materials;
	LPDIRECT3DTEXTURE9* Textures;
	D3DXMATRIX WorldMatrix;
	D3DXMATRIX RotationMatrix;
	

	//Methods
	Model3D(void);

	void Load(LPDIRECT3DDEVICE9 d3ddev, LPCSTR path, D3DXMATRIX position);

	~Model3D(void);
};

