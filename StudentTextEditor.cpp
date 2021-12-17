#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>

#include <fstream> // for file streams
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
	: TextEditor(undo)
{
	// Sets up everything when the program first starts
	// Must be O(1)

	m_cursorRow = 0;
	m_cursorCol = 0;
	m_lines.push_back("");
	m_cursorIt = m_lines.begin();
	m_addToUndoStack = true; // default setting is that calling every operation should add to undo stack
}

StudentTextEditor::~StudentTextEditor()
{
	m_lines.clear();
}

bool StudentTextEditor::load(std::string file) {

	// Loads in everything from a file
	// Must be O(M+N+U) where M is the number of characters in the editor, N is the number of characters in the file being loaded and U is the number of items in the undo stack
	// Needs to strip out carriage return
	// If the file can't be found, return false
	// Else return true
	// Must reset cursor to the beginning

	ifstream infile(file);
	if (!infile) // if file doesn't exist
	{
		//cerr << "Error: " << file << "doesn't exist" << endl;
		return false;
	}

	// When loading, reset everything including the cursor
	reset();
	m_lines.pop_back(); // reset() adds an empty line so get rid of that empty line

	string s;
	while (getline(infile, s))
	{
		if (!s.empty() && s[s.size()-1] == '\r') // checks if the current line is not empty and if the current lines ends with a "\r" 
		{
			s.pop_back(); // remove the '\r'
		}
		m_lines.push_back(s);
	}

	// To set up the cursor
	m_cursorRow = 0;
	m_cursorCol = 0;
	m_cursorIt = m_lines.begin();

	return true;
}

bool StudentTextEditor::save(std::string file)
{
	// O(M) where M is the number of lines being edited
	// Must save to the given file
	// If the given file can't be opened/accessed, return false
	// Else, save into the file and return true

	ofstream outfile(file);
	if (!outfile)
	{
		return false;
	}
	// O(M)
	for (string line : m_lines)
	{
		outfile << line << endl;
	}
	return true;
}

void StudentTextEditor::reset()
{
	// Clear everything in the list and clear undo stack
	// Reset cursor to [0,0]
	// Should be no text in the text editor afterwards
	// O(N + U) where N is the number of lines and U is the number of undo operations in the undo stack

	m_lines.clear(); // clears everything in text editor, O(N)
	m_lines.push_back(""); // adds a new empty line in the list
	
	// Sets cursor to [0,0]
	m_cursorCol = 0;
	m_cursorRow = 0;
	m_cursorIt = m_lines.begin(); // Point cursorIt to the first line of the list

	getUndo()->clear();
}

void StudentTextEditor::move(Dir dir)
{
	// Moves the cursor
	// Must be O(1)

	switch (dir)
	{
		case UP:
			if (m_cursorRow == 0) // if cursor at the top of the file, you can't do anything
				return;
			m_cursorRow--;
			m_cursorIt--;
			if (m_cursorCol > m_cursorIt->size()) // if the cursor will be off the line when going up, go to the end of the current line
				m_cursorCol = m_cursorIt->size();
			break;
		case DOWN:
			if (m_cursorRow == m_lines.size() - 1) // if the cursor is at the bottom of the file, don't do anything
				return;
			m_cursorRow++;
			m_cursorIt++;
			if (m_cursorCol > m_cursorIt->size()) // if the cursor will be off the line when going down, go to the end of the current line
				m_cursorCol = m_cursorIt->size();
			break;
		case LEFT:
			if (m_cursorCol == 0) // if the cursor is all the way towards the left
			{
				if (m_cursorRow == 0) // if cursor at the top of the file, you can't do anything
					return;
				else // if the cursor is currently at left edge of the current line and there is a line above the cursor, go to the last character of the line above
				{
					m_cursorRow--;
					m_cursorIt--;
					m_cursorCol = m_cursorIt->size();
					return;
				}
			}
			m_cursorCol--;
			break;
		case RIGHT:
			if (m_cursorCol == m_cursorIt->size()) // if the cursor is all the way towards the right of the line
			{
				if (m_cursorRow == m_lines.size() - 1) // if the cursor is at the bottom of the file, don't do anything
					return;
				else // if the cursor is currently at the right edge of the current line, go to the first character of the line below
				{
					m_cursorRow++;
					m_cursorIt++;
					m_cursorCol = 0;
					return;
				}
			}
			m_cursorCol++;
			break;
		case HOME:
			// moves to the beginning of the file
			m_cursorCol = 0;
			m_cursorRow = 0;
			m_cursorIt = m_lines.begin();
			break;
		case END:
			// moves to the end of the file
			m_cursorIt = std::prev(m_lines.end(), 1);
			m_cursorRow = m_lines.size() - 1;
			m_cursorCol = m_cursorIt->size();
		default:
			break;
	}
}

