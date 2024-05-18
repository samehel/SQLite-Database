// Importing our custom classes
#include "InputBuffer.h"
#include "Commands.h"
#include "TestSuite.h"

void ProcessResult(InputBuffer& inputBuffer, Table* table, ExecuteResult& res, short mode);
void MainMenu(InputBuffer& inputBuffer, Table* table, ExecuteResult& res);
void MainMenuDB(InputBuffer& inputBuffer, Table* table, ExecuteResult& res);

/*
 * Main function for the database application.
 * This function continuously prompts the user for commands,
 * executes the corresponding function for each command,
 * and displays an error message if an invalid command is entered.
 */
int main() {

	short choice;
	InputBuffer inputBuffer;
	ExecuteResult res = NONE;

	cout << "---------------------------------\n";
	cout << "------ Relational Database ------\n";
	cout << "---------------------------------\n";
	cout << "1. Run memory based program\n";
	cout << "2. Run memory based test cases\n";
	cout << "3. Run disk based program\n";
	cout << "4. Run disk based test cases\n";
	cout << "---------------------------------\n";

	while (true) {
		cout << ">> ";
		cin >> choice;
		cin.ignore();

		switch (choice) {
		case 1:
		{
			Table* table = initTable();
			MainMenu(inputBuffer, table, res);
			break;
		}
		case 2:
		{
			TestSuite testSuite;
			testSuite.Run();
			break;
		}
		case 3:
		{
			string dbNameInput;

			cout << "Enter the name of your database: ";
			cin >> dbNameInput;
			cin.ignore();

			if (dbNameInput == "") {
				cout << "Err: Please provide a database name. \n";
				exit(EXIT_FAILURE);
			}

			const int length = dbNameInput.length();
			char* dbName = new char[length + 1];
			strcpy_s(dbName, sizeof(dbName), dbNameInput.c_str());

			Table* table = initDB(dbName);
			MainMenuDB(inputBuffer, table, res);
			break;
		}
		case 4: 
		{

			break;
		}
		default:
			cout << "Err: Please enter a valid option" << endl;
		}
	}
}

void MainMenuDB(InputBuffer& inputBuffer, Table* table, ExecuteResult& res) {

	while (true) {

		inputBuffer.ReadInput();

		switch (inputBuffer.returnStatementType()) {
		case EXIT_STATEMENT:
			ExitDatabaseStorage(table);
			break;
		case SELECT_STATEMENT:
			res = SelectDB(inputBuffer, table);
			break;
		case INSERT_STATEMENT:
			res = InsertDB(inputBuffer, table);
			break;
		}

		ProcessResult(inputBuffer, table, res, 1);
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

		ProcessResult(inputBuffer, table, res, 0);
	}
}

void ProcessResult(InputBuffer& inputBuffer, Table* table, ExecuteResult& res, short mode) {

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

	if (mode == 0)
		MainMenu(inputBuffer, table, res);
	else if (mode == 1)
		MainMenuDB(inputBuffer, table, res);
}