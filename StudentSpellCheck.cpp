#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <fstream> // for file streams
#include <iostream> // for cerr
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck()
{
	clearDictionary(); // Need to clear dictionary when destructing class
}

bool StudentSpellCheck::load(std::string dictionaryFile)
{
	//  O(N) time where N is the number of lines in the dictionary
	// Need to clear present dictionary if there is one

	ifstream infile(dictionaryFile);
	if (!infile) // if file doesn't exist/couldn't be loaded
	{
		return false;
	}

	clearDictionary(); // clear dictionary
	root = newNode(); // after clearing a dictionary, add new root node

	// for every line in the dictionaryFile, insert the line into the dictionary
	string s;
	while (getline(infile, s))
	{
		insertWord(s);
	}

	// For testing purposes
	/*
	ofstream outfile("C:/Users/Jed/Desktop/P4output/output.txt");
	if (!outfile)
	{
		cerr << "Cannot make/open outfile" << endl;
		exit(-1);
	}
	//testTrie(outfile, root, "");
	//testSearch(outfile, root, "cArGo");
	//testSpellCheck(outfile, root, "ut's");
	testSpellCheckLine(outfile, root, "Diz\"iz-a tezt.a isn't jrr");
	exit(-1);
	*/
	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions)
{
	// return true if the word is in the dictionary
	// return false and push suggestions onto the vector
	// xole -> aole bole...xolz xol'
	// O(L^2 + maxSuggestions) where L is the length of the word being searched for

	if (search(word)) // if word is in the dictionary, return true
	{
		return true;
	}
	suggestions.clear(); // clear vector

	// loop through string word, replacing every character with every character in the alphabet plus an apostrophe and find out if those words are in the dictionary
	for (int i = 0; word[i] != '\0'; i++)
	{
		string s = word;
		for (int j = 0; j < NUM_CHARS; j++) // loop that replaces characters in the word and searches for words in the string
		{
			if (suggestions.size() == max_suggestions) // if reached max suggestions, return immediately
			{
				return false; // return false as the original word is not in the dictionary
			}
			s[i] = getLetter(j);
			if (search(s))
			{
				suggestions.push_back(s);
			}
		}
	}
	return false; // return false as the original word is not in the dictionary
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems)
{
	// spell checks line of full text
	// puts start and end (inclusive) of a misspelled word onto problems vector
	// O(S+W*L) where S is the length of the line passed in, W is the number of words in the line, L is the max length of a word

	problems.clear(); // clear vectors
	if (line.empty()) // if the line is empty, do nothing
		return;

	string word = ""; // stores which word to currently check
	int start = 0;
	int end = 0;

	// loop through the line, checking each word to check
	// goes through the line once so S
	// searching for the ALL the words in the dictionary is num of words * bound for a word (W*L)
	// time compleity is O(S+W*L)
	for (int i = 0; i < line.size(); i++)
	{
		if (isalpha(line[i]) || line[i] == '\'') // if the current character is a letter or an apostrophe
		{
			end = i; // the end of the current word is the last letter of the word
			word = word + line[i]; // add letter to word
		}
		else
		{
			if (!word.empty()) // if the word is not empty
			{
				if (!search(word)) // if the word is NOT in the dictionary, add position to the vector
				{
					addToProblemVector(problems, start, end);
				}
				word = ""; // reset word
			}
			start = i + 1; // the start of the next word is the next letter or apostrophe, this will always ensure that the start will be on an index with a letter or an apostrophe
		}
	}
	// to account for the last word in the line
	if (!word.empty()) // if word is not empty
	{
		if (!search(word)) // if the word is NOT in the dictionary, add position to the vector
		{
			addToProblemVector(problems, start, end);
		}
	}

	// Slow but readable solution O(S^2 + L*W)
	/*
	if (line.empty()) // if the line is empty, do nothing
		return;

	string word = ""; // stores which word to currently check

	// loop through the line, finding each word to check
	for (int i = 0; i < line.size(); i++)
	{
		if (isalpha(line[i]) || line[i] == '\'') // if the current character is a letter or an apostrophe
		{
			word = word + line[i];
		}
		else
		{
			if (!word.empty()) // if the word is not empty
			{
				if (!search(word)) // if the word is NOT in the dictionary, add position to the vector
				{
					int start = line.find(word); // find the start position of a word
					int end = start + word.length() - 1; // the end position of a word
					addToProblemVector(problems, start, end);
				}
				word = ""; // reset word
			}
		}
	}
	if (!word.empty()) // if word is not empty
	{
		if (!search(word)) // if the word is NOT in the dictionary, add position to the vector
		{
			int start = line.find(word); // find the start position of a word
			int end = start + word.length() - 1; // the end position of a word
			addToProblemVector(problems, start, end);
		}
	}
	*/
}
