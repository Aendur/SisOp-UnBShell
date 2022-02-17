#include <stdexcept>
#include <random>
#include "parser.h"

using std::string;
using std::vector;

class action_args {
public:
	const Token& token;
	Pipeline& pipeline;
	inline action_args(const Token& t, Pipeline& v) : token(t), pipeline(v) { }
};

//Parser::Parser(const vector<Token>& tok) : tokens(tok) { this->state = PState::START_0; }
Parser::Parser(const string & line) : line(line), tokens(Scanner::scan(line)) {
	this->state = PState::START_0;
}

Pipeline Parser::run(void) {
	Pipeline pipeline(line);
	for (const Token& tokn : tokens) {
		auto key = std::pair<PState, Symbol>{ state, tokn.type };
		auto tsit = transitions.find(key);
		if (tsit == transitions.end()) {
			// PARSER ERROR
			throw std::runtime_error("parser error: unexpected " + to_string(tokn.type));
		} else {
			action_args args(tokn, pipeline);
			tsit->second.action(this, &args);
			// printf("%-8s %-8s -> ", to_string(state).c_str(), to_string(tokn.type).c_str());
			// printf("%-8s %s\n", to_string(tsit->second.state_tgt).c_str(), tokn.value.c_str());
			state = tsit->second.state_tgt;
		}
	}
	return pipeline;
}

void Parser::shift(const void* args) {
	// printf("SHIFT    ");
	const Token& tokn = ((action_args*)args)->token;
	token_queue.push_back(tokn);
}

void concat_either(std::deque<Token> & queue, Args & args, const string & value) {
	if (value[0] == ' ') {
		// concat next " ="
		const string & v2 = queue.size() >= 2 ? queue[1].value : "";
		args.push_back(value[1] + v2);
		if (!queue.empty()) { queue.pop_front(); } // pops eq sign
		if (!queue.empty()) { queue.pop_front(); } // pops next value
	} else {
		// concat prev "= "
		if (args.empty()) { args.push_back(value[0] + ""); }
		else /* !empty */ { args.back() = args.back() + value[0]; }

		if (!queue.empty()) { queue.pop_front(); } // pops eq sign
	}
}

void concat_both(std::deque<Token> & queue, Args & args, const string & value) {
	const string & v2 = queue.size() >= 2 ? queue[1].value : "";

	if (args.empty()) { args.push_back(value + v2); }
	else /* !empty */ { args.back() = args.back() + value + v2; }

	if (!queue.empty()) { queue.pop_front(); } // pops eq sign
	if (!queue.empty()) { queue.pop_front(); } // pops next value
}

Args Parser::reduce(bool concat_eq) {
	Args cmd_args;
	while (token_queue.empty() == false) {
		const string & value = token_queue.front().value;
		Symbol type = token_queue.front().type;
		
		// expand environment variables
		if (value.length() > 0 && value[0] == '$') {
			if (value.substr(1,4) == "RAND") {
				unsigned long long rand_num = std::random_device()();
				try { if (value.size() > 5) { rand_num %= std::stoi(value.substr(5)); } }
				catch (std::exception&) {}
				cmd_args.push_back(std::to_string(rand_num));
				token_queue.pop_front();
			} else {
				const char * env_var = getenv(value.substr(1).c_str());
				cmd_args.push_back(string(env_var));
				token_queue.pop_front();
			}
		} else if (value.length() > 0 && value[0] == '~') {
			const char * env_var = getenv("HOME");
			string val = string(env_var) + (value.length() > 1 ? value.substr(1) : "");
			cmd_args.push_back(val);
			token_queue.pop_front();
		}
		// strip quotes
		else if (type == Symbol::LITERAL1 || type == Symbol::LITERAL2) {
			cmd_args.push_back(value.substr(1,value.length()-2));
			token_queue.pop_front();
		} else {
			// check if eq sign needs to be concatenated
			if (concat_eq && type == Symbol::OP_EQ) {
				switch(value.length()) {
				case 1:
					concat_both(token_queue, cmd_args, value);
					break;
				case 2: // concat prev OR next, check which
					concat_either(token_queue, cmd_args, value);
					break;
				case 3:
					cmd_args.push_back(value);
					token_queue.pop_front(); 
					break; // concat neither
				default: throw std::logic_error("parser error: strange OP_EQ");
				}
			}
			
			// push command
			else {
				cmd_args.push_back(value);
				token_queue.pop_front(); 
			}
		}
	}

	//for (auto & i : cmd_args) { printf("'%s'\n", i.c_str()); }
	return cmd_args;
}

