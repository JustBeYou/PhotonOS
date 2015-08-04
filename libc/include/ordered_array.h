#ifndef _ordered_array_h
#define _ordered_array_h

#include <system.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t (*lessthan_pred_t)(type_t, type_t);

typedef struct {
	type_t *array;
	uint32_t size;
	uint32_t max_size;
	lessthan_pred_t lessthan;
} ordered_array_t;


/**
  * Standard 'less than' checker.
  */
int8_t std_lessthan_pred(type_t a, type_t b);

/**
  * Create an ordered array.
  */
ordered_array_t create_ordered_array(uint32_t max_size, lessthan_pred_t lessthan);

/**
  * Place an ordered array at specified location.
  */
ordered_array_t place_ordered_array(void *addr, uint32_t max_size, lessthan_pred_t lessthan);

/**
  * Destroy the ordered array.
  */
void destroy_ordered_array(ordered_array_t *array);

/**
  * Insert an item in the array.
  */
void insert_ordered_array(type_t item, ordered_array_t *array);

/**
  * Look up at index 'i' for item.
  */
type_t lookup_ordered_array(uint32_t i, ordered_array_t *array);

/**
  * Remove item from array.
  */
void remove_ordered_array(uint32_t i, ordered_array_t *array);

#endif
