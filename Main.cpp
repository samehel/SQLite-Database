// Importing our custom classes
#include "InputBuffer.h"
#include "Commands.h"
#include "TestSuite.h"

void ProcessResult(InputBuffer& inputBuffer, Table* table, ExecuteResult& res);
void MainMenu(InputBuffer& inputBuffer, Table* table, ExecuteResult& res);

/*
 * Main function for the database application.
 * This function continuously prompts the user for commands,
 * executes the corresponding function for each command,
 * and displays an error message if an invalid command is entered.
 */
int main() {

	short choice;

	cout << "---------------------------------\n";
	cout << "------ Relational Database ------\n";
	cout << "---------------------------------\n";
	cout << "1. Run the program\n";
	cout << "2. Run the test cases\n";
	cout << "---------------------------------\n";

	while (true) {
		cout << ">> ";
		cin >> choice;
		cin.ignore();

		switch (choice) {
		case 1:
		{
			InputBuffer inputBuffer;
			Table* table = initTable();
			ExecuteResult res = NONE;

			MainMenu(inputBuffer, table, res);
			break;
		}
		case 2:
		{
			TestSuite testSuite;
			testSuite.Run();
			break;
		}
		default:
			cout << "Err: Please enter a valid option" << endl;
		}
	}
}

void MainMenu(InputBuffer& inputBuffer, Table* table, ExecuteResult& res) {

	while (true) {

		inputBuffer.ReadInput();

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

void ProcessResult(InputBuffer& inputBuffer, Table* table, ExecuteResult& res) {

	switch (res) {
	case EXECUTE_TABLE_FULL:
		cout << "ERR: The table you are attempting to insert into is full." << endl;
		break;
	case EXECUTE_SUCCESS:
		cout << "Statement executed successfully." << endl;
		break;
	case EXECUTE_FAILURE:
		cout << "Statement failed to execute." << endl;
		break;
	case EXECUTE_TABLE_EMPTY:
		cout << "INFO: The table you are attempting to retrieve from is empty." << endl;
		break;
	}

	MainMenu(inputBuffer, table, res);
}