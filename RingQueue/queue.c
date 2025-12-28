/**
 * COMPLETE EACH AND EVERY FUNCTION. CONVINCE YOURSELF THAT THEY WORK
 * BY USING MAIN.C.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

/**
 * Initialize the queue by setting all pointers to 0.
 * Return: QUEUE_ERR_e error code if initialization fails.
 */
QUEUE_ERR_e QUEUE_init(QUEUE_t *queue)
{
  QUEUE_ERR_e queueErr = QUEUE_ERR_NONE;
  if (NULL != queue)
  {
    queue->head = 0;
    queue->tail = 0;
    queue->full = false;
  }
  else
  {
    queueErr = QUEUE_ERR_NULL_POINTER;
  }
  return queueErr;
}

/**
 * Check that the queue is empty, which effectively means
 * both pointers are equal to eachother.
 * Returns: TRUE if empty and FALSE if not empty
 */
bool QUEUE_check_empty(QUEUE_t *queue)
{
  bool isEmpty;
  if (queue->head == queue->tail && !queue->full == true)
  {
    isEmpty = true;
  }
  else
  {
    isEmpty = false;
    queue->full = true;
  }
  return isEmpty;
}

/**
 * Check to see if the queue is full, which effectively means
 * the magnitude of the difference between the pointers will
 * be equal to the size of QUEUE_NUM_ELEMENTS
 * Returns: TRUE if full, FALSE if not
 */
bool QUEUE_check_full(QUEUE_t *queue)
{
  bool isFull;

  if (queue->head == queue->tail && queue->full == true)
  {
    isFull = true;
  }
  else
  {
    isFull = false;
    queue->full = false;
  }
  return isFull;
}

/**
 * Return a count of the number of elements currently pushed
 * into the queue.
 */
uint16_t QUEUE_count_elements(QUEUE_t *queue)
{
  uint16_t i;
  uint8_t count = 0;
  for (i = 0; i < QUEUE_NUM_ELEMENTS; i++)
  {
    if ((queue->head + i) % 10 == queue->tail)
    {
      break;
    }
    count++;
  }
  return count;
}

/**
 * Push a queue element into the queue. This means an element
 * must have been created before doing this.
 * Returns: QUEUE_ERR_E
 */
QUEUE_ERR_e QUEUE_push_elem(QUEUE_t *queue, QUEUE_ELEMENT_t element)
{
  QUEUE_ERR_e isError;
  if (queue == NULL)
  {
    isError = QUEUE_ERR_NULL_POINTER;
  }
  else if (queue->full == true)
  {
    isError = QUEUE_ERR_OVERFLOW;
  }
  else
  {
    if ((QUEUE_check_full(queue) == false))
    {
      queue->elements[queue->head] = element;
      queue->head = (queue->head + 1) % 10;
      isError = QUEUE_ERR_NONE;
      if (queue->head == queue->tail)
      {
        queue->full = true;
      }
    }
  }
  return isError;
}

/**
 * Construct a message directly and push it into the queue.
 * Returns: QUEUE_ERR_e
 */
QUEUE_ERR_e QUEUE_push_msg(QUEUE_t *queue, QUEUE_SENDER_e sender, QUEUE_PRIORITY_e priority, char *msg_)
{
  QUEUE_ERR_e pushMsgErr;
  QUEUE_ELEMENT_t *element;
  element->sender = sender;
  element->priority = priority;
  // What's wrong with this??
  strcpy(element->message, msg_);
  return QUEUE_push_elem(queue, *element);
}

/**
 * Pop an element off the queue. This will put the popped element into
 * a QUEUE_ELEMENT_t struct variable you have created.
 * Returns: QUEUE_ERR_e error code if a problem occurs.
 */
QUEUE_ERR_e QUEUE_pop_elem(QUEUE_t *queue, QUEUE_ELEMENT_t *element)
{
  QUEUE_ERR_e isError;
  if (queue == NULL || element == NULL)
  {
    isError = QUEUE_ERR_NULL_POINTER;
  }
  else if (queue->head == queue->tail && !queue->full)
  {
    isError = QUEUE_ERR_UNDERFLOW;
  }
  else
  {
    *element = queue->elements[queue->tail];
    queue->tail = (queue->tail + 1) % 10;
    queue->full = false;
    isError = QUEUE_ERR_NONE;
  }
  return isError;
}
