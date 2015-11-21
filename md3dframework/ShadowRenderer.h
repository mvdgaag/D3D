#pragma once
#include "GaagCommon.h"


class ShadowRenderer
{
public:
	ShadowRenderer() {}
	~ShadowRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render();

private:
	ShadowRenderer(ShadowRenderer const&) = delete;
	void operator=(ShadowRenderer const&) = delete;

};

