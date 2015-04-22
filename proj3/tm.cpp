// tm.cpp
//
// Deterministic Turing Machine simulator.
//
//   Author: Yuxuan "Ethan" Chen
//     Date: Tuesday, April 21, 2015


#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
using namespace std;


// check if an item is in a container
template <typename T>
bool isInList(const T x, vector<T> l) {
	return (find(l.begin(), l.end(), x) != l.end());
}


// print the configuration of the Turing Machine
void printConfig(string state, vector<string> &input, unsigned long long i) {
	unsigned long long rightmost_nonzero_ind = input.size() - 1;
	while (input[rightmost_nonzero_ind] == " ") { --rightmost_nonzero_ind; }
	cout << "(";
	bool isFirstChar = true;
	for (int j = 0; j < i; ++j) {
		if (isFirstChar) { isFirstChar = false; } else { cout << ","; }
		cout << ((j < input.size()) ? input[j] : " ");
	}
	cout << ")" << state << "(";
	isFirstChar = true;
	for (int j = i; j <= rightmost_nonzero_ind; ++j) {
		if (isFirstChar) { isFirstChar = false; } else { cout << ","; }
		cout << input[j];
	}
	cout << ")" << endl;
}


// meat of the TM simulation
void simTM(map<pair<string, string>, pair<pair<string, string>, string> > t,
	string start, string accept, string reject, vector<string> input) {
	int MAX_ROUNDS = 1000;
	string curr_state = start;
	unsigned long long tape_head = 0;
	while (curr_state != accept && curr_state != reject && MAX_ROUNDS--) {
		printConfig(curr_state, input, tape_head);
		pair<string, string> key = make_pair(curr_state, input[tape_head]);
		if (!t.count(key)) {
			curr_state = reject;
			++tape_head;
			break;
		}
		curr_state = t[key].first.first;
		input[tape_head] = t[key].first.second;
		if (t[key].second == "L") {
			tape_head = (!tape_head) ? 0 : (tape_head - 1);
		} else {
			++tape_head;
			if (tape_head == input.size()) {
				input.push_back(" ");
			}
		}
	}	
	printConfig(curr_state, input, tape_head);
	if (curr_state == accept) { cout << "ACCEPT" << endl; }
	else if (curr_state == reject) { cout << "REJECT" << endl; }
	else { cout << "DID NOT HALT" << endl; } 
}


// split a string into tokens by a delimiter and store them in a vector
vector<string> splitStringByDelimiter(string s, char delim) {
	vector<string> v;
	if (!s.size()) { return v; }
	unsigned long long ind = s.find(delim);
	while (ind < s.size()) {
		v.push_back(s.substr(0, ind));
		s = s.substr(ind + 1);
		ind = s.find(delim);
	}
	v.push_back(s);
	return v;
}


// check if a transition has valid states, characters, and direction
bool isTransitionValid(vector<string> t, vector<string> states,
	vector<string> tapealpha) {
	return (t.size() == 5 && 
		isInList(t[0],    states) && 
		isInList(t[1], tapealpha) && 
		isInList(t[2],    states) && 
		isInList(t[3], tapealpha) &&
		(t[4] == "L" || t[4] == "R"));
}


