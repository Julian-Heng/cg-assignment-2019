#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "list.h"


static void linkMethods(List*);

static void insertFirst(List*, void*, bool);
static void insertLast(List*, void*, bool);
static void removeFirst(List*, void**, bool*);
static void removeLast(List*, void**, bool*);
static void peekFirst(List*, void**, bool*);
static void peekLast(List*, void**, bool*);
static void deleteList(List**);

static void peek(ListNode*, void**, bool*);
static void deleteNode(ListNode**);

static ListNode* newNode(void* value, bool isMalloc)
{
    ListNode* node;

    if (! (node = (ListNode*)malloc(sizeof(ListNode))))
    {
        fprintf(stderr, ERR_NODE_MALLOC);
        return NULL;
    }

    memset(node, 0, sizeof(ListNode));
    node->next = NULL;
    node->prev = NULL;
    node->value = value;
    node->isMalloc = isMalloc;

    return node;
}


List* newList()
{
    List* list;

    if (! (list = (List*)malloc(sizeof(List))))
    {
        fprintf(stderr, ERR_LIST_MALLOC);
        return NULL;
    }

    memset(list, 0, sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    linkMethods(list);

    return list;
}


static void linkMethods(List* this)
{
    this->insertFirst = insertFirst;
    this->insertLast = insertLast;
    this->removeFirst = removeFirst;
    this->removeLast = removeLast;
    this->peekFirst = peekFirst;
    this->peekLast = peekLast;
    this->deleteList = deleteList;
}


static void insertFirst(List* this, void* value, bool isMalloc)
{
    ListNode* node;

    if ((node = newNode(value, isMalloc)))
    {
        if (this->length == 0)
        {
            this->head = node;
            this->tail = node;
        }
        else
        {
            this->head->prev = node;
            node->next = this->head;
            this->head = node;
        }

        this->length++;
    }
}


static void insertLast(List* this, void* value, bool isMalloc)
{
    ListNode* node;

    if ((node = newNode(value, isMalloc)))
    {
        if (this->length == 0)
        {
            this->head = node;
            this->tail = node;
        }
        else
        {
            node->prev = this->tail;
            this->tail->next = node;
            this->tail = node;
        }

        this->length++;
    }
}


static void removeFirst(List* this, void** dest, bool* isMalloc)
{
    ListNode* node = this->head;
    *dest = NULL;
    *isMalloc = false;

    if (node)
    {
        peek(node, dest, isMalloc);

        if (this->length == 1)
        {
            this->head = NULL;
            this->tail = NULL;
        }
        else
        {
            this->head = this->head->next;
        }

        deleteNode(&node);
        this->length--;
    }
}


static void removeLast(List* this, void** dest, bool* isMalloc)
{
    ListNode* node = this->tail;
    *dest = NULL;
    *isMalloc = false;

    if (node)
    {
        peek(node, dest, isMalloc);

        if (this->length == 1)
        {
            this->head = NULL;
            this->tail = NULL;
        }
        else
        {
            this->tail = this->tail->prev;
        }

        deleteNode(&node);
        this->length--;
    }
}


static void peekFirst(List* this, void** dest, bool* isMalloc)
{
    peek(this->head, dest, isMalloc);
}


static void peekLast(List* this, void** dest, bool* isMalloc)
{
    peek(this->tail, dest, isMalloc);
}


static void deleteList(List** this)
{
    void* value;
    bool isMalloc;

    while ((*this)->head)
    {
        removeLast(*this, &value, &isMalloc);

        if (isMalloc)
        {
            SAFE_FREE(value);
        }
    }

    SAFE_FREE(*this);
}


static void peek(ListNode* node, void** dest, bool* isMalloc)
{
    if (node)
    {
        *dest = node->value;
        *isMalloc = node->isMalloc;
    }
}


static void deleteNode(ListNode** node)
{
    SAFE_FREE(*node);
}
