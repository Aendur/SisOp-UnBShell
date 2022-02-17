#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <string>
#include <vector>
#include <unordered_map>
#include "token.h"

//class Token;

class Scanner {
private:
	Scanner(void){}
//	~Scanner(void){}

public:
	static const std::vector<Token> scan(const std::string & line, bool verbose = false);
};


#endif

