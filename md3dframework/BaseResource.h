#pragma once
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

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
	MATERIAL
};


class BaseResource
{
	friend class ResourceFactory;

public:
	virtual ~BaseResource() { } //TODO: unregister from factory?
	explicit BaseResource() { UuidCreate(&mUUID); }

	virtual void			CleanUp()					= 0;
	virtual ResourceType	GetResourceType() const		= 0;
	
	UUID GetUUID() const	{ return mUUID; }

protected:
	UUID mUUID;

private:
};

