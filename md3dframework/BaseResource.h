#pragma once
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

#include <string>
#include <unordered_map>
#include <windows.h>
#include <iostream>


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
	CONSTANT_BUFFER
};


class BaseResource
{
public:
	explicit BaseResource() { UuidCreate(&mUUID); }
	virtual ~BaseResource() {}

	virtual void			CleanUp()					= 0;
	virtual ResourceType	GetResourceType() const		= 0;
	
	UUID GetUUID() const	{ return mUUID; }

protected:
	UUID mUUID;
};

