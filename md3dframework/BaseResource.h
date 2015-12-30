#pragma once
#include "GaagCommon.h"
#include <string>
#include <unordered_map>
#include <windows.h>
#include <iostream>


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
	DIRECTIONAL_LIGHT
};


class BaseResource
{
	friend class ResourceFactory;

public:
	virtual ~BaseResource() { }
	explicit BaseResource() { mUUID = sCurrentID++; }

	virtual void			CleanUp()					= 0;
	virtual ResourceType	GetResourceType() const		= 0;
	
	long GetUUID() const	{ return mUUID; }

protected:
	long mUUID;

private:
	BaseResource(BaseResource const&) = delete;
	void operator=(BaseResource const&) = delete;
	static long sCurrentID;
};

