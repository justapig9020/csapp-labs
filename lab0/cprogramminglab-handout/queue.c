/**
 * @file queue.c
 * @brief Implementation of a queue that supports FIFO and LIFO operations.
 *
 * This queue implementation uses a singly-linked list to represent the
 * queue elements. Each queue element stores a string value.
 *
 * Assignment for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 * Extended to store strings, 2018
 *
 * @author justapig9020 <justapig9020@gmail.com>
 */

#include "queue.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocates a new element
 * @param[in] s The string to copied and hold by the new element
 * @return The new element, or NULL if memory allocation failed
 */
static list_ele_t *ele_new(const char *s) {
    // One addition byte for terminating NULL character
    size_t n = strlen(s) + 1;
    char *value = malloc(sizeof(char) * n);
    if (!value)
        return NULL;

    list_ele_t *ele = malloc(sizeof(*ele));
    if (!ele) {
        free(value);
        return NULL;
    }

    strncpy(value, s, n);
    ele->next = NULL;
    ele->value = value;
    return ele;
}

/**
 * @brief Frees all memory used by a element
 * @param[in] e The element to free
 * @return NULL if e is NULL, otherwise the value that the element holds
 */
char *ele_free(list_ele_t *e) {
    if (!e)
        return NULL;
    char *value = e->value;
    free(e);
    return value;
}

/**
 * @brief Allocates a new queue
 * @return The new queue, or NULL if memory allocation failed
 */
queue_t *queue_new(void) {
    queue_t *q = malloc(sizeof(queue_t));
    /* What if malloc returned NULL? */
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = &q->head;
    q->size = 0;
    return q;
}

/**
 * @brief Frees all memory used by a queue
 * @param[in] q The queue to free
 */
void queue_free(queue_t *q) {
    if (!q)
        return;
    list_ele_t *ptr = q->head;
    while (ptr) {
        list_ele_t *buf = ptr;
        ptr = ptr->next;
        char *value = ele_free(buf);
        free(value);
    }
    free(q);
}

/**
 * @brief Attempts to insert an element at head of a queue
 *
 * This function explicitly allocates space to create a copy of `s`.
 * The inserted element points to a copy of `s`, instead of `s` itself.
 *
 * @param[in] q The queue to insert into
 * @param[in] s String to be copied and inserted into the queue
 *
 * @return true if insertion was successful
 * @return false if q is NULL, or memory allocation failed
 */
bool queue_insert_head(queue_t *q, const char *s) {
    if (!q)
        return false;

    list_ele_t *newh = ele_new(s);
    if (!newh)
        return false;

    newh->next = q->head;
    q->head = newh;
    if (0 == q->size)
        q->tail = &newh->next;
    q->size += 1;
    return true;
}

/**
 * @brief Attempts to insert an element at tail of a queue
 *
 * This function explicitly allocates space to create a copy of `s`.
 * The inserted element points to a copy of `s`, instead of `s` itself.
 *
 * @param[in] q The queue to insert into
 * @param[in] s String to be copied and inserted into the queue
 *
 * @return true if insertion was successful
 * @return false if q is NULL, or memory allocation failed
 */
bool queue_insert_tail(queue_t *q, const char *s) {
    if (!q)
        return false;

    list_ele_t *newt = ele_new(s);
    if (!newt)
        return false;

    *q->tail = newt;
    q->tail = &newt->next;
    q->size += 1;
    return true;
}

/**
 * @brief Attempts to remove an element from head of a queue
 *
 * If removal succeeds, this function frees all memory used by the
 * removed list element and its string value before returning.
 *
 * If removal succeeds and `buf` is non-NULL, this function copies up to
 * `bufsize - 1` characters from the removed string into `buf`, and writes
 * a null terminator '\0' after the copied string.
 *
 * @param[in]  q       The queue to remove from
 * @param[out] buf     Output buffer to write a string value into
 * @param[in]  bufsize Size of the buffer `buf` points to
 *
 * @return true if removal succeeded
 * @return false if q is NULL or empty
 */
bool queue_remove_head(queue_t *q, char *buf, size_t bufsize) {
    if (!q || q->size == 0)
        return false;

    list_ele_t *oldh = q->head;
    q->head = q->head->next;
    q->size -= 1;
    if (0 == q->size)
        q->tail = &q->head;

    char *value = ele_free(oldh);
    if (0 < bufsize) {
        strncpy(buf, value, bufsize - 1);
        buf[bufsize - 1] = '\0';
    }
    free(value);

    return true;
}

/**
 * @brief Returns the number of elements in a queue
 *
 * This function runs in O(1) time.
 *
 * @param[in] q The queue to examine
 *
 * @return the number of elements in the queue, or
 *         0 if q is NULL or empty
 */
size_t queue_size(queue_t *q) {
    if (!q)
        return 0;
    return q->size;
}

/**
 * @brief Reverse the elements in a queue
 *
 * This function does not allocate or free any list elements, i.e. it does
 * not call malloc or free, including inside helper functions. Instead, it
 * rearranges the existing elements of the queue.
 *
 * @param[in] q The queue to reverse
 */
void queue_reverse(queue_t *q) {
    if (!q || 0 == q->size)
        return;
    list_ele_t *prev = NULL;
    list_ele_t *curr = q->head;
    list_ele_t *next;
    q->tail = &curr->next;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    q->head = prev;
}
