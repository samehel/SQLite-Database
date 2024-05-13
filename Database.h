#ifndef DATABASE
#define DATABASE

#include "Includes.h"

#define SIZE_OF_ATTRIBUTE(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define TABLE_MAX_PAGES 100

struct Row {
	uint32_t id;
	string name;
	string email;
};

const uint32_t ID_SIZE = SIZE_OF_ATTRIBUTE(Row, id);
const uint32_t NAME_SIZE = SIZE_OF_ATTRIBUTE(Row, name);
const uint32_t EMAIL_SIZE = SIZE_OF_ATTRIBUTE(Row, email);

const uint32_t ID_OFFSET = 0;
const uint32_t NAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = NAME_OFFSET + NAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + NAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

enum ExecuteResult {
	EXECUTE_TABLE_FULL,
	EXECUTE_SUCCESS,
	EXECUTE_FAILURE,
	EXECUTE_TABLE_EMPTY,
	NONE
};

struct Table {
	uint32_t numRows;
	void* pages[TABLE_MAX_PAGES];
};

enum StatementType {
	EXIT_STATEMENT,
	SELECT_STATEMENT,
	INSERT_STATEMENT
};

struct Statement {
	StatementType statementType;
	Row rowToInsert;
};

// The destination can't be a pointer to a void because it has to be a pointer to a complete object
void serialize_row(Row* source, char* destination) {
	memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
	strncpy_s(destination + NAME_OFFSET, NAME_SIZE, source->name.c_str(), _TRUNCATE);
	strncpy_s(destination + EMAIL_OFFSET, EMAIL_SIZE, source->email.c_str(), _TRUNCATE);
	// Ensure null termination for strings
	destination[NAME_OFFSET + NAME_SIZE - 1] = '\0';
	destination[EMAIL_OFFSET + EMAIL_SIZE - 1] = '\0';
}

// The source can't be a pointer to a void because it has to be a pointer to a complete object
void deserialize_row(char* source, Row* destination) {
	memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
	destination->name = string(source + NAME_OFFSET);
	destination->email = string(source + EMAIL_OFFSET);
}

char* RowSlot(Table* table, uint32_t rowNum) {
	uint32_t pageNum = rowNum / ROWS_PER_PAGE;

	void* page = table->pages[pageNum];

	if (page == NULL)
		page = table->pages[pageNum] = malloc(PAGE_SIZE);

	uint32_t rowOffset = rowNum % ROWS_PER_PAGE;
	uint32_t byteOffset = rowOffset * ROW_SIZE;

	// The page can't be added to the byteOffset because it has to be a pointer to a complete object
	// So we cast it to a pointer to a char then change it back to a void
	return (char*)page + byteOffset;
}

Table* initTable() {
	Table* table = (Table*)calloc(1, sizeof(Table));


	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		table->pages[i] = NULL;
	}

	return table;
}

void freeTable(Table* table) {
	for (int i = 0; table->pages[i]; i++)
		free(table->pages[i]);
	free(table);
}

#endif
