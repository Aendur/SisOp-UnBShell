#include "scanner.h"
#include <iostream>
#include <vector>
#include <string>

using std::string, std::vector;

int main() {
	vector<string> test_cases = {
		//"ps -ax | grep \"pts|\\?\" | tail | wc | awk  '{print $1 + $2 + $3}' > sum.txt & # comment",
		"alias 'a' 'b'",
		"alias \"a\" \"b\"",
		//"var=a;b;c",
		//"var  =a;b;c",
		//"var=  a;b;c",
		//"var  =  a;b;c",
		//"echo $PATH | wc",
		//"echo \"test # some comment",
		//"/usr/bin/python2 /home/user/bin/user_script-3.py",
		//"ls -l|wc|awk	'{print $1 + $2 + $3}' # & comment",
		//"ls -l|grep \"\"|wc|awk'' # & comment",
		//"ls --color=auto | wc",
		//"ls --color  =  auto | wc",
	};
	for (const string & tc : test_cases) {
		std::cout << "TEST CASE " << tc << std::endl;
		std::cout << "---------" << std::endl;
		try {
			for (auto& i : Scanner::scan(tc, true)) {
				std::cout << (int)i.type << ' ' << i.value << std::endl;
			}
		} catch(std::exception & e) {
			std::cout << e.what() << std::endl;
		}
		std::cout << std::endl;
	}
	return 0;
}

