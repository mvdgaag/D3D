#pragma once
#include "GaagCommon.h"


REGISTERCLASS(BaseComponent);
REGISTERCLASS(BaseObject);


enum ComponentType
{
	PHYSICS_COMPONENT,
	AI_COMPONENT,
	// TODO: materials and meshes can become components
	MATERIAL_COMPONENT
};


class BaseComponent
{
	friend class BaseObject;
public:
	explicit BaseComponent();
	virtual ~BaseComponent();

	virtual void			CleanUp()					= 0;
	virtual ComponentType	GetComponentType() const	= 0;

	unsigned long			GetUUID() const						{ return mUUID; }
	pBaseObject				GetParent() const					{ return mParent; }

protected:
	unsigned long mUUID;
	pBaseObject mParent;

private:
	BaseComponent(BaseComponent const&) = delete;
	void operator=(BaseComponent const&) = delete;
	static unsigned long sCurrentComponentID;
};

