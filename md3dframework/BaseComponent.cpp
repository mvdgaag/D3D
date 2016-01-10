#include "BaseComponent.h"
#include "BaseObject.h"


unsigned long BaseComponent::sCurrentComponentID = 0;


BaseComponent::BaseComponent()
{
	mUUID = sCurrentComponentID++;
	mParent = nullptr;
}


BaseComponent::~BaseComponent()
{
	if (mParent)
		mParent->RemoveComponent(this);
}