void Parser::reduce_empty(const void*) {}

string process_path(const string & src) {
	int siz = src.length();
	char * tgt = (char*) malloc(sizeof(char) * siz + 1);
	for (int i = 0; i < siz; ++i) {
		tgt[i] = (src[i] == ';' ? ':' : src[i]);
	}
	tgt[siz] = 0;
	string newstr = string(tgt);
	free(tgt);
	return newstr;
}


void Parser::reduce_expression(const void*) {
	if (token_queue.size() == 3) {
		const string & var_name = token_queue.front().value;
		if (var_name == "PATH") {
			const string & old_path = token_queue.back().value;
			const string & new_path = process_path(old_path);
			// printf("%s\n", var_name.c_str());
			// printf("%s\n", new_path.c_str());
			// printf("%s\n", old_path.c_str());
			setenv(var_name.c_str(), new_path.c_str(), 1);
		} else {
			setenv(var_name.c_str(), token_queue.back().value.c_str(), 1);
		}
		token_queue.clear();
	} else {
		token_queue.clear();
		throw std::logic_error("parser error: unable to parse expression");
	}
}

void Parser::reduce_command(const void* args) {
	// printf("R_COMMND ");
	Pipeline& pipeline = ((action_args*)args)->pipeline;

	Args cmd_args = reduce(true);
	pipeline.add_command(Command(cmd_args));
}

void Parser::reduce_redirection(const void* args) {
	// printf("R_REDRCT ");
	Pipeline& pipeline = ((action_args*)args)->pipeline;

	string filename = token_queue.back().value;
	token_queue.pop_back();

	bool append = token_queue.back().value.length() == 2;
	char redirect = token_queue.back().value[0];
	token_queue.pop_back();

	Args cmd_args = reduce(true);
	Command cmd = Command(cmd_args);

	switch(redirect) {
		case '<': cmd.set_input_source(filename); break;
		case '>': cmd.set_output_target(filename); break;
		default: throw std::logic_error("parser error: invalid redirect operation");
	}
	
	if (append) { cmd.set_append(); }

	pipeline.add_command(cmd);
}

void Parser::reduce_amp_cmd(const void* args) {
	// printf("R_AMPCMD ");
	token_queue.pop_back();
	reduce_command(args);
	((action_args*)args)->pipeline.set_background();
}

void Parser::reduce_amp_rdt(const void* args) {
	// printf("R_AMPRDT ");
	token_queue.pop_back();
	reduce_redirection(args);
	((action_args*)args)->pipeline.set_background();
}

void Parser::register_transition(PState src, Symbol sym, PState tgt, const std::function<ftype>& fun) {
	transitions[std::pair(src, sym)] = Transition(src, sym, tgt, fun);
}

