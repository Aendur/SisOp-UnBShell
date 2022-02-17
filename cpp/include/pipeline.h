#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <string>
#include <vector>
#include <list>

class Command;

class Pipeline {
private:
	inline static int job_id = 0;
	inline static std::list<Pipeline> background_jobs = std::list<Pipeline>();
	static void register_background_job(Pipeline &);

	std::vector<Command> pipeline;
	std::string line;
	bool _background = false;
	bool _empty = false;
	int  _id = -1;
	bool _silent = false;
	bool create_pipes(std::vector<int[2]>&);
	bool running(void) const;
public:
	static void process_background_jobs(void);

	inline Pipeline(void) : _empty(true) {}
	inline Pipeline(const std::string& line) : line(line) {}
	void add_command(const Command & cmd);
	bool execute(void);

	inline void set_background(void) { _background = true; }
	inline bool is_background(void) const { return _background; }
	inline void silent(bool _) { _silent = _; }
};

#endif
