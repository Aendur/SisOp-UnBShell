#include <sstream>
#include <regex>
#include "scanner.h"
//#include "token.h"

using std::string;
using std::vector;
using std::unordered_map;

Token tokenize(const std::smatch & result);
void print_matches(const std::smatch & result, int nmatch);

vector<Symbol> pattern_keys;
vector<string> pattern_vals;

std::ostream & operator<<(std::ostream& builder, const std::pair<Symbol, const string> & keyval) {
	pattern_keys.push_back(keyval.first);
	pattern_vals.push_back(keyval.second);
	builder << keyval.second;
	return builder;
}

const vector<Token> Scanner::scan(const std::string & line, bool verbose) {
	static bool initialized = false;
	static std::regex regex;
	if (!initialized) {
		//printf("initializing\n");
		initialized = true;
		std::stringstream builder;
		builder    << std::pair<Symbol, const string>(Symbol::COMMENT  , "(#.*)")
			<< '|' << std::pair<Symbol, const string>(Symbol::LITERAL1 , "('[^']*')")
			<< '|' << std::pair<Symbol, const string>(Symbol::LITERAL2 , "(\"[^\"]*\")")
			<< '|' << std::pair<Symbol, const string>(Symbol::OP_RIN   , "(<)")
			<< '|' << std::pair<Symbol, const string>(Symbol::OP_ROUT  , "(>{1,2})")
			<< '|' << std::pair<Symbol, const string>(Symbol::OP_PIPE  , "(\\|)")
			<< '|' << std::pair<Symbol, const string>(Symbol::OP_EQ    , "( ?\\= ?)")
			<< '|' << std::pair<Symbol, const string>(Symbol::OP_AMP   , "(\\&)")
			<< '|' << std::pair<Symbol, const string>(Symbol::ID       , "([\\./\\-;:~$\\w]+)")
			<< '|' << std::pair<Symbol, const string>(Symbol::UNMATCHED, "(\\S)")
			;

		//printf("%s\n", builder.str().c_str());
		regex = std::regex(builder.str());
	}

	std::smatch result;
	string src = line;
	vector<Token> output;

	int nmatch = 0;
	while (std::regex_search(src, result, regex)) {
		++nmatch;

		const Token & tok = tokenize(result);
		if (tok.type == Symbol::COMMENT) {
			output.push_back(Token("", Symbol::EOL));
			return output;
		} else {
			output.push_back(tok);
		}

		if (verbose) { print_matches(result, nmatch); }
		src = result.suffix();
	}
	
	output.push_back(Token("", Symbol::EOL));
	return output;
}

void print_matches(const std::smatch & result, int nmatch) {
	for(int npattern = 0; npattern < (int) result.size(); ++npattern) {
		const auto  & i = result[npattern];

		const char * pat_nam = npattern == 0 ? "FULL_MATCH" : to_string(pattern_keys[npattern-1]).c_str();
		const char * pat_val = npattern == 0 ? "----------" : pattern_vals[npattern-1].c_str();
		printf("%2d %2d %-12s %-16s %-5s   match(%s)\n", nmatch, npattern, pat_nam, pat_val, i.matched ? "true" : "false", i.str().c_str());
	}
	printf("\n");
}

Token tokenize(const std::smatch & result) {
	for(int npattern = 1; npattern < (int) result.size(); ++npattern) {
		const auto & sub = result[npattern];
		Symbol type = pattern_keys[npattern-1];
		const string & val = sub.str();		
		
		if (sub.matched) {
			if (type == Symbol::UNMATCHED) {
				printf("UNMATCHED %s\n", val.c_str());
				throw std::runtime_error("scanner error: unrecognized token: " + val);
			} else {
				return Token(val, type);
			}
		} 
	}
	throw std::runtime_error("scanner error: no matches");
}

// awk "$1 + $2"
