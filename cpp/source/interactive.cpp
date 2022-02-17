#include <unistd.h> // read
#include <termios.h> // termios
#include <csignal>

#include "interactive.h"
#include "input_field.h"
#include "command.h"
#include "pipeline.h"
#include "builtin.h"
#include "defines.h"

using std::string;

static volatile sig_atomic_t interrupted = 0;
void sig_int_handler(int) {
	//interrupted = 1;
	char buffer[] = CLR(RED, 1, "INT\033[3D");
	write(STDERR_FILENO, buffer, sizeof(buffer));
}

int InteractiveShell::run(void) const {
	signal(SIGINT, sig_int_handler);

	while (!exit) {
		_mode = Mode::INTERACTIVE;
		Pipeline::process_background_jobs();
		show_loc(get_user(), get_working_directory());
		string line = detect_input();
		if (line.length() == 0) continue;
		parse_and_execute(line, true);
	}
	return 0;
}

InteractiveShell::InteractiveShell(void) {
	this->initialize();
	Command::register_command("exit", [this](const Args&) { exit = true; });
	Builtin::register_all();
	load_rc();
	load_wd();
}

InteractiveShell::~InteractiveShell(void) {
	this->dispose();
}

void InteractiveShell::initialize(void) {
	setbuf(stdout, NULL);

	this->tsettings0 = (struct termios*)malloc(sizeof(struct termios));
	this->tsettings1 = (struct termios*)malloc(sizeof(struct termios));
	tcgetattr(0, this->tsettings0);
	*(this->tsettings1) = *(this->tsettings0);
	this->tsettings1->c_lflag &= ~ICANON;
	this->tsettings1->c_lflag &= ~ECHO;
	//this->tsettings1->c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, this->tsettings1);
}


//#include <iostream>
void InteractiveShell::dispose(void) {
	tcsetattr(0, TCSANOW, this->tsettings0);
	free(this->tsettings0);
	free(this->tsettings1);
	this->tsettings0 = NULL;
	this->tsettings1 = NULL;
	//std::cout << "disposed" << std::endl;
}

std::string InteractiveShell::detect_input(void) const {
	const int BUFSIZE = 32;
	char buffer[BUFSIZE];
	int nread = 0;

	InputField input;

	while (!input.finished()) {
		nread = read(STDIN_FILENO, buffer, BUFSIZE - 1);
		buffer[nread] = 0;

		switch (buffer[0]) {
		case '\n': input.finish(); break;
		case '\x7F': input.backspace(); break;
		case '\033': input.control_sequence(buffer); break;
		case '\t': break;
		default: input.add(buffer[0]); break;
		}
	}
	return  input.str();
}


void InteractiveShell::show_loc(const string& user_name, const string& working_directory) const {
	printf(CLR(WHI, 1, "UnBsh") "-" CLR(GRE, 1, "%s") "-" CLR(BLU, 1, "%s") "-> ", user_name.c_str(), working_directory.c_str());
}

//string InteractiveShell::get_cmdline(void) const {
//	string line;
//	std::getline(cin, line);
//	return line;
//}