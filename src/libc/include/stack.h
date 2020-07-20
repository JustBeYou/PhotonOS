#ifndef _stack_h
#define _stack_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>


#define STACK_TOP_POP_PROCESSING 1
#define STACK_TOP_PROCESSING_POP 2

typedef struct Stack*   STACK;
typedef int             STACK_CODE;
typedef void*           STACK_ITEM;
typedef int             STACK_PROCESSING_TYPE;

typedef void        (*stack_processesItemFn)(STACK_ITEM);
typedef void        (*stack_freeItemFn)(STACK_ITEM);
typedef STACK_ITEM  (*stack_copyItemFn)(const STACK_ITEM);

struct Stack
{
    struct StackNode
    {
        struct StackNode *next;
        void *info;
    } *first;
    stack_freeItemFn free;
    stack_copyItemFn copy;
};

#ifdef __cplusplus
extern "C" {
#endif

STACK       stack_init(stack_freeItemFn, stack_copyItemFn);
STACK       stack_copy(const STACK);
STACK_CODE  stack_pop(STACK);
STACK_CODE  stack_push(STACK, STACK_ITEM);
STACK_CODE  stack_empty(const STACK);
STACK_CODE  stack_delete(STACK*);
STACK_CODE  stack_for_each(STACK, STACK_PROCESSING_TYPE, stack_processesItemFn);
STACK_CODE  stack_reverse(STACK);
STACK_ITEM  stack_head(const STACK);

#ifdef __cplusplus
}
#endif

/*
 * errors codes
 */
#define STACK_NULL_POINTER              0
#define STACK_NOT_EMPTY                 0
#define STACK_EMPTY                     1
#define STACK_NO_ERROR                  2
#define STACK_MALLOC_ERROR              3
#define STACK_INVALID_PROCESSING_TYPE   4

#define STACK_FOR_EACH(stack, type, func, ...)                  \
        do                                                      \
        {                                                       \
            if (type == STACK_TOP_PROCESSING_POP)               \
                while (stack_empty(stack) == STACK_NOT_EMPTY)   \
                {                                               \
                    func(stack_head(stack), ##__VA_ARGS__);      \
                    stack_pop(stack);                           \
                }                                               \
            if (type == STACK_TOP_POP_PROCESSING)               \
            {                                                   \
                stack_freeItemFn fn = stack->free;              \
                stack->free = NULL;                             \
                while (stack_empty(stack) == STACK_NOT_EMPTY)   \
                {                                               \
                    void *item = stack_head(stack);              \
                    stack_pop(stack);                           \
                    func(item, ##__VA_ARGS__);                  \
                    if (fn)                                     \
                        fn(item);                               \
                }                                               \
                stack->free = fn;                               \
            }                                                   \
        }                                                       \
        while (0)

#endif
