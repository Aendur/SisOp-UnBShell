#ifndef __STANDALONE_H__
#define __STANDALONE_H__

#include "shell.h"

//struct termios;

class StandaloneShell : public Shell {
public:
	StandaloneShell(void);
	//~StandaloneShell(void);

	int run(const char * filename) const;
};

#endif
