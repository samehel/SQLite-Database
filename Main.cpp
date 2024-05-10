// Importing our built-in libraries
#include "Includes.h"
#include <map>

// Importing our custom classes
#include "InputBuffer.h"
#include "Commands.h"

// Creating a macro for our prefix
#define PREFIX "."


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
		map<string, void(*)()> commands = {
			{ ".exit", ExitDatabase },
		};

		auto execCmd = commands.find(inputBuffer.returnCmd());
		if (execCmd != commands.end())
			execCmd->second();
		else {
			if (inputBuffer.returnCmd() == "" || inputBuffer.returnCmd() == ". " || inputBuffer.returnCmd().find(' ') != string::npos)
				cout << "Your command cannot contain empty spaces.\n";
			else if (inputBuffer.returnCmd()[0] != '.')
				cout << "Did you try " + string(PREFIX) + inputBuffer.returnCmd() + "?" << endl;
			else
				cout << "ERR: The command you are trying to execute does not exist.\n";
		}

	}
}