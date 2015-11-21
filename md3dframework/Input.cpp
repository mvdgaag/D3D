#include "Input.h"
#include "InputListener.h"


void Input::Init()
{
	mInitialized = true;
}


void Input::CleanUp()
{
	mListeners.clear();
	mInitialized = false;
}


void Input::OnKeyDown(unsigned int inKey)
{
	mKeys[inKey] = true;
	for each (pInputListener listener in mListeners)
		listener->OnKeyDown(inKey);
}


void Input::OnKeyUp(unsigned int inKey)
{
	mKeys[inKey] = false;
	for each (pInputListener listener in mListeners)
		listener->OnKeyUp(inKey);
}


void Input::OnMouseMove(float2 inCoord)
{
	mPrevMouseCoord = mMouseCoord;
	mMouseCoord = inCoord;
	for each (pInputListener listener in mListeners)
		listener->OnMouseMove(inCoord, mPrevMouseCoord);
}


void Input::OnMouseDown(int inButton)
{
	mMouseButtons[inButton] = true;
	for each (pInputListener listener in mListeners)
		listener->OnMouseDown(inButton);
}


void Input::OnMouseUp(int inButton)
{
	mMouseButtons[inButton] = false;
	for each (pInputListener listener in mListeners)
		listener->OnMouseUp(inButton);
}
