#ifndef __STD_ARRAY_H__
#define __STD_ARRAY_H__

#include "std/types.h"
#include "std/memory.h"

typedef struct Array {
  uint8_t *data;

  size_t size;
  size_t elemSize;
  size_t reserved;

  bool zeroed;
  FreeFuncT freeFunc;
  CompareFuncT compareFunc;

  PtrT temporary;
} ArrayT;

/*
 * @brief Array constructor.
 */
ArrayT *NewArray(size_t size, size_t elemSize, bool zeroed);

/*
 * @brief Sets a comparison function useful for sorting.
 */
static inline void ArraySetCompareFunc(ArrayT *self, CompareFuncT func) {
  self->compareFunc = func;
}

static inline void ArraySetFreeFunc(ArrayT *self, FreeFuncT func) {
  self->freeFunc = func;
}

/*
 * @brief Getter & setter functions.
 */

/*
 * @brief Get a pointer to an element without extra checks.
 *
 * Assumes that index is non-negative.  Can return an invalid pointer.
 */
static inline PtrT ArrayGet(ArrayT *self, size_t index) {
  return &self->data[self->elemSize * index];
}

static inline PtrT ArraySet(ArrayT *self, size_t index, PtrT data) {
  return MemCopy(ArrayGet(self, index), data, self->elemSize);
}

/*
 * @brief Iteration functions.
 */
void ArrayForEach(ArrayT *self, IterFuncT func, PtrT data);
void ArrayForEachInRange(ArrayT *self, ssize_t begin, ssize_t end,
                         IterFuncT func, PtrT data);

/*
 * @brief Element adding functions.
 */
PtrT ArrayInsertFast(ArrayT *self, ssize_t index, PtrT data);
PtrT ArrayInsert(ArrayT *self, ssize_t index, PtrT data);
PtrT ArrayInsertElements(ArrayT *self, ssize_t index, PtrT data, size_t count);
PtrT ArrayAppend(ArrayT *self, PtrT data);
PtrT ArrayAppendElements(ArrayT *self, PtrT data, size_t count);

/*
 * @brief Remove a pointer and fill in the gap with last pointer in the array.
 *
 * Assumes that index is non-negative.
 */
void ArrayRemoveFast(ArrayT *self, size_t index);
void ArrayRemove(ArrayT *self, ssize_t index);
void ArrayRemoveRange(ArrayT *self, ssize_t first, ssize_t last);
void ArrayFilterFast(ArrayT *self, PredicateT func);

/*
 * Array size management functions.
 */
void ArrayResize(ArrayT *self, size_t newSize);

/*
 * Sorting functions.
 */
void ArrayInsertionSort(ArrayT *self, ssize_t begin, ssize_t end);
size_t ArrayPartition(ArrayT *self,
                      size_t begin, size_t end, PtrT pivot);
void ArrayQuickSort(ArrayT *self, ssize_t begin, ssize_t end);

#endif
