#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "input_field.h"

InputField::InputField(void) {
	this->history_position = (int)_history.size();
	this->buffer = (char*)calloc(sizeof(char), buffer_size);
	this->control_table = {
		/*  up     */ { "\033[A"  , [this]() { cursor_move_v(-1);  } },
		/*  down   */ { "\033[B"  , [this]() { cursor_move_v(1); } },
		/*  right  */ { "\033[C"  , [this]() { cursor_move_h(1);  } },
		/*  left   */ { "\033[D"  , [this]() { cursor_move_h(-1); } },
		/*  insert */ { "\033[2~" , [this]() { } },
		/*  delete */ { "\033[3~" , [this]() { cursor_delete(); } },
		/*  home   */ { "\033[H"  , [this]() { cursor_move_h(-cursor_position); } },
		/*  end    */ { "\033[F"  , [this]() { cursor_move_h(length - cursor_position); } },
		//{ "\033[5~" , // PGUP },
		//{ "\033[6~" , // PGDOWN },
	};

}

InputField::~InputField(void) {
	free(this->buffer);
}

std::string InputField::str(void) const {
	return std::string(buffer);
}

void InputField::finish(void) {
	printf("\n");
	_finished = true;
}

void InputField::push_history(const std::string& line) {
	if (line.length() > 0) {
		_history.push_back(line);
	}
}

void InputField::unpush_history(void) {
	_history.pop_back();
}

void InputField::pop_history(void) {
	if (_history.size() > 10) {
		_history.pop_front();
	}
}

void InputField::add(char c) {
	if (cursor_position == length) {
		printf("%c", c);
		increase_length(1);
		buffer[cursor_position++] = c;
		buffer[cursor_position] = 0;
	} else {
		char* suffix_start = &buffer[cursor_position];
		char* suffix = (char*)malloc(sizeof(char) * buffer_size);
		strcpy(suffix, suffix_start);

		printf("%c\033[s%s\033[u", c, suffix);
		increase_length(1);
		buffer[cursor_position++] = c;
		buffer[cursor_position] = 0;

		strcat(buffer, suffix);
		free(suffix);
	}
}

void InputField::increase_length(int l) {
	length += l;
	realloc_buffer(length + 1);
	//if (++length >= buffer_size - 1) {
	//	buffer_size <<= 1;
	//	buffer = (char*)realloc(buffer, sizeof(char) *  buffer_size);
	//}
}

void InputField::realloc_buffer(int len) {
	if (len > buffer_size) {
		buffer_size <<= 1;
		buffer = (char*)realloc(buffer, sizeof(char) * buffer_size);
	}
}

void InputField::backspace(void) {
	if (cursor_position > 0) {
		int suffix_length = length - cursor_position;
		char* suffix_start = &buffer[cursor_position];

		--length;
		cursor_move_h(-1);
		printf("\033[0K");
		buffer[cursor_position] = 0;

		if (suffix_length > 0) {
			char* suffix = (char*)malloc(sizeof(char) * buffer_size);
			strcpy(suffix, suffix_start);

			printf("\033[s%s\033[u", suffix);
			//cout << suffix;
			//cursor_position += suffix_length;
			//cursor_position += cursor_move_h(-suffix_length);
			strcat(buffer, suffix);

			free(suffix);
		}
	}
}

void InputField::control_sequence(char* buffer) {
	if (control_table.find(buffer) != control_table.end()) {
		control_table.at(buffer)();
	} else {
		fprintf(stderr, "UNHANDLED CTL %s", buffer + 1);
	}
}


int InputField::cursor_move_h(int n) {
	if (n < 0) {
		if (cursor_position >= -n) {
			printf("\033[%dD", -n);
			cursor_position += n;
			return n;
		}
	} else if (n > 0) {
		if (cursor_position <= length - n) {
			printf("\033[%dC", n);
			cursor_position += n;
			return n;
		}
	}
	return 0;
}

int InputField::cursor_move_v(int n) {
	if (n < 0) {
		if (history_position > 0) {
			history_position += n;
			load_history(history_position);
			return n;
		}
	} else if (n > 0) {
		if (history_position < (int)_history.size()) {
			history_position += n;
			load_history(history_position);
			return n;
		}
	}
	return 0;
}


void InputField::load_history(int i) {
	if (0 <= i && i < (int)_history.size()) {
		// adjust main buffer as needed
		int new_length = _history[i].length();
		while (buffer_size < new_length) { realloc_buffer(new_length); }

		// copy history entry to main buffer
		const char* hbuf = _history[i].c_str();
		strcpy(buffer, hbuf);

		// adjust cursor and display
		cursor_move_h(-cursor_position);
		printf("\033[0K%s", buffer);
		length = strlen(buffer);
		cursor_position = length;
	} else if (i == (int)_history.size()) {
		buffer[0] = 0;
		cursor_move_h(-cursor_position);
		printf("\033[0K");
		length = 0;
		cursor_position = 0;
	}
}

void InputField::cursor_delete(void) {
	if (cursor_position < length) {
		int suffix_length = length - (cursor_position + 1);
		char* suffix_start = &buffer[cursor_position + 1];

		--length;
		buffer[cursor_position] = 0;
		printf("\033[0K");

		if (suffix_length > 0) {
			char* suffix = (char*)malloc(sizeof(char) * buffer_size);
			strcpy(suffix, suffix_start);

			printf("\033[s%s\033[u", suffix);
			strcat(buffer, suffix);

			free(suffix);
		}
	}
}

