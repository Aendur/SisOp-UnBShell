#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <functional>
#include "scanner.h"
#include "command.h"
#include "pipeline.h"


class Parser;
//class Command;
//class Pipeline;
//class Args;

using ftype = void(Parser*, const void*);

enum class PState {
	UNDEFINED,
	START_0,
	CMD_EXP,

	EXPRESS,
	EXP_END,

	COMMAND,
	R_START,
	R_FINIS,

	AMP_CMD,
	AMP_RDT,
	END,
};

std::string to_string(PState);

class Transition {
public:
	PState state_src = PState::UNDEFINED;
	Symbol symbol = Symbol::UNDEFINED;
	PState state_tgt = PState::UNDEFINED;
	std::function<ftype> action;

	inline Transition(void) { }
	inline Transition(
		PState state1,
		Symbol symbol1,
		PState state2,
		std::function<ftype> func
	) : state_src(state1), symbol(symbol1), state_tgt(state2), action(func) { }
};

using TMap = std::map<std::pair<PState, Symbol>, Transition>;

class Parser {
public:
	static Pipeline parse(const std::string& line);

private:
	static void register_transition(PState src, Symbol sym, PState tgt, const std::function <ftype>& fun);
	static inline TMap transitions = TMap();

	Parser(void) = delete;
	// Parser(const std::vector<Token>&);
	Parser(const std::string&);

	PState state;
	const std::string line;
	const std::vector<Token> tokens;
	std::deque<Token> token_queue;

	Pipeline run(void);

	void shift(const void*);
	
	Args reduce(bool);
	void reduce_empty(const void*);
	void reduce_expression(const void*);
	void reduce_command(const void*);
	void reduce_redirection(const void*);
	void reduce_amp_cmd(const void*);
	void reduce_amp_rdt(const void*);
};

#endif