Pipeline Parser::parse(const string& line) {
	static bool initialized = false;
	if (!initialized) {
		register_transition(PState::START_0, Symbol::ID      , PState::CMD_EXP, &Parser::shift);
		register_transition(PState::START_0, Symbol::EOL     , PState::START_0, &Parser::reduce_empty);

		register_transition(PState::CMD_EXP, Symbol::OP_EQ   , PState::EXPRESS, &Parser::shift);
		register_transition(PState::CMD_EXP, Symbol::ID      , PState::COMMAND, &Parser::shift);
		register_transition(PState::CMD_EXP, Symbol::LITERAL1, PState::COMMAND, &Parser::shift);
		register_transition(PState::CMD_EXP, Symbol::LITERAL2, PState::COMMAND, &Parser::shift);

		register_transition(PState::CMD_EXP, Symbol::OP_AMP  , PState::AMP_CMD, &Parser::shift);
		register_transition(PState::CMD_EXP, Symbol::OP_PIPE , PState::START_0, &Parser::reduce_command);
		register_transition(PState::CMD_EXP, Symbol::EOL     , PState::END    , &Parser::reduce_command);

		register_transition(PState::EXPRESS, Symbol::ID      , PState::EXP_END, &Parser::shift);
		register_transition(PState::EXPRESS, Symbol::LITERAL1, PState::EXP_END, &Parser::shift);
		register_transition(PState::EXPRESS, Symbol::LITERAL2, PState::EXP_END, &Parser::shift);
		register_transition(PState::EXP_END, Symbol::EOL     , PState::END    , &Parser::reduce_expression);

		register_transition(PState::COMMAND, Symbol::ID      , PState::COMMAND, &Parser::shift);
		register_transition(PState::COMMAND, Symbol::LITERAL1, PState::COMMAND, &Parser::shift);
		register_transition(PState::COMMAND, Symbol::LITERAL2, PState::COMMAND, &Parser::shift);
		register_transition(PState::COMMAND, Symbol::OP_EQ   , PState::COMMAND, &Parser::shift);
		register_transition(PState::COMMAND, Symbol::OP_AMP  , PState::AMP_CMD, &Parser::shift);
		register_transition(PState::COMMAND, Symbol::OP_PIPE , PState::START_0, &Parser::reduce_command);
		register_transition(PState::COMMAND, Symbol::EOL     , PState::END    , &Parser::reduce_command);

		register_transition(PState::CMD_EXP, Symbol::OP_RIN  , PState::R_START, &Parser::shift);
		register_transition(PState::CMD_EXP, Symbol::OP_ROUT , PState::R_START, &Parser::shift);
		register_transition(PState::COMMAND, Symbol::OP_RIN  , PState::R_START, &Parser::shift);
		register_transition(PState::COMMAND, Symbol::OP_ROUT , PState::R_START, &Parser::shift);
		register_transition(PState::R_START, Symbol::ID      , PState::R_FINIS, &Parser::shift);
		register_transition(PState::R_FINIS, Symbol::OP_AMP  , PState::AMP_RDT, &Parser::shift);
		register_transition(PState::R_FINIS, Symbol::OP_PIPE , PState::START_0, &Parser::reduce_redirection);
		register_transition(PState::R_FINIS, Symbol::EOL     , PState::END    , &Parser::reduce_redirection);

		register_transition(PState::AMP_CMD, Symbol::EOL     , PState::END    , &Parser::reduce_amp_cmd);
		register_transition(PState::AMP_RDT, Symbol::EOL     , PState::END    , &Parser::reduce_amp_rdt);
	}

	//const vector<Token>& tokens = Scanner::scan(line);
	Parser parser(line);
	return parser.run();
}


std::string to_string(PState s) {
	switch (s) {
	case PState::UNDEFINED: return "UNDEFINED";
	case PState::START_0: return "START_0";
	case PState::CMD_EXP: return "CMD_EXP";
	case PState::EXPRESS: return "EXPRESS";
	case PState::EXP_END: return "EXP_END";
	case PState::COMMAND: return "COMMAND";
	case PState::R_START: return "R_START";
	case PState::R_FINIS: return "R_FINIS";
	case PState::AMP_CMD: return "AMP_CMD";
	case PState::AMP_RDT: return "AMP_RDT";
	case PState::END: return "END";
	default: throw std::runtime_error("unknown enum value");
	}
}


