#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include <vector>

namespace Utility {
	std::vector<std::string> split(const std::string& str, const std::string& pattern);
	std::string strip(const std::string& str);
};


#endif
