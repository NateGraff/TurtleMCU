#include "escapechars.h"

#include <iostream>
#include <cstdio>
#include <regex>

using namespace std;

queue<uint16_t> escapeString(string input) {
	// Create regexae
	regex newline("\\\\n");
	regex null("\\\\0");

	// Find matches for newlines and null bytes
	string s = regex_replace(input, newline, "\n");
	s = regex_replace(s, null, "\0");

	queue<uint16_t> out_queue;

	enum {
		IDLE,
		X,
		NUM1,
		NUM2,
		NUM3,
		NUM4
	} state = IDLE;

	char temp[5];

	for(int i = 0; i < s.length(); i++) {
		switch(state) {
			case IDLE:
				if(s[i] == '\\') {
					state = X;
				} else {
					out_queue.push((uint16_t) s[i]);
				}
				break;
			case X:
				if(s[i] == 'x') {
					state = NUM1;
				} else {
					state = IDLE;
					out_queue.push((uint16_t) s[i-1]);
					out_queue.push((uint16_t) s[i]);
				}
				break;
			case NUM1:
				if((s[i] >= '0' && s[i] <= '9') ||
				   (s[i] >= 'A' && s[i] <= 'Z') ||
				   (s[i] >= 'a' && s[i] <= 'z')) {
					state = NUM2;
				} else {
					state = IDLE;
					out_queue.push((uint16_t) s[i-2]);
					out_queue.push((uint16_t) s[i-1]);
					out_queue.push((uint16_t) s[i]);
				}
				break;
			case NUM2:
				if((s[i] >= '0' && s[i] <= '9') ||
				   (s[i] >= 'A' && s[i] <= 'Z') ||
				   (s[i] >= 'a' && s[i] <= 'z')) {
					state = NUM3;
				} else {
					state = IDLE;
					temp[0] = s[i-1];
					temp[1] = 0;
					uint16_t val = strtol(temp, NULL, 16);
					out_queue.push(val);
					i--;
				}
				break;
			case NUM3:
				if((s[i] >= '0' && s[i] <= '9') ||
				   (s[i] >= 'A' && s[i] <= 'Z') ||
				   (s[i] >= 'a' && s[i] <= 'z')) {
					state = NUM4;
				} else {
					state = IDLE;
					temp[0] = s[i-2];
					temp[1] = s[i-1];
					temp[2] = 0;
					uint16_t val = strtol(temp, NULL, 16);
					cout << val << endl;
					out_queue.push(val);
					i--;
				}
				break;
			case NUM4:
				if((s[i] >= '0' && s[i] <= '9') ||
				   (s[i] >= 'A' && s[i] <= 'Z') ||
				   (s[i] >= 'a' && s[i] <= 'z')) {
					temp[0] = s[i-3];
					temp[1] = s[i-2];
					temp[2] = s[i-1];
					temp[3] = s[i];
					temp[4] = 0;
					uint16_t val = strtol(temp, NULL, 16);
					out_queue.push(val);
				} else {
					temp[0] = s[i-3];
					temp[1] = s[i-2];
					temp[2] = s[i-1];
					temp[3] = 0;
					uint16_t val = strtol(temp, NULL, 16);
					out_queue.push(val);
					i--;
				}
				state = IDLE;
				break;
		}
	}

	return out_queue;
}
