#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

using Args = std::vector<std::string>;
using Action = std::function<void(const Args&)>;

class Command {
private:
	static inline std::unordered_map<std::string, Action> command_table;
	static inline std::unordered_map<std::string, std::string> alias_table;

	char* check_path(const std::string& filename) const;
	void handle_pid(pid_t pid, char* const* argv) const;
	char** create_argv(void) const;
	void delete_argv(char**) const;

	void child_process_init(void) const;
	void parent_process_init(void) const;
	
	void builtin_init(int*,int*) const;
	void builtin_dispose(int,int) const;

	int* _pipe_in = NULL;
	int* _pipe_out = NULL;

	std::string _file_in = "";
	std::string _file_out = "";

	bool _append = false;
	bool _builtin = false;
	pid_t _pid = 0;

	Args args;

public:
	static void register_command(const std::string& cmdname, const Action& func);
	static void register_alias(const std::string& src, const std::string& tgt);

	inline Command(const Args& args) : args(args) { }
	void execute(void);

	inline void bind_read(int* p) { _pipe_in = p; }
	inline void bind_write(int* p) { _pipe_out = p; }

	inline void set_input_source(const std::string& f) { _file_in = f; }
	inline void set_output_target(const std::string& f) { _file_out = f; }
	inline void set_append(void) { _append = true; }

	inline pid_t pid(void) const { return _pid; }
	inline bool builtin(void) const { return _builtin; }
};

#endif