void StudentTextEditor::del()
{
	// Needs to be O(L)
	// deletes the ON the cursor
	// Implementation is O(L) when erasing a letter and O(L1 + L2) when erasing a line

	// if the cursor is NOT past the last character of a line
	if (m_cursorCol != m_cursorIt->size()) // if the cursor is NOT past the last character of a line
	{
		char ch = m_cursorIt->at(m_cursorCol); // stores the char to delete
		m_cursorIt->erase(m_cursorCol, 1); // delete character where the cursor is
		if(m_addToUndoStack) // if the operation is suppoed to add to the undo stack
			getUndo()->submit(Undo::Action::DELETE, m_cursorRow, m_cursorCol, ch); // add to Undo stack
	}
	else // if the cursor is past the last character of a line
	{
		if (m_cursorRow == m_lines.size() - 1) // if the cursor is in the last line of the file
		{
			return; // do nothing
		}

		// to get to this point the cursor must be in the last column of a line that's not the last line
		// in this case a JOIN operation is pushed onto the undo stack because a line is being joined with another
		auto nextLineIt = std::next(m_cursorIt, 1); // points to the line below the cursor
		*m_cursorIt = *m_cursorIt + *nextLineIt; // combine the current line and the next line
		m_lines.erase(nextLineIt); // delete the next line
		if (m_addToUndoStack) // if the operation is suppoed to add to the undo stack
			getUndo()->submit(Undo::Action::JOIN, m_cursorRow, m_cursorCol, '\n'); // add to Undo stack
	}

}

void StudentTextEditor::backspace()
{
	// Needs to be O(L)
	// deletes the letter before the cursor
	// Implementation is O(L) when erasing a letter and O(L1 + L2) when erasing a line

	// if the cursor isn't in the first column of the current line
	if (m_cursorCol > 0)
	{
		char ch = m_cursorIt->at(m_cursorCol - 1); // stores the char to delete
		m_cursorIt->erase(m_cursorCol - 1, 1); // delete character to the left of where the cursor is
		m_cursorCol--;
		if(m_addToUndoStack) // if the operation is suppoed to add to the undo stack
			getUndo()->submit(Undo::Action::DELETE, m_cursorRow, m_cursorCol, ch);
	}
	else // if the cursor is in the first column of the current line
	{
		if (m_cursorRow == 0) // if the cursor is in the first row of the current line
		{
			return; // do nothing
		}
		// to get to this point this means that the cursor is in the first col of a line that's not the first line
		// in this case a JOIN operation is pushed onto the undo stack because a line is being joined with another
		auto previousLineIt = std::prev(m_cursorIt, 1); // points to the line above the cursor
		m_cursorCol = previousLineIt->size(); // change column to be at appropriate position
		*previousLineIt = *previousLineIt + *m_cursorIt; // combine current line and previous line
		m_lines.erase(m_cursorIt); // remove the current line
		// move the cursor up
		m_cursorIt = previousLineIt;
		m_cursorRow--;
		if(m_addToUndoStack) // if the operation is suppoed to add to the undo stack
			getUndo()->submit(Undo::Action::JOIN, m_cursorRow, m_cursorCol, '\n'); // add to Undo stack
	}
}

void StudentTextEditor::insert(char ch)
{
	// O(L) is needed where L is the length of the line
	// If tab is pressed, enter 4 spaces at the cursor line and move cursor to the right four times. 4 actions will be pushed to the undo stack

	// string.insert is O(L) long so this implementation passes the time complexity requirement
	if (ch == '\t') // if a tab is entered
	{
		// Treat's adding a tab as adding four consecutive spaces
		// This means that there will be four pushes
		for (int i = 0; i < TAB_LENGTH; i++) // add four spaces to the current line in the current column, shifting the column as appropriate
		{
			m_cursorIt->insert(m_cursorCol, " ");
			m_cursorCol++; // move column to the right by one
			if(m_addToUndoStack) // if the operation is suppoed to add to the undo stack
				getUndo()->submit(Undo::Action::INSERT, m_cursorRow, m_cursorCol, ch); // add to undo stack
		}
	}
	else if (ch != '\t') // if a tab is NOT entered
	{
		m_cursorIt->insert(m_cursorCol, std::string(1, ch)); // insert ch at the current cursor column
		m_cursorCol++; // move column to the right by one
		if(m_addToUndoStack) // if the operation is suppoed to add to the undo stack
			getUndo()->submit(Undo::Action::INSERT, m_cursorRow, m_cursorCol, ch); // add to undo stack
	}
}

