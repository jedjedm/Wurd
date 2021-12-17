#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch)
{
	// Used by text editor to push stuff onto the stack
	// must be O(1) in average case, going up to O(length of current edited line) sometimes

	// if undo stack is empty, just add normally
	if (m_undoStack.empty())
	{
		addToStack(action, row, col, ch);
	}
	else // if the undto stack is not empty, check cases
	{
		UndoData top = m_undoStack.top(); // stores what's on the top of the stack

		if (action != top.m_action) // if the action of the top of the stack is not the same as the new action being pushed in, push something new
		{
			addToStack(action, row, col, ch);
		}
		else // if the top of the stack has the same action as the new thing to push
		{
			// if the action is JOIN or SPLIT add to the stack normally since these don't batch
			if (action == JOIN || action == SPLIT)
			{
				addToStack(action, row, col, ch);
			}
			else if (action == INSERT)
			{
				// if the action is batchable, batch it
				if (top.m_row == row && (top.m_col + m_undoStack.top().m_count) == col)
				{
					m_undoStack.top().m_count++; // increase the number of characters to delete
				}
				else // since batching does not occur here, add normally
				{
					addToStack(action, row, col, ch);
				}

			}
			else if (action == DELETE)
			{
				// if batching by del() works, add ch to the end of the batch text
				if (top.m_row == row && ((col == top.m_col)))
				{
					m_undoStack.top().m_text += std::string(1, ch);
				}
				else if (top.m_row == row && (col == (top.m_col - 1))) // if batching by backspace() works, add character to the beginning of the batch text
				{
					m_undoStack.top().m_text = std::string(1, ch) + m_undoStack.top().m_text;
					m_undoStack.top().m_col = col; // shift the starting position by one when backspace is called
				}
				else // since batching does not occur here, add normally
				{
					addToStack(action, row, col, ch);
				}
			}
		}
	}
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text)
{
	// return the opposite of what's in the row
	// return the start row/col of the operation, not it's end coordinates
	// must be O(1) time
	if (m_undoStack.empty())
	{
		return Action::ERROR;
	}

	// get the data and pop it
	StudentUndo::UndoData und = m_undoStack.top();
	m_undoStack.pop();

	// Set the referenes to the appropriate values
	row = und.m_row;
	count = und.m_count;
	text = und.m_text;

	// return the opposite operation
	switch (und.m_action)
	{
		case INSERT:
			col = und.m_col - 1; // when returning a delete, the column should be where the delete started
			return DELETE;
			break;
		case DELETE:
			col = und.m_col;
			return INSERT;
			break;
		case JOIN:
			col = und.m_col;
			return SPLIT;
			break;
		case SPLIT:
			col = und.m_col;
			return JOIN;
			break;
		default:
			return ERROR;
			break;
	}
	return ERROR;
}

void StudentUndo::clear()
{
	// Clear what's ever in the stack
	// Must be O(N) where N is the number of elements in the stack

	while (!m_undoStack.empty())
		m_undoStack.pop();
}