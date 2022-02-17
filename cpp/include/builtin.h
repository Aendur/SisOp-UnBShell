#ifndef __BUILTIN_H__
#define __BUILTIN_H__

#include "command.h"

namespace Builtin {
	void register_all(void);
	void history(const Args &);
	void cd(const Args &);
	void ver(const Args &);
	void alias(const Args &);
	//Action history;
}


#endif