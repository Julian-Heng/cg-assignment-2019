#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#define ERR_NODE_MALLOC "Error: unable to allocate memory for node\n"
#define ERR_LIST_MALLOC "Error: unable to allocate memory for list\n"

#define LIST_FOR_EACH(l, i) for ((i) = (l)->head; (i); (i) = (i)->next)

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
    void (*deleteListShallow)(struct List**);
} List;

List* newList(void);

#endif
