#pragma once

#include "GaagCommon.h"

REGISTERCLASS(AiManager);
REGISTERCLASS(AiAgent);

class AiManager
{
public:
	AiManager()		{};
	~AiManager()	{};

	void RegisterAgent(pAiAgent inAgent)	{ assert(mAgents.find(inAgent) == mAgents.end()); mAgents.insert(inAgent); }
	void UnRegisterAgent(pAiAgent inAgent)	{ assert(mAgents.find(inAgent) != mAgents.end()); mAgents.erase(inAgent); }
	void Update();

private:
	std::unordered_set<pAiAgent> mAgents;
};

