#include "AiManager.h"
#include "AiAgent.h"


void AiManager::Update()
{
	for (std::unordered_set<pAiAgent>::iterator itr = mAgents.begin(); itr != mAgents.end(); ++itr) 
	{
		pAiAgent agent = *itr;
		agent->Update();
	}
}