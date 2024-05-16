#ifndef INPUTBUFFER_CLASS
#define INPUTBUFFER_CLASS

#include "Includes.h"
#include "Database.h"

// Creating a macro for our prefix
#define PREFIX "."

/*
	Creating a basic InputBuffer class for the purpose of handing input
	from the user
*/
class InputBuffer {
private:
	string cmd;
	Statement statement;

	StatementType statementType;

	// Create a setter to set our Enum
	ostream& setStatement(string cmd) {
		if (cmd == string(PREFIX) + "exit") {
			statementType = EXIT_STATEMENT;
		} else if (cmd.substr(0, 7) == string(PREFIX) + "select") {
			statementType = SELECT_STATEMENT;
		} else if (cmd.substr(0, 7) == string(PREFIX) + "insert") {
			statementType = INSERT_STATEMENT;
		} else {
			return cout << "ERR: The command you are trying to execute does not exist. \n";
		}
	}

	// A function made just to check whether our string input only contains spaces
	bool containsOnlySpaces(string str) {
		for (char c : str) {
			if (c != ' ') {
				return false; 
			}
		}
		return true;
	}

public:

	// function created for reading command input from the user
	ostream& ReadInput() {
		cout << "Sameh\'s DB >> ";
		getline(cin, cmd);

		if (cmd == "" || cmd == string(PREFIX) + " " || containsOnlySpaces(cmd)) 
			return cout << "Your command cannot contain empty spaces. \n";
		else if (cmd[0] != PREFIX[0])
			return cout << "Did you try " + string(PREFIX) + cmd + "?" << endl;
			
		setStatement(cmd);
	}

	ostream& ReadInput(string cmd) {
		this->cmd = cmd;

		if (cmd == "" || cmd == string(PREFIX) + " " || containsOnlySpaces(cmd))
			return cout << "Your command cannot contain empty spaces. \n";
		else if (cmd[0] != PREFIX[0])
			return cout << "Did you try " + string(PREFIX) + cmd + "?" << endl;

		setStatement(cmd);
	}

	// Create a getter for our private string variable storing the DB command we want to execute
	string returnCmd() {
		return this->cmd;
	}

	// Create a getter for our private StatementType variable
	StatementType returnStatementType() {
		return this->statementType;
	}

	// Getter & Setter for our Statement
	Statement& getStatement() {
		return this->statement;
	}

	void setStatement(Statement statement) {
		this->statement = statement;
	}


};

#endif



