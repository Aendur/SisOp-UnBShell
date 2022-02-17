#include "interactive.h"
#include "standalone.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc > 1) {
		StandaloneShell shell;
		shell.run(argv[1]);
	} else {
		InteractiveShell shell;
		shell.run();
	}
	return 0;
}

