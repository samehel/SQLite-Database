// Importing our built-in libraries
#include "Includes.h"
#include <map>

// Importing our custom classes
#include "InputBuffer.h"
#include "Commands.h"


/*
 * Main function for the database application.
 * This function continuously prompts the user for commands,
 * executes the corresponding function for each command,
 * and displays an error message if an invalid command is entered.
 */
int main() {
	InputBuffer inputBuffer;
	while(true) {

		inputBuffer.read_input();
		
		switch (inputBuffer.returnStatement()) {
		case EXIT_STATEMENT:
			exit(EXIT_SUCCESS);
			break;
		case SELECT_STATEMENT:
			Select();
			break;
		case INSERT_STATEMENT:
			Insert();
			break;
		}

	}
}