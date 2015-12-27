#pragma once

#include "GaagCommon.h"

REGISTERCLASS(AiAgent);

class AiAgent
{
public:
	AiAgent()	{};
	~AiAgent()	{};

	virtual void Update() = 0;
};

