#pragma once
#include <Windows.h>
#include "GAAGCommon.h"
#include <hash_set>


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

	void RegisterListener(pInputListener inListener) { mListeners.insert(inListener); }
	void UnRegisterListener(pInputListener inListener) { mListeners.erase(inListener); }

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
	bool mKeys[256];
	float2 mPrevMouseCoord;
	float2 mMouseCoord;
	std::hash_set<pInputListener> mListeners;

private:
	Input() {};
	~Input() {};
	Input(Input const&) = delete;
	void operator=(Input const&) = delete;
};

