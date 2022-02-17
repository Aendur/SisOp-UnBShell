#include <regex>
#include <cstdlib>

#include "utility.h"
using std::string;
using std::vector;

vector<string> Utility::split(const string& str, const string& pattern) {
	std::regex regex(pattern);
	std::smatch result;
	vector<string> res;
	string src = str;
	while (std::regex_search(src, result, regex)) {
		res.push_back(strip(result.prefix()));
		src = result.suffix();
	}
	res.push_back(strip(src));

	// for (const string& s : res) {
	// 	cout << s << ':' << s.length() << endl;
	// }
	return res;
}

string Utility::strip(const string& str) {
	std::smatch result;

	std::regex regex0("(^\\s+)");
	std::regex_search(str, result, regex0);
	int pos0 = result.empty() ? 0 : result.length();

	std::regex regex1("(\\s+$)");
	std::regex_search(str, result, regex1);
	int pos1 = result.empty() ? 0 : result.length();

	string rts = str.substr(pos0, str.length() - pos0 - pos1);
	return rts;
}
