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
			return cout << "ERR: The command you are trying to execute does not exist.\n";
		}
	}

public:

	// function created for reading command input from the user
	ostream& read_input() {
		cout << "Sameh\'s DB >> ";
		getline(cin, cmd);

		if (cmd == "" || cmd == ". " || cmd.find(' ') != string::npos)
			return cout << "Your command cannot contain empty spaces.\n";
		else if (cmd[0] != '.')
			return cout << "Did you try " + string(PREFIX) + cmd + "?" << endl;
			
		setStatement(cmd);
	}
	

	// Create a getter for our private string variable storing the DB command we want to execute
	string returnCmd() {
		return cmd;
	}

	// Create a getter for our private statement variable
	StatementType returnStatement() {
		return statementType;
	}

};

#endif



