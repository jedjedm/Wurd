#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <list> // for std::list

class Undo;

constexpr int TAB_LENGTH = 4;
class StudentTextEditor : public TextEditor {
public:

	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
	int m_cursorRow;
	int m_cursorCol;
	std::list<std::string> m_lines;
	std::list<std::string>::iterator m_cursorIt;

	bool m_addToUndoStack; // stores whether or to submit(action) whenever insert(), backspace(), del(), or enter() is called
};

#endif // STUDENTTEXTEDITOR_H_
