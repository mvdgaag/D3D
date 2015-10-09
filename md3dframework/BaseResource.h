#pragma once
#include <string>
#include <unordered_map>


enum ResourceType
{
	MESH,
	TEXTURE,
	RENDER_TARGET,
	SAMPLER,
	PIXEL_SHADER,
	VERTEX_SHADER,
	COMPUTE_SHADER,
	CONSTANT_BUFFER
};


class BaseResource
{
public:
	explicit BaseResource(std::string inName) : mName(inName) {};
	virtual ~BaseResource() {};

	virtual void			CleanUp()					= 0;
	virtual ResourceType	GetResourceType() const		= 0;
	std::string				GetName() const				{ return mName; }

protected:
	std::string mName;
};