int main(int argc, char** argv) {
	// check the number of arguments
	if (argc != 2) {
		cout << "usage: ./tm <tm_config> < <input_file> > <output_file>" << endl;
		exit(1);
    }
	// check if the input file exists and is readable
	ifstream inf(argv[1], ios::in);
	if (!inf) {
		cout << "Invalid input file: " << argv[1] << "." << endl;
		exit(1);
	}


    // read the set of states
	string line;
	getline(inf, line);
	if (line[0] != 'Q' || line[1] != ':') {
		cout << "Missing description of the set of states." << endl;
		exit(1);
	}
	vector<string> states = splitStringByDelimiter(line.substr(2), ',');


	// read the input alphabet
	getline(inf, line);
	if (line[0] != 'A' || line[1] != ':') {
		cout << "Missing description of the input alphabet." << endl;
		exit(1);
	}
	vector<string> inputalpha = splitStringByDelimiter(line.substr(2), ',');
	// sort the input alphabet to prepare for the later check of whether the input
	// alphabet is included in the tape alphabet
	sort(inputalpha.begin(), inputalpha.end());


	// if the input alphabet contains symbols that are not exactly one character
	// long or the blank character, then halt
 	for (int i = 0; i < inputalpha.size(); ++i) {
 		if (inputalpha[i].size() != 1) {
 			cout << "Input character must be exactly one character long: " 
 				 << inputalpha[i] << endl;
 			exit(1);
 		} else if (inputalpha[i] == " ") {
 			cout << "Input alphabet cannot contain the blank character." << endl;
 			exit(1);
 		}
 	}
	

    // read the tape alphabet
	getline(inf, line);
	if (line[0] != 'Z' || line[1] != ':') {
		cout << "Missing description of the tape alphabet." << endl;
		exit(1);
	}
	vector<string> tapealpha = splitStringByDelimiter(line.substr(2), ',');
	// sort the tape alphabet to prepare for the later check of whether the input
	// alphabet is included in the tape alphabet
	sort(tapealpha.begin(), tapealpha.end());


	// if the tape alphabet does not contain the blank character, then halt
	if (find(tapealpha.begin(), tapealpha.end(), " ") == tapealpha.end()) {
		cout << "Tape alphabet must contain the blank character." << endl;
		exit(1);
	}
	// if the tape alphabet contains symbols that are not exactly one character,
	// then halt
	for (int i = 0; i < tapealpha.size(); ++i) {
		if (tapealpha[i].size() != 1) {
			cout << "Tape character must be exactly one character long: "
				 << tapealpha[i] << endl;
			exit(1);
		}
	}
	// if the tape alphabet does not contain the input alphabet, then halt
	if (!includes(tapealpha.begin(),  tapealpha.end(),
				 inputalpha.begin(), inputalpha.end())) {
		cout << "The input alphabet is not a part of the tape alphabet." << endl;
		exit(1);
	}


	// read the transition rules
	map<pair<string, string>, pair<pair<string, string>, string> > transitions;
	while (getline(inf, line)) {
		if (line[0] != 'T' || line[1] != ':') { break; }
		vector<string> transition = splitStringByDelimiter(line.substr(2), ',');

		// if any transition has invalid states, tape characters, or directions,
		// then halt
		if (!isTransitionValid(transition, states, tapealpha)) {
			cout << "Invalid transition: " << line.substr(2) << endl;
			exit(1);
		}	

		// if there are two transitions with the same starting states and the
		// same current tape symbol, then halt
		if (transitions.count(make_pair(transition[0], transition[1]))) {
			cout << "Conflicting transition: " << line.substr(2) << endl;
			exit(1);
		}

		transitions[make_pair(transition[0], transition[1])] = 
			make_pair(make_pair(transition[2], transition[3]), transition[4]);
	}


	// read the start state
	if (line[0] != 'S' || line[1] != ':') {
		cout << "Missing description of the start state." << endl;
		exit(1);
	} else if (!isInList(line.substr(2), states)) {
		cout << "Invalid start state: " << line.substr(2) << endl;
		exit(1);
	}
	string start = line.substr(2);


	// read the final states
	getline(inf, line);
	if (line[0] != 'F' || line[1] != ':') {
		cout << "Missing description of the final states." << endl;
		exit(1);
	}
	vector<string> end = splitStringByDelimiter(line.substr(2), ',');
	if (end.size() != 2) {
		cout << "There must be exactly two final states." << endl;
		exit(1);
	}
	// if the final states are the same or any of the final states are not
	// valid, then halt
	if (!isInList(end[0], states)) {
		cout << "Invalid accept state: " << end[0] << endl;
		exit(1);
	} else if (!isInList(end[1], states)) {
		cout << "Invalid reject state: " << end[1] << endl;
		exit(1);
	} else if (end[0] == end[1]) {
		cout << "Accept and reject states must be different." << endl;
		exit(1);
	}


	// read user input and compute results
	int numOfCases = 0;
	cin >> numOfCases;
	getline(cin, line);
	bool isFirstCase = true;
	while (numOfCases--) {
		if (isFirstCase) { isFirstCase = false; } else { cout << endl; }
		getline(cin, line);
		vector<string> input = splitStringByDelimiter(line, ',');
		// if input line is empty, then initialize the input vector with
		// a blank character
		simTM(transitions, start, end[0], end[1], input);
	}
}