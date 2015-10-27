#pragma once
#include "BaseDrawable.h"
#include "GAAGCommon.h"

// predefinitions
PREDEFINE(DrawableObject, pDrawableObject);
PREDEFINE(Mesh, pMesh);
PREDEFINE(Material, pMaterial);
PREDEFINE(ConstantBuffer, pConstantBuffer);


class DrawableObject : public BaseDrawable
{
public:
	DrawableObject() : BaseDrawable() {}
	~DrawableObject() {};

	void Init(pMesh inMesh, pMaterial inMaterial);
	void CleanUp();

	pMesh GetMesh() { return mMesh; }
	pMaterial GetMaterial() { return mMaterial; }
	pConstantBuffer GetConstantBuffer();

private:
	struct ConstantBufferData
	{
		DirectX::XMMATRIX modelView;
		DirectX::XMMATRIX modelViewProjection;
		DirectX::XMMATRIX prevModelViewProjection;
	};
	ConstantBufferData mConstantBufferData;
	
	pMesh mMesh = nullptr;
	pMaterial mMaterial = nullptr;
	pConstantBuffer mConstantBuffer = nullptr;
};

