#include <unistd.h>
#include <sys/wait.h>

#include "pipeline.h"
#include "command.h"
#include "defines.h"

using std::vector;
using std::string;

void Pipeline::add_command(const Command & cmd) {
	pipeline.push_back(cmd);
}

bool Pipeline::create_pipes(vector<int[2]>& pipes) {
	int num_pipes = pipeline.size() - 1;
	pipes = vector<int[2]>(num_pipes);
	
	for (int i = 0; i < num_pipes; ++i) {
		if (pipe(pipes[i]) == -1) {
			if(!_silent)
				fprintf(stderr, CLR(RED, 1, "Erro ao criar o pipe.") "\n");
			return false;
		} else {
			pipeline[i].bind_write(pipes[i]);
			pipeline[i + 1].bind_read(pipes[i]);
		}
	}
	return true;
}

bool Pipeline::execute(void) {
	if (pipeline.size() > 0) {
		vector<int[2]> pipes;
		bool created = create_pipes(pipes);
		if (!created) return false;
		for (Command& cmd : pipeline) { cmd.execute(); }

		if (_background) {
			// register children to finish if it is not a background job
			register_background_job(*this);
		} else {
			// wait for children to finish if it is not a background job
			for (const Command& cmd : pipeline) {
				if (!cmd.builtin())
					waitpid(cmd.pid(), NULL, 0);
			}
		}
	}
	return true;
}

bool Pipeline::running(void) const {
	bool any_running = false;
	for (const Command& cmd : pipeline) {
		if (!cmd.builtin()) {
			int status;
			pid_t pid = waitpid(cmd.pid(), &status, WNOHANG | WUNTRACED);
			any_running |= (pid == 0);
		}

		// printf("pid %d\n", pid);
		// printf("WIFEXITED %d\n", WIFEXITED(status));
		// printf("WIFSIGNALED %d\n", WIFSIGNALED(status));
		// printf("WIFSTOPPED %d\n", WIFSTOPPED(status));
		
		//pid == -1 // error
		//pid == 0 // running
		//pid > 0 // finished
	}
	return any_running;
}


void Pipeline::register_background_job(Pipeline & job) {
	job._id = ++job_id;
	background_jobs.push_back(job);
	if (!job._silent)
		fprintf(stderr, "Processo em background [%d] foi iniciado\n", job._id);
}

void Pipeline::process_background_jobs(void) {
	//background_jobs.erase_if([&] (auto& job) { return job.running() == false; } );
	std::list<Pipeline>::iterator job = background_jobs.begin();
	while(job != background_jobs.end()) {
		if (job->running() == false) {
			if (!job->_silent)
				fprintf(stderr, "Processo em background [%d] [executado] \"%s\"\n",job->_id, job->line.c_str());
			background_jobs.erase(job++);
		} else {
			++job;
		}
	}
}


/*
WIFEXITED(status)
returns true if the child terminated normally, that is, by calling exit(3) or _exit(2), or by returning from main().
WEXITSTATUS(status)
returns the exit status of the child. This consists of the least significant 8 bits of the status argument that the child specified in a call to exit(3) or _exit(2) or as the argument for a return statement in main(). This macro should only be employed if WIFEXITED returned true.
WIFSIGNALED(status)
returns true if the child process was terminated by a signal.
WTERMSIG(status)
returns the number of the signal that caused the child process to terminate. This macro should only be employed if WIFSIGNALED returned true.
WCOREDUMP(status)
returns true if the child produced a core dump. This macro should only be employed if WIFSIGNALED returned true. This macro is not specified in POSIX.1-2001 and is not available on some UNIX implementations (e.g., AIX, SunOS). Only use this enclosed in #ifdef WCOREDUMP ... #endif.
WIFSTOPPED(status)
returns true if the child process was stopped by delivery of a signal; this is only possible if the call was done using WUNTRACED or when the child is being traced (see ptrace(2)).
WSTOPSIG(status)
returns the number of the signal which caused the child to stop. This macro should only be employed if WIFSTOPPED returned true.
WIFCONTINUED(status)
(since Linux 2.6.10) returns true if the child process was resumed by delivery of SIGCONT.
*/
