#include "escapechars.h"

#include <cstdio>
#include <regex>

queue<uint16_t> escapeString(string input) {
	// Create regexae
	regex newline("\\\\n");
	regex null("\\\\0");
	regex hex("\\\\x[0-9A-Za-z]{1,4}");

	// Find matches for newlines and null bytes
	string s = regex_replace(input, newline, "\n");
	s = regex_replace(s, null, "\0");

	// Find matches for raw hex
	smatch matches;
	regex_search(s, matches, hex);

	queue<uint16_t> out_queue;
	if(matches.size() > 0) {
		int match = 0;
		for(int i = 0; i < s.size(); i++) {
			if(i != matches.position(match)) {
				out_queue.push((uint16_t) s[i]);
			} else {
				uint16_t val = strtol(s.c_str() + matches.position(match) + 2, NULL, 16);
				out_queue.push(val);
				i += matches.length(match);
				if(match < (matches.size() - 1)) {
					match++;
				}
			}
		}
	} else {
		for(string::iterator it = s.begin(); it != s.end(); it++) {
			out_queue.push((uint16_t) *it);
		}
	}

	return out_queue;
}
