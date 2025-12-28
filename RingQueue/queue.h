#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_NUM_ELEMENTS 10
#define QUEUE_MESSAGE_SIZE 10

/**
 * Types of errors that might occur when using the queue functions
 */
typedef enum
{
  QUEUE_ERR_NONE,         // no error occurred
  QUEUE_ERR_NULL_POINTER, // a null pointer was encountered
  QUEUE_ERR_OVERFLOW,     // the queue is full and the push was ignored
  QUEUE_ERR_UNDERFLOW,    // the queue is empty and the pop was ignored
  QUEUE_ERR_OTHER         // a catch-all for some other error
} QUEUE_ERR_e;

/**
 * Types of senders that might use the message queue. These might
 * be used to tell a receiver from whence the message comes.
 */
typedef enum
{
  QUEUE_SENDER_TASK1,
  QUEUE_SENDER_TASK2,
  QUEUE_SENDER_TASK3
} QUEUE_SENDER_e;

/**
 * Types of message priorities. These might be used to tell a
 * receiver that a message is urgent (or not).
 */
typedef enum
{
  QUEUE_PRIORITY_NORMAL,
  QUEUE_PRIORITY_HIGH
} QUEUE_PRIORITY_e;

/**
 * The basic structure of a queue element. Each item pushed or
 * popped from the queue will take this form.
 */
typedef struct
{
  QUEUE_SENDER_e sender;
  QUEUE_PRIORITY_e priority;
  char message[QUEUE_MESSAGE_SIZE];
} QUEUE_ELEMENT_t;

typedef struct
{
  uint16_t head; // the pointer to the head (push) of the buffer
  uint16_t tail; // the pointer to the tail (pop) of the buffer
  bool full;     // a flag to needed for knowing if it's full
  QUEUE_ELEMENT_t elements[QUEUE_NUM_ELEMENTS];
} QUEUE_t;

/**
 * Initialize the queue by setting all pointers to 0.
 * Return: QUEUE_ERR_e error code if initialization fails.
 */
QUEUE_ERR_e QUEUE_init(QUEUE_t *queue);

/**
 * Check that the queue is empty, which effectively means
 * both pointers are equal to eachother.
 * Returns: TRUE if empty and FALSE if not empty
 */
bool QUEUE_check_empty(QUEUE_t *queue);

/**
 * Check to see if the queue is full, which effectively means
 * the magnitude of the difference between the pointers will
 * be equal to the size of QUEUE_NUM_ELEMENTS
 * Returns: TRUE if full, FALSE if not
 */
bool QUEUE_check_full(QUEUE_t *queue);

/**
 * Return a count of the number of elements currently pushed
 * into the queue.
 */
uint16_t QUEUE_count_elements(QUEUE_t *queue);

/**
 * Push a queue element into the queue. This means an element
 * must have been created before doing this.
 * Returns: QUEUE_ERR_E
 */
QUEUE_ERR_e QUEUE_push_elem(QUEUE_t *queue, QUEUE_ELEMENT_t element);

/**
 * Construct a message directly and push it into the queue.
 * Returns: QUEUE_ERR_e
 */
QUEUE_ERR_e QUEUE_push_msg(QUEUE_t *queue, QUEUE_SENDER_e sender, QUEUE_PRIORITY_e priority, char *msg_);

/**
 * Pop an element off the queue. This will put the popped element into
 * a QUEUE_ELEMENT_t struct variable you have created.
 * Returns: QUEUE_ERR_e error code if a problem occurs.
 */
QUEUE_ERR_e QUEUE_pop_elem(QUEUE_t *queue, QUEUE_ELEMENT_t *element);
#endif