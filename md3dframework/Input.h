#pragma once
#include <Windows.h>
#include "GaagCommon.h"


#define theInput Input::GetInstance()

REGISTERCLASS(InputListener);


class Input
{
public:
	static Input& GetInstance()
	{
		static Input instance;
		return instance;
	}

	void Init();
	void CleanUp();

	static void RegisterListener(InputListener* inListener) { if (mInitialized) mListeners.insert(inListener); }
	static void UnRegisterListener(InputListener* inListener) { if (mInitialized) mListeners.erase(inListener); }

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

	static bool mInitialized;
	static std::unordered_set<InputListener*> mListeners;
};

