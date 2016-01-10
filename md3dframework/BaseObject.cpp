#include "BaseObject.h"
#include "BaseComponent.h"


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
		assert(result != siblings.end());
		siblings.erase(result);
	}

	for each (pBaseComponent component in mComponents)
		component->mParent = nullptr;
}


void BaseObject::AddComponent(pBaseComponent inComponent)
{
	assert(inComponent);
	mComponents.push_back(inComponent);
	if (inComponent->mParent)
		inComponent->mParent->RemoveComponent(inComponent);
	inComponent->mParent = this;
}


void BaseObject::RemoveComponent(pBaseComponent inComponent)
{
	auto result = std::find(mComponents.begin(), mComponents.end(), inComponent);
	(*result)->mParent = nullptr;
	if (result != mComponents.end())
		mComponents.erase(result);
}