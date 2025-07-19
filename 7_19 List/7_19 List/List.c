#include "List.h"


LTNode* BuyLTNode(LTDataType x)
{
	LTNode* newnode = (LTNode*)malloc(sizeof(LTNode));
	if (newnode == NULL)
	{
		perror("malloc fail");
		return NULL;
	}

	newnode->data = x;
	newnode->next = NULL;
	newnode->prev = NULL;
	return newnode;

}

LTNode* LTInit()
{
	LTNode* phead = BuyLTNode(-1);
	phead->next = phead;
	phead->prev = phead;

	return phead;
}

bool LTEmpty(LTNode* phead)
{
	assert(phead);

	return phead->next == phead;
}

void LTPushBack(LTNode* phead, LTDataType x)
{
	assert(phead);
	LTNode* tail = phead->prev;
	LTNode* newnode = BuyLTNode(x);

	tail->next = newnode;
	newnode->prev = tail;
	newnode->next = phead;
	phead->prev = newnode;
}

void LTPushFront(LTNode* phead, LTDataType x)
{
	assert(phead);
	LTNode* first = phead->next;
	LTNode* newnode = BuyLTNode(x);

	phead->next = newnode;
	newnode->prev = phead;
	newnode->next = first;
	first->prev = newnode;

}

void LTPopBack(LTNode* phead)
{
	assert(phead);
	assert(phead->next != phead);//·ÀÖ¹°ÑÉÚ±øÎ»É¾µô
	LTNode* tail = phead->prev;
	LTNode* tailprev = tail->prev;
	free(tail);

	phead->prev = tailprev;
	tailprev->next = phead;
}

void LTPopFront(LTNode* phead)
{
	assert(phead);
	assert(phead->next != phead);
	LTNode* first = phead->next;
	LTNode* firstnext = first->next;
	free(first);

	firstnext->prev = phead;
	phead->next = firstnext;
}

LTNode* LTFind(LTNode* phead, LTDataType x)
{
	assert(phead);
	LTNode* cur = phead->next;
	while (cur != phead)
	{
		if (cur->data == x)
		{
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}

void LTInsert(LTNode* pos, LTDataType x)
{
	assert(pos);
	LTNode* newnode = BuyLTNode(x);
	LTNode* posprev = pos->prev;

	newnode->prev = posprev;
	posprev->next = newnode;

	newnode->next = pos;
	pos->prev = newnode;
}

void LTErase(LTNode* pos)
{
	assert(pos);
	LTNode* posprev = pos->prev;
	LTNode* posnext = pos->next;
	free(pos);

	posprev->next = posnext;
	posnext->prev = posprev;
}

void LTDestroy(LTNode* phead)
{
	assert(phead);
	LTNode* cur = phead->next;
	while (cur != phead)
	{
		LTNode* next = cur->next;
		free(cur);
		cur = next;
	}
	free(phead);
}

void LTPrint(LTNode* phead)
{
	assert(phead);

	printf("guard<==>");
	LTNode* cur = phead->next;
	while (cur != phead)
	{
		printf("%d<==>", cur->data);
		cur = cur->next;
	}
	printf("\n");
}