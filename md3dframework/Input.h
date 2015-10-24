#pragma once
#include <Windows.h>
#include "LinearAlgebra.h"
#include <vector>


#define theInput Input::GetInstance()


class InputListener;


class Input
{
public:
	static Input& GetInstance()
	{
		static Input instance;
		return instance;
	}

	bool GetKeyDown(unsigned int inKey) { return mKeys[inKey]; }
	float2 GetMouseCoord() { return mMouseCoord; }
	float2 GetPrevMouseCoord() { return mPrevMouseCoord; }
	float2 GetMouseButton(unsigned int inButton) { return mMouseButtons[inButton]; }

	void OnKeyDown(unsigned int inKey);
	void OnKeyUp(unsigned int inKey);
	void OnMouseMove(float2 inCoord);
	void OnMouseDown(int inButton);
	void OnMouseUp(int inButton);

protected:
	bool mMouseButtons[3];
	bool mKeys[255];
	float2 mPrevMouseCoord;
	float2 mMouseCoord;
	std::vector<InputListener*> mListeners;

private:
	Input() {};
	~Input() {};
	Input(Input const&) = delete;
	void operator=(Input const&) = delete;
};