void StudentTextEditor::enter()
{
	// For when the user presses enter
	// Will break the line where the cursor is at into two
	// Everything after the cursor (including the cursor) is added to the next line
	// if there is no next line, make one
	// O(L) where L is the length of the line of the cursor, must not depend on how many lines there are in the text
	// after pressing enter, the cursor is at the first column of the next row

	// Before doing anything, add to undo stack for enter
	if(m_addToUndoStack) // if the operation is suppoed to add to the undo stack
		getUndo()->submit(Undo::Action::SPLIT, m_cursorRow, m_cursorCol, '\n');


	// This implementation is O(L)
	// the check to see if the cursor is on the last line and adding a new line in this case is O(1)
	// subtr is O(L)
	auto nextLineIt = std::next(m_cursorIt, 1); // iterator to the next line

	// The if and else if statements figure out how to add the new line
	if (nextLineIt == m_lines.end()) // if the cursor is currently at the last line, make a new line at the end
	{
		m_lines.push_back(""); // makes a new line that's empty, constant time
		nextLineIt = m_lines.end(); // constant time
		nextLineIt--; // constant time
	}
	else if(nextLineIt != m_lines.end()) // if there is a line below a the cursor, insert a new line below the cursor
	{
		m_lines.insert(nextLineIt, "");
		nextLineIt = std::next(m_cursorIt, 1); // iterator now points to the newly made line
	}

	string nextLine = m_cursorIt->substr(m_cursorCol); // stores what to put into the next line
	*nextLineIt = nextLine + *nextLineIt; // add the new string into the next line
	*m_cursorIt = m_cursorIt->substr(0, m_cursorCol); // cuts the current line

	// moves the cursor to the appropriate spot after pressing enter
	m_cursorRow++;
	m_cursorCol = 0;
	m_cursorIt++;
}

void StudentTextEditor::getPos(int& row, int& col) const
{
	row = m_cursorRow;
	col = m_cursorCol;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const
{
	if (startRow < 0 || numRows < 0 || startRow > m_lines.size()) // if startRow and numRows are invalid numbers
		return -1;
	lines.clear(); // clear vector

	// figures out where to start inserting, whether to move the iterator upwards or downwards
	auto it = m_cursorIt; // set an iterator to where the cursor is
	if (m_cursorRow > startRow) // if the cursor is below the start row, go up
	{
		int count = m_cursorRow - startRow;
		while (count > 0)
		{
			it--;
			count--;
		}
	}
	else if (m_cursorRow < startRow) // if the cursor is above the start row, go down
	{
		int count = startRow - m_cursorRow;
		while (count > 0)
		{
			count--;
			it++;
		}
	}

	// add to the vector stack starting from startRow
	int count = 0;
	while (it != m_lines.end() && count < numRows)
	{
		lines.push_back(*it);
		it++;
		count++;
	}
	return lines.size();
}

void StudentTextEditor::undo()
{
	int row, col, count;
	string text;
	Undo::Action action = getUndo()->get(row, col, count, text);

	// do nothing if the undo stack is empty
	if (action == Undo::Action::ERROR)
		return;

	// set's the cursor row to the where the operation should start
	if (m_cursorRow > row) // if the cursor is below the row where the action takes place, go up
	{
		int countRow = m_cursorRow - row;
		while (countRow > 0)
		{
			m_cursorIt--;
			m_cursorRow--;
			countRow--;
		}
	}
	else if (m_cursorRow < row) // if the cursor is above the row where the action takes place, go down
	{
		int countRow = row - m_cursorRow;
		while (countRow > 0)
		{
			countRow--;
			m_cursorIt++;
			m_cursorRow++;
		}
	}
	m_cursorCol = col; // sets the column to where the operation should start

	m_addToUndoStack = false; // make it false since this function uses del(), and enter()

	// Does the operations
	switch (action)
	{
		// have to insert text
		case Undo::Action::INSERT:
			m_cursorIt->insert(m_cursorCol, text); // insert string text starting from col position
			break;
		// have to delete text
		case Undo::Action::DELETE:
			m_cursorIt->erase(m_cursorCol, count); // delete count number of characters starting from the col position
			break;
		// have to join two lines
		case Undo::Action::JOIN:
			del(); // deleting at the edge of a line takes care of join
			break;
		// have to split two lines
		case Undo::Action::SPLIT:
			enter(); // entering at right position takes care of the split
			break;
		default:
			break;
	}
	m_addToUndoStack = true; // after this function is done, operations should act normal
	return;
}