#include "Ray.h"


Ray::Ray(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	Origin = origin;
	D3DXVec3Normalize(&Direction, &direction);
}

bool Ray::IntersectsMesh(Model3D& model, D3DXVECTOR3& intersection, float& distance, 
	D3DXVECTOR3& normal, D3DMATERIAL9& material)
{
	BOOL hit;
	DWORD faceIndex;
	float dist;

	bool successfull = false;
	DWORD finalFaceIndex;
	distance = std::numeric_limits<float>::infinity();



	//Transform the ray into object space
	D3DXVECTOR3 origin, direction;
	D3DXMATRIX inverseWorld;
	D3DXMatrixInverse(&inverseWorld, 0, &model.WorldMatrix);
	D3DXVec3TransformCoord(&origin, &Origin, &inverseWorld);
	D3DXVec3TransformNormal(&direction, &Direction, &inverseWorld);
	D3DXVec3Normalize(&direction, &direction);

	//Check for ray intersection with all subsets in the mesh
	for(UINT i = 0; i < model.NumMaterials; ++i)
	{
		D3DXIntersectSubset(model.Mesh, i, &origin, &direction, &hit, &faceIndex, 
			NULL, NULL, &dist, NULL, NULL);
		//If intersection occurs and is closer than previously detected intersections
		if(hit && dist<distance)
		{
			successfull = true;
			intersection = Origin + (dist*Direction);
			distance = dist;
			finalFaceIndex = faceIndex;
			material = model.Materials[i];
		}
	}

	//If intersection was successful, calculate the normal at the intersection point
	if(successfull)
	{
		Vertex* vertex;
		WORD* index;

		model.Mesh->LockIndexBuffer(D3DLOCK_READONLY,(VOID**)&index);
		model.Mesh->LockVertexBuffer(D3DLOCK_READONLY,(VOID**)&vertex);
		
		auto vert1 = vertex[index[3*finalFaceIndex]].position;
		auto vert2 = vertex[index[3*finalFaceIndex+1]].position;
		auto vert3 = vertex[index[3*finalFaceIndex+2]].position;

		//Calculate face normal
		auto u = vert2 - vert1;
		auto v = vert3 - vert1;
		D3DXVec3Cross(&normal,&u,&v);
		D3DXVec3TransformNormal(&normal, &normal, &model.WorldMatrix);
		D3DXVec3Normalize(&normal, &normal);
	}

	return successfull;
}

//Redundant, replaced by built in Directx Mesh tracing function
bool Ray::IntersectsTriangle(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2,
	const D3DXVECTOR3& p3, D3DXVECTOR3& normal, D3DXVECTOR3& outIntersection)
{
	//Calculate point of intersection with the plane of the polygon
	D3DXVECTOR3 n, s;
	float mu =0;
	D3DXVec3Normalize(&n,&normal);

	s = p1-Origin;
	mu = D3DXVec3Dot(&n, &s)/D3DXVec3Dot(&Direction, &n);
	
	outIntersection = Origin + (mu*Direction);

	//If the intersection does not occur "behind" the ray,
	//check if point is inside the polygon
	if(mu > 0)
	{
		D3DXVECTOR3 w, u, v;
		float alpha, beta;
		w = outIntersection - p1;
		u = p2 - p1;
		v = p3 - p1;

		float w_u, w_v, u_u, u_v, v_v;
		w_u = D3DXVec3Dot(&w, &u);
		w_v = D3DXVec3Dot(&w, &v);
		u_u = D3DXVec3Dot(&u, &u);
		u_v = D3DXVec3Dot(&u, &v);
		v_v = D3DXVec3Dot(&v, &v);

		alpha = (w_u*v_v-w_v*u_v)/(u_u*v_v-u_v*u_v);
		beta = (w_v*u_u-w_u*u_v)/(u_u*v_v-u_v*u_v);

		if(alpha>=0 && beta>=0 && alpha+beta<=1)
			return true;
	}
	return false;
}

Ray::~Ray(void)
{
}
