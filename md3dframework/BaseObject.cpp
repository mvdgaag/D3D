#include "BaseObject.h"


unsigned long BaseObject::sCurrentObjectID = 0;


BaseObject::BaseObject()
{
	mUUID = sCurrentObjectID++; 
	mParent = nullptr; 
}


BaseObject::~BaseObject()
{
	// todo: this is slow
	if (mParent != nullptr)
	{
		apBaseObject siblings = mParent->GetChildren();
		auto result = std::find(siblings.begin(), siblings.end(), this);
		assert(result != siblings.end()); // could be conditional
		siblings.erase(result);
	}
}