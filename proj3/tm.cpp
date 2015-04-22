// tm.cpp
//
// Deterministic Turing Machine simulator.
//
//   Author: Yuxuan "Ethan" Chen
//     Date: Tuesday, April 21, 2015
//
// Checklist for features:
//
// [√] If the input alphabet contains symbols that are not exactly one 
//     character long, then halt
// [√] If the input alphabet contains the blank character, then halt
// [√] If the tape alphabet contains symbols that are not exactly one
//     character long, then halt
// [√] If the tape alphabet does not contain the input alphabet or the blank
//     character, then halt
// [√] If any transition has invalid states, tape characters, or directions,
//     then halt
// [√] If there are two transition rules with the same starting states and the
//     same current tape symbol, halt
// [√] If the start state is an invalid state, then halt
// [] If there are not exactly two final states, then halt
// [] If the final states are the same or any of the final states are not 
//     valid, halt
// [] If there is no transition rule for one config, it should go to the reject state, does not change the tape, and moves right
// [] If the tape head is at the leftmost position, it cannot move left anymore
// [] If the simulator runs for 1000 transitions and still does not halt, print DID NOT HALT

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <utility>
#include <vector>
using namespace std;

/*
// check if a transition can be allowed in an existing list of transitions
bool isNewTransitionValid(map<string, map<pair<string, string>, pair<string, string> > > transitions, vector<string> transition) {
	if (!transitions[transition[0]].size()) return true;
	if (transition[1] == "e" && transition[2] == "e") return false;
	map<pair<string, string>, pair<string, string> > tmpMap = transitions[transition[0]];
	if (tmpMap.count(make_pair("e", "e"))) return false;
	if (tmpMap.count(make_pair(transition[1], transition[2]))) return false;
	if (transition[1] != "e" && transition[2] != "e") {
		return !(tmpMap.count(make_pair("e", transition[2])) || tmpMap.count(make_pair(transition[1], "e")));
	}
	for (map<pair<string, string>, pair<string, string> >::const_iterator it = tmpMap.begin(); it != tmpMap.end(); ++it) {
		if ((transition[1] == "e" && (it->first.second == "e" || (transition[2] == it->first.second && it->first.first != "e"))) ||
			(transition[2] == "e" && (it->first.first == "e"  || (transition[1] == it->first.first  && it->first.second != "e"))))
		return false;
	}
	return true;
}

// print the elements of a stack from the top to the bottom followed by a newline character
void printStack(stack<string> s) {
	if (!s.empty()) {
		cout << " " << s.top();
		s.pop();
		while (!s.empty()) {
			cout << "," << s.top();
			s.pop();
		}
	}
	cout << endl;
}

// assume the input is empty string and see how far we can go in the DPDA
// the didInputFinish variable is basically a switch. when it's true, we will check if the current state is an accept state and stop accordingly
void reachOutWithEmptyStringInput(string & curr_state,
	stack<string> & curr_stack, map<string, map<pair<string, string>,
	pair<string, string> > > transitions, vector<string> end, 
	bool didInputFinish = false) {
	while (transitions[curr_state].count(make_pair("e", "e")) || 
		  (!curr_stack.empty() && transitions[curr_state].count(make_pair("e", curr_stack.top())))) {
		if (transitions[curr_state].count(make_pair("e", "e"))) {
			cout << curr_state << "; e; e; " << transitions[curr_state][make_pair("e", "e")].first << ";";
			string newElem = transitions[curr_state][make_pair("e", "e")].second;
			if (newElem != "e") curr_stack.push(newElem);
			curr_state = transitions[curr_state][make_pair("e", "e")].first;
		} else {
			string stackTop = curr_stack.top();
			cout << curr_state << "; e; " << stackTop << "; " << transitions[curr_state][make_pair("e", stackTop)].first << ";";
			curr_stack.pop();
			string newElem = transitions[curr_state][make_pair("e", stackTop)].second;
			if (newElem != "e") curr_stack.push(newElem);
			curr_state = transitions[curr_state][make_pair("e", stackTop)].first;
        }
		printStack(curr_stack);
		if (didInputFinish && isInList(curr_state, end)) {
			cout << "ACCEPT" << endl;
			return;
		}
	}
	if (didInputFinish) cout << "REJECT" << endl;
}

// meat of the DPDA analyses
void analyzeDPDA(map<string, map<pair<string, string>, pair<string, string> > > transitions, string start, vector<string> end, vector<string> input) {
	string curr_state = start;
	stack<string> curr_stack;
	reachOutWithEmptyStringInput(curr_state, curr_stack, transitions, end);

	for (int i = 0; i < input.size(); ++i) {
		if (transitions[curr_state].count(make_pair(input[i], "e"))) {
			cout << curr_state << "; " << input[i] << "; e; " << transitions[curr_state][make_pair(input[i], "e")].first << ";";
			string newElem = transitions[curr_state][make_pair(input[i], "e")].second;
			if (newElem != "e") curr_stack.push(newElem);
			curr_state = transitions[curr_state][make_pair(input[i], "e")].first;
		} else if (!curr_stack.empty() && transitions[curr_state].count(make_pair(input[i], curr_stack.top()))) {
			string stackTop = curr_stack.top();
			cout << curr_state << "; " << input[i] << "; " << stackTop << "; " << transitions[curr_state][make_pair(input[i], stackTop)].first << ";";
			curr_stack.pop();
			string newElem = transitions[curr_state][make_pair(input[i], stackTop)].second;
			if (newElem != "e") curr_stack.push(newElem);
			curr_state = transitions[curr_state][make_pair(input[i], stackTop)].first;
		} else {
			cout << "REJECT" << endl;
			return;
		}
		printStack(curr_stack);
		if (i != input.size() - 1) {
			reachOutWithEmptyStringInput(curr_state, curr_stack, transitions, end);
			continue;
		}
		reachOutWithEmptyStringInput(curr_state, curr_stack, transitions, end, true);
	}
}
*/


// check if an item is in a container
template <typename T>
bool isInList(const T x, vector<T> l) {
	return (find(l.begin(), l.end(), x) != l.end());
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
		isInList(t[0], states) && 
		isInList(t[1], tapealpha) && 
		isInList(t[2], states) && 
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
	/*
	// read user input and output results
	int numOfCases = 0;
	cin >> numOfCases;
	getline(cin, line);
	bool isFirstCase = true;
	while (numOfCases--) {
		if (isFirstCase) { isFirstCase = false; } else { cout << endl; }
		getline(cin, line);
		vector<string> input = splitStringByDelimiter(line, ',');
		analyzeDPDA(transitions, start, end, input);
	}
	*/
}