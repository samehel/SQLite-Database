#ifndef COMMANDS
#define COMMANDS

#include "Database.h"
#include "Node.h"
#include "B-TREE.h"

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
	
	SerializeRow(rowToInsert, RowSlot(table, table->numRows));
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

	SerializeRow(rowToInsert, slot);

	flushPager(table->pager, table->numRows / ROWS_PER_PAGE, PAGE_SIZE);

	table->numRows += 1;

	free(cursor);
	return EXECUTE_SUCCESS;
}

ExecuteResult InsertBTree(InputBuffer inputBuffer, Table* table) {
	
	void* node = GetPageBTree(table->pager, table->rootPageNum);
	if ((*LeafNodeNumCells(node) >= LEAF_NODE_MAX_CELLS))
		return EXECUTE_TABLE_FULL;

	ExecuteResult res = ExtractInsertStatementData(inputBuffer);

	if (res == EXECUTE_FAILURE)
		return EXECUTE_FAILURE;

	Row* rowToInsert = &inputBuffer.getStatement().rowToInsert;
	Cursor* cursor = TableEnd(table);

	InsertIntoLeafNode(cursor, rowToInsert->id, rowToInsert);

	free(cursor);
	return EXECUTE_SUCCESS;
}

void PrintRow(Row* row) {
	cout << "ID: " + to_string(row->id) + "\nName: " + row->name + "\nEmail: " + row->email + "\n-----\n";
}

void MonitorConstants() {
	cout << "Node Constants:\n\n";
	cout << "ROW_SIZE: " + to_string(ROW_SIZE) << endl;
	cout << "COMMON_NODE_HEADER_SIZE: " + to_string(COMMON_NODE_HEADER_SIZE) << endl;
	cout << "LEAF_NODE_HEADER_SIZE: " + to_string(LEAF_NODE_HEADER_SIZE) << endl;
	cout << "LEAF_NODE_CELL_SIZE: " + to_string(LEAF_NODE_CELL_SIZE) << endl;
	cout << "LEAF_NODE_SPACE_FOR_CELLS: " + to_string(LEAF_NODE_SPACE_FOR_CELLS) << endl;
	cout << "LEAF_NODE_MAX_CELLS: " + to_string(LEAF_NODE_MAX_CELLS) << endl;
}

void DisplayBTree(void* node) {
	uint32_t numCells = *LeafNodeNumCells(node);
	cout << "Leaf (size " + to_string(numCells) + ") \n";
	for (uint32_t i = 0; i < numCells; i++) {
		uint32_t key = *LeafNodeKey(node, i);
		Row row;
		DeserializeRow(static_cast<char*>(LeafNodeValue(node, i)), &row);
		cout << " - " + to_string(key) + ": ID = " << row.id << ", Name = " << row.name << ", Email = " << row.email << endl << endl;
	}
}

void MonitorAndDisplayBTree(void* node) {
	MonitorConstants();
	cout << endl;
	DisplayBTree(node);
}

ExecuteResult Select(InputBuffer inputBuffer, Table* table) {
	
	Row row;

	if (table->numRows <= 0)
		return EXECUTE_TABLE_EMPTY;

	cout << "Number of rows in table: " << table->numRows << endl;

	for (uint32_t i = 0; i < table->numRows; i++) {
		DeserializeRow(RowSlot(table, i), &row);
		PrintRow(&row);
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
		DeserializeRow(RowSlotDB(cursor), &row);
		PrintRow(&row);
		IncrementCursor(cursor);
	}

	free(cursor);
	return EXECUTE_SUCCESS;
}

#endif