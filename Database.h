#ifndef DATABASE
#define DATABASE

#include "Includes.h"

#define SIZE_OF_ATTRIBUTE(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define TABLE_MAX_PAGES 100

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

struct Table {
	uint32_t num_rows;
	void* pages[TABLE_MAX_PAGES];
};

struct Row {
	uint32_t id;
	string name;
	string email;
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

void serialize_row(Row* source, char* destination) {
	memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
	memcpy(destination + NAME_OFFSET, &(source->name), NAME_SIZE);
	memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(char* source, Row* destination) {
	memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
	memcpy(&(destination->name), source + NAME_OFFSET, NAME_SIZE);
	memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

#endif
