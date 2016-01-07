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
	// todo: this is slow
	if (mParent != nullptr)
	{
		apBaseComponent siblings = mParent->GetComponents();
		auto result = std::find(siblings.begin(), siblings.end(), this);
		assert(result != siblings.end()); // could be conditional
		siblings.erase(result);
	}
}