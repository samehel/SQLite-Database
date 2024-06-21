#ifndef NODE_CLASS
#define NODE_CLASS

#include "Includes.h"
#include "Database.h"

void* GetPageBTree(Pager* pager, uint32_t pageNum);
uint32_t getUnusedPageNum(Pager* pager);

enum NodeType {
	NODE_INTERNAL,
	NODE_LEAF
};

const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint8_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = 0;
const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET = 4;
const uint32_t INTERNAL_NODE_HEADER_SIZE = 8;
const uint32_t INTERNAL_NODE_CELL_SIZE = 12;
const uint32_t INTERNAL_NODE_CHILD_SIZE = 4;


uint32_t* LeafNodeNumCells(void* node) {
	return  (uint32_t*)((char*)node + LEAF_NODE_NUM_CELLS_OFFSET);
}

void* LeafNodeCell(void* node, uint32_t cellNum) {
	return (char*)node + LEAF_NODE_HEADER_SIZE + cellNum * LEAF_NODE_CELL_SIZE;
}

uint32_t* LeafNodeKey(void* node, uint32_t cellNum) {
	return (uint32_t*)LeafNodeCell(node, cellNum);
}

void* LeafNodeValue(void* node, uint32_t cellNum) {
	return (char*)LeafNodeCell(node, cellNum) + LEAF_NODE_KEY_SIZE;
}

uint32_t* InternalNodeNumKeys(void* node) {
	return (uint32_t*)((char*)node + INTERNAL_NODE_NUM_KEYS_OFFSET);
}

uint32_t* InternalNodeRightChild(void* node) {
	return (uint32_t*)((char*)node + INTERNAL_NODE_RIGHT_CHILD_OFFSET);
}

uint32_t* InternalNodeCell(void* node, uint32_t cellNum) {
	return (uint32_t*)((char*)node + INTERNAL_NODE_HEADER_SIZE + cellNum * INTERNAL_NODE_CELL_SIZE);
}

uint32_t* InternalNodeChild(void* node, uint32_t childNum) {
	uint32_t numKeys = *InternalNodeNumKeys(node);
	if (childNum > numKeys) {
		cout << "Tried to access child_num " << childNum << " > num_keys " << numKeys << endl;
		exit(EXIT_FAILURE);
	}
	else if (childNum == numKeys) {
		return InternalNodeRightChild(node);
	}
	else {
		return InternalNodeCell(node, childNum);
	}
}

uint32_t* InternalNodeKey(void* node, uint32_t keyNum) {
	return (uint32_t*)((char*)node + INTERNAL_NODE_HEADER_SIZE + keyNum * INTERNAL_NODE_CELL_SIZE + INTERNAL_NODE_CHILD_SIZE);
}

void setNodeType(void* node, NodeType type) {
	uint8_t value = type;
	*((uint8_t*)((char*)node + NODE_TYPE_OFFSET)) = value;
}

void setNodeRoot(void* node, bool isRoot) {
	uint8_t value = isRoot ? 1 : 0;
	*((uint8_t*)((char*)node + IS_ROOT_OFFSET)) = value;
}

void InitLeafNode(void* node) {
	setNodeType(node, NODE_LEAF);
	setNodeRoot(node, false);
	*LeafNodeNumCells(node) = 0;
}

void InitInternalNode(void* node) {
	setNodeType(node, NODE_INTERNAL);
	setNodeRoot(node, false);
	*InternalNodeNumKeys(node) = 0;
}

uint32_t getMaxLeafNodeKey(void* node) {
	return *LeafNodeKey(node, *LeafNodeNumCells(node) - 1);
}

void createNewRoot(Table* table, uint32_t rightChildPageNum) {
	void* root = GetPageBTree(table->pager, table->rootPageNum);
	void* rightChild = GetPageBTree(table->pager, rightChildPageNum);
	uint32_t leftChildPageNum = getUnusedPageNum(table->pager);
	void* leftChild = GetPageBTree(table->pager, leftChildPageNum);

	memcpy(leftChild, root, PAGE_SIZE);
	setNodeRoot(leftChild, false);
	InitInternalNode(root);
	setNodeRoot(root, true);
	*InternalNodeNumKeys(root) = 1;
	*InternalNodeChild(root, 0) = leftChildPageNum;
	uint32_t leftChildMaxKey = getMaxLeafNodeKey(leftChild);
	*InternalNodeKey(root, 0) = leftChildMaxKey;
	*InternalNodeRightChild(root) = rightChildPageNum;
}

void InsertIntoLeafNode(Cursor* cursor, uint32_t key, Row* value) {
		
	void* node = GetPageBTree(cursor->table->pager, cursor->pageNum);

	uint32_t numCells = *LeafNodeNumCells(node);
	if (numCells >= LEAF_NODE_MAX_CELLS) {
		cout << "Split nodes\n";
		exit(EXIT_FAILURE);
	}

	if (cursor->cellNum < numCells)
		for (uint32_t i = numCells; i > cursor->cellNum; i--)
			memcpy(LeafNodeCell(node, i), LeafNodeCell(node, i - 1), LEAF_NODE_CELL_SIZE);
		
	*(LeafNodeNumCells(node)) += 1;
	*(LeafNodeKey(node, cursor->cellNum)) = key;
	SerializeRow(value, (char*)LeafNodeValue(node, cursor->cellNum));
}

#endif