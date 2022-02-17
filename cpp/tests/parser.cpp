#include "parser.h"
#include <iostream>

int main() {
	try {
		Parser::parse("ps -ax | grep \"pts|\\?\" | tail | wc | awk  '{print $1 + $2 + $3}' > sum.txt & # comment");
		//for (auto & i : tokens) { std::cout << (int) i.type << ' ' << i.value << std::endl; }
	} catch(std::exception & e) { std::cout << e.what() << std::endl; }
	// try {
	// 	Parser::parse("var=a;b;c");
	// 	//for (auto & i : tokens) { std::cout << (int) i.type << ' ' << i.value << std::endl; }
	// } catch(std::exception & e) { std::cout << e.what() << std::endl; }
	// try {
	// 	Parser::parse("echo $PATH | wc");
	// 	//for (auto & i : tokens) { std::cout << (int) i.type << ' ' << i.value << std::endl; }
	// } catch(std::exception & e) { std::cout << e.what() << std::endl; }
	// try {
	// 	Parser::parse("echo \"test # some comment");
	// 	//for (auto & i : tokens) { std::cout << (int) i.type << ' ' << i.value << std::endl; }
	// } catch(std::exception & e) { std::cout << e.what() << std::endl; }
	// try {
	// 	Parser::parse("/usr/bin/python2 /home/user/bin/user_script-3.py");
	// 	//for (auto & i : tokens) { std::cout << (int) i.type << ' ' << i.value << std::endl; }
	// } catch(std::exception & e) { std::cout << e.what() << std::endl; }
	// try {
	// 	Parser::parse("ls -l|wc|awk	'{print $1 + $2 + $3}' # & comment");
	// 	//for (auto & i : tokens) { std::cout << (int) i.type << ' ' << i.value << std::endl; }
	// } catch(std::exception & e) { std::cout << e.what() << std::endl; }
	// try {
	// 	Parser::parse("ls -l|grep \"\"|wc|awk'' # & comment");
	// 	//for (auto & i : tokens) { std::cout << (int) i.type << ' ' << i.value << std::endl; }
	// } catch(std::exception & e) { std::cout << e.what() << std::endl; }
	return 0;
}

