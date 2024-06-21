#ifndef B_TREE_COMMANDS
#define B_TREE_COMMANDS

#include "Database.h"
#include "Node.h"

void* GetPageBTree(Pager* pager, uint32_t pageNum) {

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

		if (pageNum >= pager->numPages)
			pager->numPages = pageNum + 1;
	}

	return pager->pages[pageNum];
}

Cursor* TableStartBTree(Table* table) {
	Cursor* cursor = (Cursor*)calloc(1, sizeof(Cursor));
	cursor->table = table;

	cursor->pageNum = table->rootPageNum;
	cursor->cellNum = 0;

	void* rootNode = GetPageBTree(table->pager, table->rootPageNum);
	uint32_t numCells = *LeafNodeNumCells(rootNode);
	cursor->endOfTable = (numCells == 0);

	return cursor;
}

Cursor* TableEndBTree(Table* table) {
	Cursor* cursor = (Cursor*)calloc(1, sizeof(Cursor));

	cursor->table = table;
	cursor->pageNum = table->rootPageNum;
	
	void* rootNode = getPage(table->pager, table->rootPageNum);
	uint32_t numCells = *LeafNodeNumCells(rootNode);
	cursor->cellNum = numCells;

	cursor->endOfTable = true;

	return cursor;
}

Pager* initPagerBtree(const char* filename) {

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
	pager->numPages = (fileLength / PAGE_SIZE);

	if (fileLength % PAGE_SIZE != 0) {
		cout << "Err: Corrupt file\n";
		exit(EXIT_FAILURE);
	}

	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		pager->pages[i] = nullptr;
	}

	return pager;
}

void flushPager(Pager* pager, uint32_t pageNum) {
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
	BOOL result = WriteFile(pager->fileDescriptor, pager->pages[pageNum], PAGE_SIZE, &bytesWritten, NULL);
	if (!result) {
		cout << "Unable to write to file\n";
		exit(EXIT_FAILURE);
	}
}

void closeBTreeDB(Table* table) {
	Pager* pager = table->pager;

	for (uint32_t i = 0; i < pager->numPages; i++) {
		if (pager->pages[i] == NULL)
			continue;

		flushPager(pager, i);
		free(pager->pages[i]);
		pager->pages[i] = NULL;
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

void IncrementCursorBTree(Cursor* cursor) {
	uint32_t pageNum = cursor->pageNum;
	void* node = GetPageBTree(cursor->table->pager, pageNum);

	cursor->cellNum += 1;
	if (cursor->cellNum >= (*LeafNodeNumCells(node))) 
		cursor->endOfTable = true;

}

char* RowSlotBTree(Cursor* cursor) {
	uint32_t pageNum = cursor->pageNum;

	void* page = GetPageBTree(cursor->table->pager, pageNum);

	return (char*)LeafNodeValue(page, cursor->cellNum);
}

Table* initBTree(const char* filename) {
	Pager* pager = initPagerBtree(filename);

	Table* table = (Table*)calloc(1, sizeof(Table));
	table->pager = pager;

	table->rootPageNum = 0;

	if (pager->numPages == 0) {
		void* rootNode = GetPageBTree(pager, 0);
		InitLeafNode(rootNode);
	}

	return table;
}

uint32_t getUnusedPageNum(Pager* pager) {
	return pager->numPages;
}

#endif 
