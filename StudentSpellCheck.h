#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>
#include <fstream> // for filestream

// File constants
constexpr int NUM_CHARS = 27; // number of children a trie node should have, 27 for all letters plus an apostrophe

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck()
	{
		root = nullptr; // root doesn't start off with anything
	}
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	// Trie Implementation
	struct TrieNode
	{
		bool isDefined;
		TrieNode* children[NUM_CHARS];
	};
	TrieNode* root;

	// Private helper functions

	TrieNode* newNode() // returns a TrieNode ptr with initiliazed variables
	{
		TrieNode* tmp = new TrieNode;
		tmp->isDefined = false;
		for (int i = 0; i < NUM_CHARS; i++)
		{
			tmp->children[i] = nullptr;
		}
		return tmp;
	}

	// getIndex of a character
	// 0 to 25 for letters A to Z, 26 for apostrophe
	// Case insensitive
	// returns -1 if a character is not a valid letter
	int getIndex(char c)
	{
		if (c >= 'a' && c <= 'z')
			return c - 'a';
		if (c >= 'A' && c <= 'Z')
			return c - 'A';
		if (c == '\'')
		{
			return 26;
		}
		return -1; // if character is not a letter or an apostrophe
	}

	// returns a lowercase letter given an index
	// 0 to 25 returns 'a' to 'z'
	char getLetter(int index)
	{
		if (index == 26)
			return '\'';
		return index + 'a';
	}

	// Inserts a word into the trie
	void insertWord(std::string word)
	{
		TrieNode* trav = root;
		for (char c : word)
		{
			int index = getIndex(c);
			if (index == -1) // if char c is not a letter or an apostrophe
			{
				continue;
			}
			if (!trav->children[index]) // if the children[index] doesn't exist, make one
			{
				trav->children[index] = newNode();
			}
			trav = trav->children[index];
		}
		trav->isDefined = true; // after reaching destination node, note that the word is defined
	}

	// Searches through dictionary if word is in the dictionary
	bool search(std::string word)
	{
		if (word.empty()) // if the word is empty, then the word is not in the dictionary
		{
			return false;
		}
		TrieNode* trav = root;

		// loop through the word, going to the appropriate node
		for (char c : word)
		{
			int index = getIndex(c);
			if (index == -1) // if a character is not a letter or an apostrophe, skip
				continue;
			if (!trav->children[index]) // if the children[index] doesn't exist then the word is not defined
			{
				return false;
			}
			trav = trav->children[index]; // go down corresponding branch
		}
		return trav->isDefined;
	}

	void clearDictionary() // destructs the trie
	{
		if (root == nullptr)
			return;
		release(root);
		root = nullptr;
	}

	// Frees a trie seed and it's children
	void release(TrieNode* seed)
	{
		if (seed == nullptr)
			return;
		for (int i = 0; i < NUM_CHARS; i++)
		{
			release(seed->children[i]);
		}
		delete seed;
		seed = nullptr;
		return;
	}

	// Adds a new SpellCheck::Position with the appropriate start and end
	void addToProblemVector(std::vector<SpellCheck::Position>& problems, int start, int end)
	{
		SpellCheck::Position tmp;
		tmp.start = start;
		tmp.end = end;
		problems.push_back(tmp);
	}

	// tester functions
	/*
	void testTrie(std::ofstream& outfile, TrieNode* seed, std::string word)
	{
		if (seed == nullptr)
			return;
		if (seed->isDefined == true)
		{
			outfile << word << std::endl;
		}
		for (int i = 0; i < NUM_CHARS; i++)
		{
			testTrie(outfile, seed->children[i], word + getLetter(i));
		}
		return;
	}
	*/
	/*
	void testTrieBackwards(std::ofstream& outfile, TrieNode* seed, std::string word)
	{
		if (seed == nullptr)
			return;
		for (int i = NUM_CHARS - 1; i >= 0; i--)
		{
			testTrieBackwards(outfile, seed->children[i], word + getLetter(i));
		}
		if (seed->isDefined)
			outfile << word << std::endl;
	}
	*/
	/*
	void testSearch(std::ofstream& outfile, TrieNode* seed, std::string word)
	{
		if (search(word))
			outfile << word << " is in the string" << std::endl;
		else
			outfile << word << " is not in the string" << std::endl;
	}
	*/
	/*
	void testSpellCheck(std::ofstream& outfile, TrieNode* seed, std::string word)
	{
		std::vector<std::string> suggestions;
		suggestions.push_back("The word is in the dictionary!!!!\n");
		spellCheck(word, 12, suggestions);
		for (std::string s : suggestions)
		{
			outfile << s << std::endl;
		}
	}
	*/
	/*
	void testSpellCheckLine(std::ofstream& outfile, TrieNode* seed, std::string line)
	{
		std::vector<SpellCheck::Position> problems;
		spellCheckLine(line, problems);
		//outfile << "This is running\n";
		for (SpellCheck::Position pos : problems)
		{
			outfile << "{" << pos.start << ", " << pos.end << "}" << std::endl;
		}
	}
	*/
};

#endif  // STUDENTSPELLCHECK_H_
