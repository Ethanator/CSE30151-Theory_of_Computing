/* nfa.cpp
 * First part of the multi-step automata simulator.
 *   Author: Yuxuan "Ethan" Chen
 *     Date: Sunday, February 8, 2015
 */

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

// check if an item is in the vector
template <typename T>
bool isInList(const T x, vector<T> l) {
	return (find(l.begin(), l.end(), x) != l.end());
}

template <typename T>
bool isInList(const T x, set<T> l) {
	return (find(l.begin(), l.end(), x) != l.end());
}

// check if a transition is valid
bool isTransitionValid(vector<string> t, vector<string> alpha, vector<string> states) {
	return (isInList(t[0], states) && (t[1] == "e" || isInList(t[1], alpha)) && isInList(t[2], states));
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

// get all the possible states that can be reached from here
set<string> reachable_from_here(set<string> here, string input, map<pair<string, string>, vector<string> > transitions) {
	set<string> next;
	for (set<string>::const_iterator it = here.begin(); it != here.end(); ++it) {
		if (transitions.count(make_pair(*it, input))) {
			vector<string> reachables = transitions[make_pair(*it, input)];
			for (int i = 0; i < reachables.size(); ++i) {
				next.insert(reachables[i]);
			}
		}
	}
	return next;
}

// count number of duplicates between two sets
int duplicatesOfTwoSets(set<string> a, set<string> b) {
	if (a.size() < b.size()) {
		set<string> t = a;
		a = b;
		b = t;
	}
	int initialSize = a.size();
	for (set<string>::const_iterator it = b.begin(); it != b.end(); ++it) {
		a.insert(*it);
	}	
	return initialSize + b.size() - a.size();
}

// merge two sets
set<string> mergeTwoSets(set<string> a, set<string> b) {
	for (set<string>::const_iterator it = b.begin(); it != b.end(); ++it) {
		a.insert(*it);
	}
	return a;
}

// print a set with a specified delimiter
string printSetByDelim(set<string> s, char delim) {
	bool isFirstCase = true;
	string res = "";
	for (set<string>::const_iterator it = s.begin(); it != s.end(); ++it) {
		if (isFirstCase) { isFirstCase = false; } else { res += delim; } 
		res += (*it);
	}
	return res;
}

// meat of the NFA analyses
void analyzeNFA(vector<string> alphabet, vector<string> states, map<pair<string, string>, vector<string> > transitions, string start, vector<string> end, vector<string> input) {
	set<string> reachable_so_far; // all the states that can be reached by the input so far
	reachable_so_far.insert(start);
	cout << "; " << start << endl;
	for (int i = 0; i < input.size(); ++i) {
		if (!isInList(input[i], alphabet)) {
			cout << "Invalid input: " << input[i] << endl;
			exit(1);
		}
		// deal with empty string input 
		set<string> new_reachable = reachable_from_here(reachable_so_far, "e", transitions);
		while (duplicatesOfTwoSets(new_reachable, reachable_so_far) < new_reachable.size()) {
			reachable_so_far = mergeTwoSets(new_reachable, reachable_so_far);
			new_reachable = reachable_from_here(reachable_so_far, "e", transitions);
		}
		// deal with the real string input
		new_reachable = reachable_from_here(reachable_so_far, input[i], transitions);
		reachable_so_far = new_reachable;
		// deal with empty string again
		new_reachable = reachable_from_here(reachable_so_far, "e", transitions);
		while (duplicatesOfTwoSets(new_reachable, reachable_so_far) < new_reachable.size()) {
			reachable_so_far = mergeTwoSets(new_reachable, reachable_so_far);
			new_reachable = reachable_from_here(reachable_so_far, "e", transitions);
		}
		cout << input[i] << "; " << printSetByDelim(reachable_so_far, ',') << endl; 
	}
	for (int i = 0; i < end.size(); ++i) {
		if (isInList(end[i], reachable_so_far)) {
			cout << "ACCEPT" << endl;
			return;
		} 
	}
	cout << "REJECT" << endl;
}

// main program
int main(int argc, char** argv) {
	// check if there is one and exactly one parameter
	if (argc != 2) {
		cout << "usage: ./nfa <nfa_description> < <input> > <output>" << endl;
		exit(1);
    }

	// exit program if input file is invalid
	ifstream inf(argv[1], ios::in);
	if (!inf) {
		cout << "Invalid input file: %s" << argv[1] << endl;
		exit(1);
	}

	// read the alphabet
	string line;
	inf >> line;
	if (line[0] != 'A' || line[1] != ':') {
		cout << "Invalid description of the alphabet." << endl;
		exit(1);
	}
	vector<string> alphabet = splitStringByDelimiter(line.substr(2), ',');
	
	// read the states
	inf >> line;
	if (line[0] != 'Q' || line[1] != ':') {
		cout << "Invalid description of the states." << endl;
		exit(1);
	}
	vector<string> states = splitStringByDelimiter(line.substr(2), ',');

	// read the transitions
	map<pair<string, string>, vector<string> > transitions;
	while (inf >> line) {
		if (line[0] != 'T' || line[1] != ':') { break; }
		vector<string> transition = splitStringByDelimiter(line.substr(2), ',');
		if (!isTransitionValid(transition, alphabet, states)) {
			cout << "Invalid transition: " << line.substr(2) << endl;
			exit(1);
		}	
		if (!transitions.count(make_pair(transition[0], transition[1]))) {
			vector<string> tmpVector;
			tmpVector.push_back(transition[2]);
			transitions[make_pair(transition[0], transition[1])] = tmpVector;
		} else {
			transitions[make_pair(transition[0], transition[1])].push_back(transition[2]);
		}
	}

	// read the start state
	if (line[0] != 'S' || line[1] != ':') {
		cout << "Invalid description of the start state." << endl;
		exit(1);
	} else if (!isInList(line.substr(2), states)) {
		cout << "Invalid start state: " << line.substr(2) << endl;
		exit(1);
	}
	string start = line.substr(2);

	// read the accepted states
	inf >> line;
	if (line[0] != 'F' || line[1] != ':') {
		cout << "Invalid description of the accepted states." << endl;
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
		analyzeNFA(alphabet, states, transitions, start, end, input);
	}
}
