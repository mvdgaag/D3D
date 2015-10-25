#include "FileUtil.h"

namespace FileUtil
{
	bool FileExists(const char *fileName)
	{
		std::ifstream infile(fileName);
		return infile.good();
	}
}