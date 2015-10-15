#pragma once


class ShadowRenderer
{
public:
	ShadowRenderer() {}
	~ShadowRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render();
};

