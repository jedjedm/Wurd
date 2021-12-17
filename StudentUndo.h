#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack> // for std::stack
#include <string> // for std::string

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct UndoData
	{
		Action m_action;
		int m_row;
		int m_col;
		std::string m_text; // will be empty if m_action is INSERT, JOIN, or SPLIT
							// if m_action is DELETE, will store what text to restore
		int m_count; // will be 1 if m_action is DELETE, JOIN, SPLIT
					 // if m_action is INSERT, will store how many characters to delete
	};
	std::stack<UndoData> m_undoStack; // undoStack, holds UndoData struct

	// Whenever batching does not occur, call this function to add stuff to the stack
	void addToStack(const Action action, int row, int col, char ch)
	{
		UndoData und;
		und.m_action = action;
		und.m_row = row;
		und.m_col = col;
		und.m_count = 1; // the starting count of every action is 1

		// when adding an INSERT, JOIN, or SPLIT action to the stack, the text should be empty
		// when adding a DELETE action to the stack, the text should be filled with what to do
		switch (action)
		{
		case INSERT:
		case JOIN:
		case SPLIT:
			und.m_text = "";
			break;
		case DELETE:
			und.m_text = std::string(1, ch);
			break;
		default:
			break;
		}
		m_undoStack.push(und);
	}
};

#endif // STUDENTUNDO_H_
