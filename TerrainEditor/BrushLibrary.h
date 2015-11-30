#pragma once
#include "Gaag.h"
#include "Brush.h"

REGISTERCLASS(BrushLibrary);

class BrushLibrary
{
public:
	BrushLibrary() {};
	~BrushLibrary() {};
	
	void Init();
	void CleanUp();

	pBrush GetBrush(std::string inName);
	void AddBrush(std::string inName, pBrush inBrush);
	void RemoveBrush(std::string inName);

private:
	std::unordered_map<std::string, pBrush> mBrushes;
};

