#ifndef __SHELL_H__
#define __SHELL_H__

#include <string>
#include <vector>

class Command;
class Pipeline;

class Shell {
public:
	enum class Mode {
		UNDEFINED,
		STANDALONE,
		INTERACTIVE
	};
protected:
	//Shell(void);
	static void load_rc(void);
	static inline Mode _mode = Mode::UNDEFINED;

public:
	static void load_wd(void);
	static void parse_and_execute(const std::string& line, bool add_history);
	static std::string get_user(void);
	static std::string get_working_directory(void);
	static std::string get_home(void);
	static std::string get_path(void);
	static std::vector<std::string> get_paths(void);
};

#endif
