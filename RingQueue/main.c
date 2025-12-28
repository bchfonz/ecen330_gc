#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

QUEUE_t m_queue; // create our queue

int main(void)
{
  //_e is for enum
  QUEUE_ERR_e queueErr;

  /* Initialize the queue and confirm it works */
  if (queueErr = QUEUE_init(&m_queue))
  {
    printf("The queue failed to initialize! Code: %d\n", (uint8_t)queueErr);
  }

  /* Test every one of your queue functions here as d*/
  return 0;
}