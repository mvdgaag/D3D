#pragma once
#include <string>
#include <unordered_map>


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
	explicit BaseResource() {};
	virtual ~BaseResource() {};

	virtual void			CleanUp()					= 0;
	virtual ResourceType	GetResourceType() const		= 0;
};

