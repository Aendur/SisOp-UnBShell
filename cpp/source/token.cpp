#include <stdexcept>

#include "token.h"

std::string to_string(Symbol s) {
	switch (s) {
	case Symbol::UNDEFINED: return "UNDEFINED";
	case Symbol::EOL: return "EOL";
	case Symbol::FULL_MATCH: return "FULL_MATCH";
	case Symbol::COMMENT: return "COMMENT";
	case Symbol::LITERAL1: return "LITERAL1";
	case Symbol::LITERAL2: return "LITERAL2";
	case Symbol::OP_RIN: return "OP_RIN";
	case Symbol::OP_ROUT: return "OP_ROUT";
	case Symbol::OP_PIPE: return "OP_PIPE";
	case Symbol::OP_EQ: return "OP_EQ";
	case Symbol::OP_AMP: return "OP_AMP";
	case Symbol::ID: return "ID";
	case Symbol::UNMATCHED: return "UNMATCHED";
	default: throw std::runtime_error("unknown enum value");
	}
}
