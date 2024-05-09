#ifndef INPUTBUFFER_CLASS
#define INPUTBUFFER_CLASS

#include "Includes.h"

/*
	Creating a basic InputBuffer class for the purpose of taking input
	from the user
*/
class InputBuffer {
private:
	string cmd;

public:

	// function created for reading command input from the user
	void read_input() {
		cout << "Sameh\'s DB >> ";
		getline(cin, cmd);
	}

	// Create a getter for our private string variable storing the DB command we want to execute
	string returnCmd() {
		return cmd;
	}
};

#endif



