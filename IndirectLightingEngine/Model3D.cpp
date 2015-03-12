#include "Model3D.h"


Model3D::Model3D(void)
{
	Materials = NULL;
	Textures = NULL;
}

void Model3D::Load(LPDIRECT3DDEVICE9 d3ddev, LPCSTR path, D3DXMATRIX position)
{
	//Load Mesh
	LPD3DXMESH mesh;
	D3DXLoadMeshFromX(path,D3DXMESH_SYSTEMMEM,d3ddev,NULL,&D3DXMtrlBuffer,NULL,&NumMaterials,&mesh);
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)D3DXMtrlBuffer->GetBufferPointer();
	Materials = new D3DMATERIAL9[NumMaterials];
	Textures = new LPDIRECT3DTEXTURE9[NumMaterials];

	//Get folder path
	std::string folderPath = path;
	unsigned pos = folderPath.find_last_of("/\\");
	folderPath = folderPath.substr(0,++pos);

	//Load materials and textures
	for(DWORD i = 0; i<NumMaterials; i++)
	{
		Materials[i] = d3dxMaterials[i].MatD3D;
		Materials[i].Ambient = Materials[i].Diffuse;

		if(d3dxMaterials[i].pTextureFilename)
		{
			std::string texturepath = d3dxMaterials[i].pTextureFilename;
			texturepath = folderPath+texturepath;

			HRESULT hr = D3DXCreateTextureFromFile(d3ddev, texturepath.c_str(), &Textures[i]);
			bool test = FAILED(hr);
			if(hr)
			{
				Textures[i] = NULL;
			}
		}
		else
		{
			Textures[i] = NULL;
		}

	}
	D3DXMtrlBuffer->Release();

	//Convert mesh to use custom vertex format
	D3DVERTEXELEMENT9 vertexElements[MAX_FVF_DECL_SIZE];
	UINT num = 0;
	Vertex::Decl->GetDeclaration(vertexElements, &num);
	mesh->CloneMesh(D3DXMESH_MANAGED, vertexElements, d3ddev, &Mesh);
	mesh->Release();
	Mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT,NULL,NULL,NULL,NULL);

	//Set drawing position
	WorldMatrix = position;
}

Model3D::~Model3D(void)
{
	delete[] Materials;
	for(DWORD i = 0; i < NumMaterials; i++)
	{
		if(Textures[i])
			Textures[i]->Release();
	}
	delete[] Textures;
	Mesh->Release();
}
