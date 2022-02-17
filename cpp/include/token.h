#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <string>

enum class Symbol {
	// Control	
	UNDEFINED = -100,
	EOL = -1,
	FULL_MATCH = 0,

	// Terminals	
	COMMENT = 1,
	LITERAL1,
	LITERAL2,
	OP_RIN,
	OP_ROUT,
	OP_PIPE,
	OP_EQ,
	OP_AMP,
	ID,
	UNMATCHED,
};

std::string to_string(Symbol);

class Token {
public:
	std::string value;
	Symbol type;

	inline Token(const std::string& v, Symbol t) : value(v), type(t) { }
};

#endif

