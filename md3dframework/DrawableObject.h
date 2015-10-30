#pragma once
#include "BaseDrawable.h"
#include "GaagCommon.h"

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

	pMesh			GetMesh()				{ return mMesh; }
	pMaterial		GetMaterial()			{ return mMaterial; }

	// Can be overridden to set custom textures/constantbuffers
	// Note that some textures and constantbuffers are reserved by the deferred renderer!
	virtual void	PrepareToDraw()			{}					

private:
	pMesh mMesh = nullptr;
	pMaterial mMaterial = nullptr;
	pConstantBuffer mConstantBuffer = nullptr;
};

