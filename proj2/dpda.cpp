/* dpda.cpp
 * Deterministic Pushdown Automata simulator.
 *   Author: Yuxuan "Ethan" Chen
 *     Date: Monday, March 30, 2015
 */

#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <utility>
#include <vector>
using namespace std;

// check if an item is in a container
template <typename T>
bool isInList(const T x, vector<T> l) {
	return (find(l.begin(), l.end(), x) != l.end());
}

// check if an individual transition is valid in terms of having the right states and alphabets
bool isTransitionValid(vector<string> t, vector<string> states, vector<string> inputalpha, vector<string> stackalpha) {
	return (isInList(t[0], states) && 
		(t[1] == "e" || isInList(t[1], inputalpha)) && 
		(t[2] == "e" || isInList(t[2], stackalpha)) && 
		    isInList(t[3], states) &&
		(t[4] == "e" || isInList(t[4], stackalpha)));
}

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
void reachOutWithEmptyStringInput(string & curr_state, stack<string> & curr_stack, map<string, map<pair<string, string>, pair<string, string> > > transitions, vector<string> end, bool didInputFinish = false) {
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

int main(int argc, char** argv) {

	// check the number of arguments
	if (argc != 2) {
		cout << "usage: ./dpda <dpda_config>  <  <input_file>  >  <output_file>" << endl;
		exit(1);
    }

	// check if the input file exists and is readable
	ifstream inf(argv[1], ios::in);
	if (!inf) {
		cout << "Invalid input file: %s" << argv[1] << endl;
		exit(1);
	}

    // read the set of states
	string line;
	inf >> line;
	if (line[0] != 'Q' || line[1] != ':') {
		cout << "Missing description of the set of states." << endl;
		exit(1);
	}
	vector<string> states = splitStringByDelimiter(line.substr(2), ',');

	// read the input alphabet
	inf >> line;
	if (line[0] != 'A' || line[1] != ':') {
		cout << "Missing description of the input alphabet." << endl;
		exit(1);
	}
	vector<string> inputalpha = splitStringByDelimiter(line.substr(2), ',');
	
    // read the stack alphabet
	inf >> line;
	if (line[0] != 'Z' || line[1] != ':') {
		cout << "Missing description of the stack alphabet." << endl;
		exit(1);
	}
	vector<string> stackalpha = splitStringByDelimiter(line.substr(2), ',');

	// read the transition rules
	map<string, map<pair<string, string>, pair<string, string> > > transitions;
	while (inf >> line) {
		if (line[0] != 'T' || line[1] != ':') { break; }
		vector<string> transition = splitStringByDelimiter(line.substr(2), ',');
		
		if (!isTransitionValid(transition, states, inputalpha, stackalpha)) {
			cout << "Invalid transition: " << line.substr(2) << endl;
			exit(1);
		}	
		
		if (!transitions.count(transition[0])) {
			map<pair<string, string>, pair<string, string> > tmpMap;
			transitions[transition[0]] = tmpMap;
		} 
		
		if (!isNewTransitionValid(transitions, transition)) {
			cout << "Invalid transition: " << line.substr(2) << endl;
			exit(1);
		}

		transitions[transition[0]][make_pair(transition[1], transition[2])] = make_pair(transition[3], transition[4]);
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

	// read the list of accepted states
	inf >> line;
	if (line[0] != 'F' || line[1] != ':') {
		cout << "Missing description of the accepted states." << endl;
		exit(1);
	}
	vector<string> end = splitStringByDelimiter(line.substr(2), ',');
	for (int i = 0; i < end.size(); ++i) {
		if (!isInList(end[i], states)) {
			cout << "Invalid accepted state: " << end[i] << endl;
			exit(1);
		}
	}

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
}