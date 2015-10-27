#pragma once
#include "GAAGCommon.h"


class ShadowRenderer
{
public:
	ShadowRenderer() {}
	~ShadowRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render();
};

