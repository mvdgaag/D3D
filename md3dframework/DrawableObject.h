#pragma once
#include "BaseDrawable.h"
#include "GaagCommon.h"

// predefinitions
REGISTERCLASS(DrawableObject);
REGISTERCLASS(Mesh);
REGISTERCLASS(Material);
REGISTERCLASS(ConstantBuffer);


class DrawableObject : public BaseDrawable
{
public:
	DrawableObject() : BaseDrawable() {}
	~DrawableObject() {};

	void Init(pMesh inMesh, pMaterial inMaterial);
	void CleanUp();

	pMesh			GetMesh()		{ return mMesh; }
	pMaterial		GetMaterial()	{ return mMaterial; }
	AABB			GetAABB()		{ return mAABB; }

	// Can be overridden to set custom textures/constantbuffers
	// Note that some textures and constantbuffers are reserved by the deferred renderer!
	virtual void	PrepareToDraw()				{}
	virtual void	FinalizeAfterDraw()			{}
	virtual void	PrepareToDrawShadow()		{}
	virtual void	FinalizeAfterDrawShadow()	{}

private:
	AABB mAABB;
	pMesh mMesh = nullptr;
	pMaterial mMaterial = nullptr;
	pConstantBuffer mConstantBuffer = nullptr;
};

