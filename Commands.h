#ifndef COMMANDS
#define COMMANDS

#include "Database.h"

void ExitDatabase(Table* table) {
	cout << "Exiting database..." << endl;
	freeTable(table);
	exit(EXIT_SUCCESS);
}

void ExitDatabaseStorage(Table* table) {
	cout << "Exiting database..." << endl;
	closeDB(table);
	exit(EXIT_SUCCESS);
}


ExecuteResult ExtractInsertStatementData(InputBuffer& inputBuffer) {

	Statement statement;

	statement.statementType = INSERT_STATEMENT;

	istringstream iss(inputBuffer.returnCmd().substr(7)); // Skip ".insert" part
	iss >> statement.rowToInsert.id >> statement.rowToInsert.name >> statement.rowToInsert.email;

	// Check if extraction was successful
	if (iss.fail())
		return EXECUTE_FAILURE;
	else
		inputBuffer.setStatement(statement);

	return EXECUTE_SUCCESS;
}

ExecuteResult Insert(InputBuffer inputBuffer, Table* table) {

	if (table->numRows >= TABLE_MAX_ROWS)
		return EXECUTE_TABLE_FULL;

	ExecuteResult res = ExtractInsertStatementData(inputBuffer);

	if (res == EXECUTE_FAILURE)
		return EXECUTE_FAILURE;

	Row* rowToInsert = &inputBuffer.getStatement().rowToInsert;
	
	serialize_row(rowToInsert, RowSlot(table, table->numRows));
	table->numRows += 1;

	return EXECUTE_SUCCESS;
}

ExecuteResult InsertDB(InputBuffer inputBuffer, Table* table) {
	if (table->numRows >= TABLE_MAX_ROWS)
		return EXECUTE_TABLE_FULL;

	ExecuteResult res = ExtractInsertStatementData(inputBuffer);

	if (res == EXECUTE_FAILURE)
		return EXECUTE_FAILURE;

	Row* rowToInsert = &inputBuffer.getStatement().rowToInsert;
	Cursor* cursor = TableEnd(table);

	char* slot = RowSlotDB(cursor);

	serialize_row(rowToInsert, slot);

	flushPager(table->pager, table->numRows / ROWS_PER_PAGE, PAGE_SIZE);

	table->numRows += 1;

	free(cursor);
	return EXECUTE_SUCCESS;
}

void printRow(Row* row) {
	cout << "ID: " + to_string(row->id) + "\nName: " + row->name + "\nEmail: " + row->email + "\n-----\n";
}

ExecuteResult Select(InputBuffer inputBuffer, Table* table) {
	
	Row row;

	if (table->numRows <= 0)
		return EXECUTE_TABLE_EMPTY;

	cout << "Number of rows in table: " << table->numRows << endl;

	for (uint32_t i = 0; i < table->numRows; i++) {
		deserialize_row(RowSlot(table, i), &row);
		printRow(&row);
	}

	return EXECUTE_SUCCESS;
}

ExecuteResult SelectDB(InputBuffer inputBuffer, Table* table) {
	
	Cursor* cursor = TableStart(table);
	Row row;

	if (table->numRows <= 0)
		return EXECUTE_TABLE_EMPTY;

	cout << "Number of rows in table: " << table->numRows << endl;

	while (!(cursor->endOfTable)) {
		deserialize_row(RowSlotDB(cursor), &row);
		printRow(&row);
		IncrementCursor(cursor);
	}

	free(cursor);
	return EXECUTE_SUCCESS;
}

#endif