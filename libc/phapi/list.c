#include <list.h>
#include <stdlib.h>
#include <kheap.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

int8_t std_lessthan_pred(type_t a, type_t b)
{
	return (a < b) ? 1 : 0;
}

list_t create_list(uint32_t max_size, lessthan_pred_t lessthan)
{
	list_t ret;
	ret.array = (void*) kmalloc(max_size * sizeof(type_t));
	memset(ret.array, 0, max_size * sizeof(type_t));
	ret.size = 0;
	ret.max_size = max_size;
	ret.lessthan = lessthan;
	return ret;
}

list_t place_list(void *addr, uint32_t max_size, lessthan_pred_t lessthan)
{
	list_t ret;
	ret.array = (type_t*) addr;
	memset(ret.array, 0, max_size * sizeof(type_t));
	ret.size = 0;
	ret.max_size = max_size;
	ret.lessthan = lessthan;
	return ret;
}

void destroy_list(list_t *array)
{
	kfree(array->array);
}

void insert_list(type_t item, list_t *array)
{
	ASSERT(array->lessthan);
	uint32_t iterator = 0;
	while (iterator < array->size && array->lessthan(array->array[iterator], item)) {
		iterator++;
	}
	
	if (iterator == array->size) {
		array->array[array->size++] = item;
	} else {
		type_t tmp = array->array[iterator];
		array->array[iterator] = item;
		while (iterator < array->size) {
			iterator++;
			type_t tmp2 = array->array[iterator];
			array->array[iterator] = tmp;
			tmp = tmp2;
		}
		array->size++;
	}
}

type_t lookup_list(uint32_t i, list_t *array)
{
	ASSERT(i < array->size);
	return array->array[i];
}

void remove_list(uint32_t i, list_t *array)
{
	while (i < array->size) {
		array->array[i] = array->array[i + 1];
		i++;
	}
	array->size--;
}
