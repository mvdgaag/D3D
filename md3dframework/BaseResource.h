#pragma once
#include "GaagCommon.h"


REGISTERCLASS(BaseResource);


enum ResourceType
{
	MESH,
	TEXTURE,
	RENDER_TARGET,
	DEPTH_STENCIL_TARGET,
	SAMPLER,
	PIXEL_SHADER,
	VERTEX_SHADER,
	COMPUTE_SHADER,
	CONSTANT_BUFFER,
	MATERIAL,
	POINT_LIGHT,
	SPOT_LIGHT,
	DIRECTIONAL_LIGHT,
	RASTERIZER_STATE,
	VIEWPORT
};


class BaseResource
{
	friend class ResourceFactory;

public:
	virtual ~BaseResource()								{ }
	explicit BaseResource()								{ mUUID = sCurrentResourceID++; }

	virtual void			CleanUp()					= 0;
	virtual ResourceType	GetResourceType() const		= 0;
	
	unsigned long			GetUUID() const				{ return mUUID; }

protected:
	unsigned long mUUID;

private:
	BaseResource(BaseResource const&) = delete;
	void operator=(BaseResource const&) = delete;
	static unsigned long sCurrentResourceID;
};

