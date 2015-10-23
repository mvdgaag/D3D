#pragma once
#include "BaseDrawable.h"
#include "LinearAlgebra.h"


// predefinitions
class Mesh;
class Material;
class ConstantBuffer;


class DrawableObject : public BaseDrawable
{
public:
	DrawableObject() : BaseDrawable() {}
	~DrawableObject() {};

	void Init(Mesh* inMesh, Material* inMaterial);

	Mesh* GetMesh() { return mMesh; }
	Material* GetMaterial() { return mMaterial; }
	ConstantBuffer* GetConstantBuffer();

private:
	struct ConstantBufferData
	{
		DirectX::XMMATRIX modelView;
		DirectX::XMMATRIX modelViewProjection;
		DirectX::XMMATRIX prevModelViewProjection;
	};
	ConstantBufferData mConstantBufferData;
	
	Mesh* mMesh = nullptr;
	Material* mMaterial = nullptr;
	ConstantBuffer* mConstantBuffer = nullptr;
};

