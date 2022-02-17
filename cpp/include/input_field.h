#ifndef __INPUT_FIELD_H__
#define __INPUT_FIELD_H__

#include <string>
#include <unordered_map>
#include <functional>
#include <deque>

class InputField {
private:
	inline static std::deque<std::string> _history = std::deque<std::string>();

	int length = 0;
	int buffer_size = 1024;
	int cursor_position = 0;
	char* buffer = NULL;
	bool _finished = false;

	int history_position;
	std::unordered_map<std::string, std::function<void()>> control_table;

	void increase_length(int l);
	void realloc_buffer(int len);
	int cursor_move_v(int n);
	int cursor_move_h(int n);
	void cursor_delete(void);
	void load_history(int i);


public:
	inline static const std::deque<std::string>& history(void) { return _history; }
	static void push_history(const std::string& line);
	static void unpush_history(void);
	static void pop_history(void);

	InputField(void);
	~InputField(void);

	inline bool finished(void) { return _finished; }

	std::string str(void) const;

	void add(char c);
	void finish(void);
	void backspace(void);
	void control_sequence(char* buffer);
	//void cursor_move(cursor_dir dir);
};

#endif
