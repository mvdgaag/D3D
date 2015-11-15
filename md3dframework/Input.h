#pragma once
#include <Windows.h>
#include "GaagCommon.h"


#define theInput Input::GetInstance()

PREDEFINE(InputListener, pInputListener);


class Input
{
public:
	static Input& GetInstance()
	{
		static Input instance;
		return instance;
	}

	static void RegisterListener(pInputListener inListener) { mListeners.insert(inListener); }
	static void UnRegisterListener(pInputListener inListener) { if (mListeners.find(inListener) != mListeners.end()) mListeners.erase(inListener); }

	bool GetKeyDown(unsigned int inKey) { return mKeys[inKey]; }
	float2 GetMouseCoord() { return mMouseCoord; }
	float2 GetPrevMouseCoord() { return mPrevMouseCoord; }
	bool GetMouseButton(unsigned int inButton) { return mMouseButtons[inButton]; }

	void OnKeyDown(unsigned int inKey);
	void OnKeyUp(unsigned int inKey);
	void OnMouseMove(float2 inCoord);
	void OnMouseDown(int inButton);
	void OnMouseUp(int inButton);

protected:
	bool mMouseButtons[3];
	bool mKeys[256];
	float2 mPrevMouseCoord;
	float2 mMouseCoord;

private:
	Input() {};
	~Input() {};
	Input(Input const&) = delete;
	void operator=(Input const&) = delete;

	static std::unordered_set<pInputListener> mListeners;
};

