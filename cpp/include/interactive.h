#ifndef __INTERACTIVE_H__
#define __INTERACTIVE_H__

#include "shell.h"

struct termios;

class InteractiveShell : public Shell {
public:
	InteractiveShell(void);
	~InteractiveShell(void);

	int run(void) const;

private:
	struct termios* tsettings0 = NULL;
	struct termios* tsettings1 = NULL;
	bool exit = false;


	std::string detect_input(void) const;
	void initialize(void);
	void dispose(void);

	static void handle_ret(int ret);

	void show_loc(const std::string& user_name, const std::string& working_directory) const;
	std::string get_cmdline(void) const;
};

#endif
