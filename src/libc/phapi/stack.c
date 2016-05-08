#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

STACK_CODE stack_delete(STACK *stack)
{
    if (stack == NULL)
        return STACK_NULL_POINTER;
    if (*stack == NULL)
        return STACK_NULL_POINTER;

    while (stack_empty(*stack) == STACK_NOT_EMPTY)
        stack_pop(*stack);

    free(*stack);
    *stack = NULL;

    return STACK_NO_ERROR;
}

STACK stack_init(stack_freeItemFn freeItem, stack_copyItemFn copyItem)
{
    STACK stack = (STACK) malloc(sizeof(struct Stack));
    if (stack == NULL)
        return NULL;

    stack->first = NULL;
    stack->free = freeItem;
    stack->copy = copyItem;

    return stack;
}

STACK_CODE stack_empty(const STACK stack)
{
    if (stack == NULL)
        return STACK_NULL_POINTER;

    if (stack->first == NULL)
        return STACK_EMPTY;
    else
        return STACK_NOT_EMPTY;
}

STACK_CODE stack_push(STACK stack, STACK_ITEM item)
{
    if (stack == NULL)
        return STACK_NULL_POINTER;
    if (item == NULL)
        return STACK_NULL_POINTER;

    struct StackNode *node = (struct StackNode*) malloc(sizeof(struct StackNode));
    if (node == NULL)
        return STACK_MALLOC_ERROR;

    node->info = item;
    node->next = stack->first;
    stack->first = node;

    return STACK_NO_ERROR;
}

STACK_CODE stack_pop(STACK stack)
{
    if (stack == NULL)
        return STACK_NULL_POINTER;

    if (stack->first == NULL)
        return STACK_EMPTY;

    struct StackNode *node = stack->first;
    stack->first = stack->first->next;

    if (stack->free != NULL)
        stack->free(node->info);

    node->info = NULL;
    free(node);
    node = NULL;

    return STACK_NO_ERROR;
}

STACK_ITEM stack_head(const STACK stack)
{
    if (stack == NULL)
        return NULL;

    if (stack->first == NULL)
        return NULL;

    return stack->first->info;
}

STACK_CODE stack_for_each(STACK stack, STACK_PROCESSING_TYPE type,
                          stack_processesItemFn func)
{
    if (stack == NULL)
        return STACK_NULL_POINTER;
    if (func == NULL)
        return STACK_NULL_POINTER;
    if (type != STACK_TOP_POP_PROCESSING && type != STACK_TOP_PROCESSING_POP)
        return STACK_INVALID_PROCESSING_TYPE;

    if (type == STACK_TOP_PROCESSING_POP)
        while (stack_empty(stack) == STACK_NOT_EMPTY)
        {
            func(stack_head(stack));
            stack_pop(stack);
        }
    else
    {
        stack_freeItemFn fn = stack->free;
        stack->free = NULL;

        while (stack_empty(stack) == STACK_NOT_EMPTY)
        {
            void *item = stack_head(stack);
            stack_pop(stack);
            func(item);

            if (fn)
                fn(item);
        }

        stack->free = fn;
    }

    return STACK_NO_ERROR;
}

STACK stack_copy(const STACK stack)
{
    if (stack == NULL)
        return NULL;
    if (stack->copy == NULL)
        return NULL;

    STACK newStack = (STACK) malloc(sizeof(struct Stack));
    if (newStack == NULL)
        return NULL;

    newStack->first = NULL;
    newStack->free = stack->free;
    newStack->copy = stack->copy;

    if (stack->first)
    {
        newStack->first = (struct StackNode*) malloc(sizeof(struct StackNode));
        if (newStack->first == NULL)
        {
            stack_delete(&newStack);
            return NULL;
        }

        newStack->first->next = NULL;
        newStack->first->info = stack->copy(stack->first->info);
    }
    else
        return newStack;

    struct StackNode *sn  = stack->first->next;
    struct StackNode *nsn = newStack->first;

    while (sn)
    {
        nsn->next = (struct StackNode*) malloc(sizeof(struct StackNode));
        if (nsn->next == NULL)
        {
            stack_delete(&newStack);
            return NULL;
        }

        nsn->next->info = stack->copy(sn->info);
        nsn->next->next = NULL;
        nsn = nsn->next;
        sn  = sn->next;
    }

    return newStack;
}

STACK_CODE stack_reverse(STACK stack)
{
    if (stack == NULL)
        return STACK_NULL_POINTER;
    if (stack->first == NULL)
        return STACK_EMPTY;
    if (stack->first->next == NULL)
        return STACK_NO_ERROR;

    struct StackNode *sn = stack->first;
    struct StackNode *newFirst = NULL;
    struct StackNode *copy;
    while (sn)
    {
        copy = sn;
        sn = sn->next;
        copy->next = newFirst;
        newFirst = copy;
    }

    stack->first = newFirst;

    return STACK_NO_ERROR;
}

