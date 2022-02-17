#include <vector>
#include <cstdio>
#include <unordered_map>
#include <unistd.h>

#include "builtin.h"
#include "input_field.h"
#include "command.h"
#include "shell.h"
#include "defines.h"
#include "utility.h"

using std::string;
using std::vector;
using std::unordered_map;

void Builtin::register_all(void) {
	Command::register_command("alias", alias);
	Command::register_command("ver", ver);
	Command::register_command("cd", cd);
	Command::register_command("history", history);
}

void Builtin::alias(const Args & args) {
	//for(const auto & i : args) { printf("%s\n", i.c_str()); }
	if (args.size() == 3) {
		// && args[1][0] == '"' && args[2][0] == '"'
		Command::register_alias(args[1], args[2]);
	} else {
		fprintf(stderr, CLR(RED, 1, "alias: formato inválido. Uso: alias \"comando antigo\" \"comando novo\"\n"));
	}
}


unordered_map<string, string> months = {
	{"Jan","01"}, {"Feb","02"}, {"Mar","03"}, {"Apr","04"},
	{"May","05"}, {"Jun","06"}, {"Jul","07"}, {"Aug","08"},
	{"Sep","09"}, {"Oct","10"}, {"Nov","11"}, {"Dec","12"},
};
void Builtin::ver(const Args&) {
	const auto & date = Utility::split(__DATE__, " ");
	
	printf("UnBShell\n");
	printf("\t      Versão  " VERSION "\n");
	printf("\t Atualização  " VDATE "\n");
	printf("\t  Compilação  %s/%s/%s às " __TIME__ "\n", date[1].c_str(), months[date[0]].c_str(), date[2].c_str());
	printf("\t       Autor  " AUTHOR " - " AUTHID "\n");
}

void Builtin::cd(const Args& args) {
	if (args.size() > 1) {
		chdir(args[1].c_str());
		Shell::load_wd();
	} else {
		const char* homedir = getenv("HOME");
		if (homedir != NULL) {
			chdir(homedir);
			Shell::load_wd();
		}
	}
}

void Builtin::history(const Args& args) {
	if (args.size() == 1) {
		if (InputField::history().size() <= 10) {
			int i = 0;
			for (const string& cmd : InputField::history()) {
				printf("%2d  %s\n", ++i, cmd.c_str());
			}
		} else {
			int i = 0;
			for (const string& cmd : InputField::history()) {
				if (i == 0) { ++i; continue; }
				printf("%2d  %s\n", i++, cmd.c_str());
			}
		}
	} else if (args.size() > 1) {
		int i;
		try {
			i = stoi(args.at(1)) - 1;
		} catch (std::exception& e) {
			fprintf(stderr, CLR(RED, 1, "Argumento inválido: %s") "\n", args[1].c_str());
			i = -1;
		}

		if (0 <= i && i < (int)InputField::history().size()) {
			InputField::unpush_history();
			Shell::parse_and_execute(InputField::history().at(i), true);
		} else {
			fprintf(stderr, CLR(RED, 1, "Histórico não encontrado: %s") "\n", args[1].c_str());
		}
	}
}


