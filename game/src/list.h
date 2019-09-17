#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#define ERR_NODE_MALLOC "Cannot allocate memory for node\n"
#define ERR_LIST_MALLOC "Cannot allocate memory for list\n"

typedef struct ListNode
{
    struct ListNode* next;
    struct ListNode* prev;
    void* value;
    bool isMalloc;
} ListNode;


typedef struct List
{
    ListNode* head;
    ListNode* tail;
    int length;

    void (*insertFirst)(struct List*, void*, bool);
    void (*insertLast)(struct List*, void*, bool);
    void (*removeFirst)(struct List*, void**, bool*);
    void (*removeLast)(struct List*, void**, bool*);
    void (*peekFirst)(struct List*, void**, bool*);
    void (*peekLast)(struct List*, void**, bool*);
    void (*peekAt)(struct List*, int, void**, bool*);
    void (*deleteList)(struct List**);
} List;

List* newList(void);

#endif
