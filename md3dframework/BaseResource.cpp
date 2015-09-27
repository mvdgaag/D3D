#include "BaseResource.h"
#include <assert.h>


std::unordered_map<std::string, BaseResource*> BaseResource::sResourceMap;


BaseResource::BaseResource(std::string inName)
{ 
	mName = inName;
	assert(sResourceMap.find(inName) == sResourceMap.end());
	sResourceMap[inName] = this;
}


BaseResource::~BaseResource()
{
	assert(sResourceMap.find(mName) != sResourceMap.end());
	sResourceMap.erase(mName);
}