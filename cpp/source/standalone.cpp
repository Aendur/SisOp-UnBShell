#include <cstdio>

#include "standalone.h"
#include "pipeline.h"
#include "builtin.h"

using std::string;

int StandaloneShell::run(const char * filename) const {
	_mode = Mode::STANDALONE;
	int nline = 0;	
	FILE * stream = fopen(filename, "r");
	char buffer[1024];
	if (stream != NULL) {
		while (fgets(buffer, sizeof(buffer), stream) != NULL) {
			string line = string(buffer);
			if (line.length() == 0) continue;
			if (++nline == 1 && line.length() > 2 && line.substr(0,2) == "#!") continue;
			parse_and_execute(line, true);
		}
	}
	
	if (stream != NULL) { fclose(stream); }
	//Pipeline::process_background_jobs();
	//printf("will return now \n");
	return 0;
}

StandaloneShell::StandaloneShell(void) {
	Command::register_command("exit", [this](const Args&) {});
	Builtin::register_all();
	load_rc();
	load_wd();
}
