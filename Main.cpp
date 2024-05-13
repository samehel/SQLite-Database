// Importing our custom classes
#include "InputBuffer.h"
#include "Commands.h"


void ProcessResult(InputBuffer inputBuffer, Table* table, ExecuteResult res);
void MainMenu(InputBuffer inputBuffer, Table* table, ExecuteResult res);

/*
 * Main function for the database application.
 * This function continuously prompts the user for commands,
 * executes the corresponding function for each command,
 * and displays an error message if an invalid command is entered.
 */
int main() {
	InputBuffer inputBuffer;
	Table* table = initTable();
	ExecuteResult res = NONE;

	MainMenu(inputBuffer, table, res);
}

void MainMenu(InputBuffer inputBuffer, Table* table, ExecuteResult res) {

	while (true) {

		inputBuffer.read_input();

		switch (inputBuffer.returnStatementType()) {
		case EXIT_STATEMENT:
			ExitDatabase(table);
			break;
		case SELECT_STATEMENT:
			res = Select(inputBuffer, table);
			break;
		case INSERT_STATEMENT:
			res = Insert(inputBuffer, table);
			break;
		}

		ProcessResult(inputBuffer, table, res);
	}
}

void ProcessResult(InputBuffer inputBuffer, Table* table, ExecuteResult res) {

	switch (res) {
	case EXECUTE_TABLE_FULL:
		cout << "ERR: The table you are attempting to insert into is full. " << endl;
		break;
	case EXECUTE_SUCCESS:
		cout << "Statement executed successfully. " << endl;
		break;
	case EXECUTE_FAILURE:
		cout << "Statement failed to execute. " << endl;
		break;
	case EXECUTE_TABLE_EMPTY:
		cout << "INFO: The table you are attempting to retrieve from is empty. " << endl;
		break;
	}

	MainMenu(inputBuffer, table, res);
}