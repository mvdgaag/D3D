#pragma once
#include "BaseObject.h"
#include "GaagCommon.h"

// predefinitions
REGISTERCLASS(MeshObject);
REGISTERCLASS(Mesh);
REGISTERCLASS(Material);
REGISTERCLASS(ConstantBuffer);


class MeshObject : public BaseObject
{
public:
	MeshObject() : BaseObject() {}
	~MeshObject() { CleanUp(); }

	void Init(pMesh inMesh, pMaterial inMaterial);
	void CleanUp() override;
	ObjectType GetObjectType() const override { return ObjectType::MESH_OBJECT; }

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

