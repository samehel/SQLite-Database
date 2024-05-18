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

struct Pager {
	HANDLE fileDescriptor;
	uint32_t fileLength;
	void* pages[TABLE_MAX_PAGES];
};

struct Table {
	uint32_t numRows;
	void* pages[TABLE_MAX_PAGES];
	Pager* pager;
};

struct Cursor {
	Table* table;
	uint32_t rowNum;
	bool endOfTable;
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

void* getPage(Pager* pager, uint32_t pageNum) {

	if (pageNum > TABLE_MAX_PAGES) {
		cout << "Err: Out of bounds page number\n";
		exit(EXIT_FAILURE);
	}

	if (pager->pages[pageNum] == NULL) {
		void* page = malloc(PAGE_SIZE);
		uint32_t numPages = pager->fileLength / PAGE_SIZE;

		if (pager->fileLength % PAGE_SIZE)
			numPages += 1;

		if (pageNum <= numPages) {
			DWORD filePointer = SetFilePointer(pager->fileDescriptor, pageNum * PAGE_SIZE, NULL, FILE_BEGIN);

			if (filePointer == INVALID_SET_FILE_POINTER) {
				cout << "Err: Unable to set file pointer\n";
				exit(EXIT_FAILURE);
			}

			DWORD bytesRead;
			BOOL result = ReadFile(pager->fileDescriptor, page, PAGE_SIZE, &bytesRead, NULL);
			if (!result) {
				cout << "Unable to read file\n";
				exit(EXIT_FAILURE);
			}
		}

		pager->pages[pageNum] = page;
	}

	return pager->pages[pageNum];
}

Cursor* TableStart(Table* table) {
	Cursor* cursor = (Cursor*)calloc(1, sizeof(Cursor));
	cursor->table = table;
	cursor->rowNum = 0;
	cursor->endOfTable = (table->numRows == 0);

	return cursor;
}

Cursor* TableEnd(Table* table) {
	Cursor* cursor = (Cursor*)calloc(1, sizeof(Cursor));
	cursor->table = table;
	cursor->rowNum = 0;
	cursor->endOfTable = true;

	return cursor;
}

void IncrementCursor(Cursor* cursor) {
	cursor->rowNum += 1;
	if (cursor->rowNum >= cursor->table->numRows)
		cursor->endOfTable = true;
}

char* RowSlotDB(Cursor* cursor) {
	uint32_t rowNum = cursor->rowNum;
	uint32_t pageNum = rowNum / ROWS_PER_PAGE;

	void* page = getPage(cursor->table->pager, pageNum);

	uint32_t rowOffset = rowNum % ROWS_PER_PAGE;
	uint32_t byteOffset = rowOffset * ROW_SIZE;

	// The page can't be added to the byteOffset because it has to be a pointer to a complete object
	// So we cast it to a pointer to a char then change it back to a void
	return (char*)page + byteOffset;
}

Table* initTable() {
	Table* table = (Table*)calloc(1, sizeof(Table));


	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		table->pages[i] = nullptr;
	}

	return table;
}

Pager* initPager(const char* filename) {

	LPCSTR lpFilename = filename;

	HANDLE hFile = CreateFileA(lpFilename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		cout << "Unable to open file\n";
		exit(EXIT_FAILURE);
	}

	DWORD fileLength = GetFileSize(hFile, NULL);
	if (fileLength == INVALID_FILE_SIZE) {
		CloseHandle(hFile);
		cout << "Unable to get file size\n";
		exit(EXIT_FAILURE);
	}

	Pager* pager = new Pager;
	pager->fileDescriptor = hFile;
	pager->fileLength = fileLength;

	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		pager->pages[i] = nullptr;
	}

	return pager;
}

Table* initDB(const char* filename) {
	Pager* pager = initPager(filename);
	uint32_t numRows = pager->fileLength / ROW_SIZE;

	Table* table = (Table*)calloc(1, sizeof(Table));
	table->numRows = 0;
	table->pager = pager;
	table->numRows = numRows;

	return table;
}

void freeTable(Table* table) {
	for (int i = 0; table->pages[i]; i++)
		free(table->pages[i]);
	free(table);
}

void flushPager(Pager* pager, uint32_t pageNum, uint32_t size) {
	if (pager->pages[pageNum] == NULL) {
		cout << "Err: Null page cannot be flushed.\n";
		exit(EXIT_FAILURE);
	}

	DWORD offset = SetFilePointer(pager->fileDescriptor, pageNum * PAGE_SIZE, NULL, FILE_BEGIN);

	if (offset == INVALID_SET_FILE_POINTER) {
		cout << "Err: Cannot seek\n";
		exit(EXIT_FAILURE);
	}

	DWORD bytesWritten;
	BOOL result = WriteFile(pager->fileDescriptor, pager->pages[pageNum], size, &bytesWritten, NULL);
	if (!result) {
		cout << "Unable to write to file\n";
		exit(EXIT_FAILURE);
	}
}

void closeDB(Table* table) {
	Pager* pager = table->pager;
	uint32_t numFullPages = table->numRows / ROWS_PER_PAGE;

	for (uint32_t i = 0; i < numFullPages; i++) {
		if (pager->pages[i] == NULL)
			continue;

		flushPager(pager, i, PAGE_SIZE);
		free(pager->pages[i]);
		pager->pages[i] = NULL;
	}

	uint32_t numAdditionalRows = table->numRows % ROWS_PER_PAGE;
	if (numAdditionalRows > 0) {
		uint32_t pageNum = numFullPages;
		if (pager->pages[pageNum] != NULL) {
			flushPager(pager, pageNum, numAdditionalRows * ROW_SIZE);
			free(pager->pages[pageNum]);
			pager->pages[pageNum] = NULL;
		}
	}

	BOOL result = CloseHandle(pager->fileDescriptor);
	if (!result) {
		cout << "Unable to close file\n";
		exit(EXIT_FAILURE);
	}

	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		void* page = pager->pages[i];
		if (page) {
			free(page);
			pager->pages[i] = NULL;
		}
	}

	free(pager);
	free(table);
}

#endif
