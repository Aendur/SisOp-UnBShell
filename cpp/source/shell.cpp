#include <regex>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>

#include "shell.h"
#include "parser.h"
#include "utility.h"
#include "input_field.h"
#include "defines.h"

using std::string, std::vector;

vector<string> Shell::get_paths(void) {
	const char* env_var = getenv("PATH");
	if (env_var != NULL) {
		return Utility::split(string(env_var), ":");
	} else {
		return vector<string>();
	}
}

string Shell::get_path(void) {
	const char* env_var = getenv("PATH");
	if (env_var != NULL) {
		return string(env_var);
	} else {
		return "";
	}
}

string Shell::get_home(void) {
	const char* env_var = getenv("HOME");
	if (env_var != NULL) {
		return string(env_var);
	} else {
		return "";
	}
}

string Shell::get_working_directory(void) {
	const char* env_var = getenv("PWD");
	if (env_var != NULL) {
		return string(env_var);
	} else {
		return "";
	}
}

string Shell::get_user(void) {
	char* env_var = getenv("USER");
	if (env_var != NULL) {
		return string(env_var);
	} else {
		return "";
	}
}

void Shell::parse_and_execute(const string& line, bool add_history) {
	string sline = Utility::strip(line);
	if (add_history) InputField::push_history(sline);
	try {
		//printf("execute %s\n", line.c_str());
		Pipeline pipeline = Parser::parse(sline);
		pipeline.silent(_mode != Mode::INTERACTIVE);
		pipeline.execute();
	} catch (std::exception& err) {
		fprintf(stderr, CLR(RED, 1, "%s\n"), err.what());
	}
	if (add_history) InputField::pop_history();
}

void Shell::load_rc(void) {
	const char* homedir = getenv("HOME");
	if (homedir != NULL) {
		FILE* stream = fopen((string(homedir) + "/.unbshrc_profile").c_str(), "r");
		if (stream != NULL) {
			char line[1024];
			while (fgets(line, 1024, stream) != NULL) {
				parse_and_execute(string(line), false);
			}
		} else {
			fprintf(stderr, CLR(RED, 1, "~/.unbshrc_profile não encontrado\n"));
		}
		if (stream != NULL) { fclose(stream); }
	}
}

void Shell::load_wd(void) {
	char buffer[1024];
	getcwd(buffer, sizeof(buffer));
	setenv("PWD", buffer, 1);
}
