#pragma once
#include "GaagCommon.h"
#include "BaseObject.h"


REGISTERCLASS(ParticleSystem);


class ParticleSystem : public BaseObject
{
public:
	ParticleSystem() {}
	~ParticleSystem() { CleanUp(); }

	void Init();
	void CleanUp() override;

	ObjectType GetObjectType() const override { return ObjectType::PARTICLE_SYSTEM; }

private:
	ParticleSystem(ParticleSystem const&) = delete;
	void operator=(ParticleSystem const&) = delete;
};



