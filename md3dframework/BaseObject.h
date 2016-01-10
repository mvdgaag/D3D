#pragma once
#include "GaagCommon.h"


REGISTERCLASS(BaseObject);
REGISTERCLASS(BaseComponent);


enum ObjectType
{
	MESH_OBJECT,
	// TODO: lights must become objects
	//POINT_LIGHT,
	//SPOT_LIGHT,
	//DIRECTIONAL_LIGHT,
	PARTICLE_SYSTEM
};


class BaseObject
{
public:
	explicit BaseObject();
	virtual ~BaseObject();

	virtual void			CleanUp()				= 0;
	virtual ObjectType		GetObjectType() const	= 0;

	unsigned long GetUUID() const					{ return mUUID; }

	void						AddComponent(pBaseComponent inComponent);
	void						RemoveComponent(pBaseComponent inComponent);
	const apBaseComponent		GetComponents()	  	{ return mComponents; }
	const apBaseObject			GetChildren()	  	{ return mChildren; }
	const float3				GetPosition()	  	{ return float3(mTransform[3]) / mTransform[3].w; }
	const float4x4&				GetTransform()	  	{ return mTransform; }
	const float4x4&				GetPrevTransform()	{ return mPrevTransform; }
	void						SwapTransform()	  	{ mPrevTransform = mTransform; }

	void Rotate(float3 inAxis, float inAngle)	
	{ 
		mTransform = rotate(mTransform, inAngle, inAxis);
	}
	
	void Translate(float3 inTranslation)			
	{ 
		mTransform = translate(mTransform, inTranslation);
	}

protected:
	float4x4		mTransform;
	float4x4		mPrevTransform;
	pBaseObject		mParent;
	apBaseObject	mChildren;
	apBaseComponent mComponents;
	unsigned long	mUUID;

private:
	BaseObject(BaseObject const&) = delete;
	void operator=(BaseObject const&) = delete;
	static unsigned long sCurrentObjectID;
};

