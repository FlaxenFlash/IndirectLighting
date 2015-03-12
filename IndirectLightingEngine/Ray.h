#pragma once
#include <vector>
#include <d3dx9.h>
#include <d3d9.h>
#include "Model3D.h"

class Ray
{
public:
	D3DXVECTOR3 Origin;
	D3DXVECTOR3 Direction;

	Ray(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	~Ray(void);

	bool IntersectsMesh(Model3D& model, D3DXVECTOR3& intersection, 
		float& dist, D3DXVECTOR3& normal, D3DMATERIAL9& material);

	bool IntersectsTriangle(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2,
	const D3DXVECTOR3& p3, D3DXVECTOR3& normal, D3DXVECTOR3& intersection);
};

