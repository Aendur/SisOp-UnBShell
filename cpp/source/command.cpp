#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "command.h"
#include "utility.h"
#include "defines.h"
#include "shell.h"

using std::string;

void Command::register_command(const string& cmdname, const Action& func) {
	command_table[cmdname] = func;
}

void Command::register_alias(const string& src, const string& tgt) {
	alias_table[tgt] = src;
}

void Command::builtin_init(int * save_out, int * fd_out) const {
	*save_out = dup(STDOUT_FILENO); // save standard output
	
	int mode_flag = O_WRONLY | O_CREAT | (_append ? O_APPEND : O_TRUNC);
	*fd_out = _file_out == "" ? -100 : open(_file_out.c_str(),  mode_flag, S_IWUSR | S_IRUSR);

	if (*fd_out >= 0) {
		if (_pipe_out != NULL) close(_pipe_out[1]); // close pipe
		dup2(*fd_out, STDOUT_FILENO); // write to file
	}
}

void Command::builtin_dispose(int save_out, int fd_out) const {
	if (fd_out >= 0) {
		close(fd_out);
	}
	
	dup2(save_out, STDOUT_FILENO);
	close(save_out);
}


void Command::execute(void) {
	if (args.size() > 0) {
		// replace alias
		if (alias_table.find(args[0]) != alias_table.end()) {
			args.front() = alias_table[args[0]];
		}

		const string& cmd = args[0];
		if (command_table.find(cmd) != command_table.end()) {
			this->_builtin = true;
			int save_out, fd_out;
			builtin_init(&save_out, &fd_out);
			command_table.at(cmd)(args);
			builtin_dispose(save_out, fd_out);
		} else {
			// check and run PATH
			char** argv = create_argv();
			if (argv != NULL) {
				//printf("CMD %s\n", argv[0]);
				this->_pid = fork();
				handle_pid(this->_pid, argv);
				delete_argv(argv);
			} else {
				// otherwise show msg
				fprintf(stderr, CLR(RED, 1, "Comando não encontrado: %s\n"), args[0].c_str());
				this->_builtin = true;
			}
		}
	} else {
		fprintf(stderr, CLR(RED, 1, "Comando inválido.") "\n");
		this->_pid = -1;
	}
}

char** Command::create_argv(void) const {
	char* filename = check_path(args[0]);
	if (filename == NULL) return NULL;

	char** argv = (char**)calloc(args.size() + 1, sizeof(char*));
	argv[0] = filename;

	for (int i = 1; i < (int)args.size(); ++i) {
		const string& arg = args[i];
		argv[i] = strdup(arg.c_str());
		argv[i + 1] = NULL;
	}

	return argv;
}

void Command::delete_argv(char** argv) const {
	for (int i = 0; i < (int)args.size(); ++i) {
		if (argv[i] != NULL) free(argv[i]);
	}
	free(argv);
}

void Command::handle_pid(pid_t pid, char* const* argv) const {
	if (pid == 0) {
		child_process_init();
		execv(argv[0], argv);
	} else {
		parent_process_init();
	}
}

void Command::child_process_init(void) const {
	if (_pipe_in != NULL) {
		close(_pipe_in[1]); // close write of in
		dup2(_pipe_in[0], STDIN_FILENO); // read from pipe
	}
	if (_pipe_out != NULL) {
		close(_pipe_out[0]); // close read of out
		dup2(_pipe_out[1], STDOUT_FILENO); // write to pipe
	}

	int fd_in = _file_in == "" ? -100 : open(_file_in.c_str(), O_RDONLY);
	int mode_flag = O_WRONLY | O_CREAT | (_append ? O_APPEND : O_TRUNC);
	int fd_out = _file_out == "" ? -100 : open(_file_out.c_str(),  mode_flag, S_IWUSR | S_IRUSR);

	if (fd_in >= 0) {
		if (_pipe_in != NULL) close(_pipe_in[0]); // close pipe
		dup2(fd_in, STDIN_FILENO); // read from file
	}
	if (fd_out >= 0) {
		if (_pipe_out != NULL) close(_pipe_out[1]); // close pipe
		dup2(fd_out, STDOUT_FILENO); // write to file
	}
}



void Command::parent_process_init(void) const {
	if (_pipe_in != NULL) {
		close(_pipe_in[0]); // close read of in
	}
	if (_pipe_out != NULL) {
		close(_pipe_out[1]); // close write of out
	}
}


char* Command::check_path(const std::string& filename) const {
	const int SIZE = 1024;
	char* full_path = (char*)malloc(SIZE);

	strncpy(full_path, filename.c_str(), SIZE - 1);
	if (full_path[0] == '.' && full_path[1] == '/')
		if (access(full_path, X_OK) == 0)
			return full_path;

	for (const string& p : Shell::get_paths()) {
		strncpy(full_path, p.c_str(), SIZE - 1);
		strncat(full_path, "/", SIZE - 1);
		strncat(full_path, filename.c_str(), SIZE - 1);

		if (access(full_path, X_OK) == 0) {
			return full_path;
			}
	}

	free(full_path);
	return NULL;
}

