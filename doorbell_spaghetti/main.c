/**
You are designing a synchSM that rings a bell when a button is pushed.  Your system must meet the following requirements:
•	Respond to all button presses that last 300 ms or more (presses that last less than 300ms can safely be ignored).
•	If the button is released and pressed again, the system should detect that a separate press occurred if the button was released for more than 250 ms (releases that last less than 250ms can safely be ignored).
•	The maximum latency from a button press to the bell ring is 120 ms.  
•	The bell should ring for 500 ms, or until released, whichever is longer. 

NOTE: There is not buzzer in this demo. Please use MYGPIO_writeLed() 
for this purpose
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TIMER_PERIOD_MS     50lu
#define TIMER_PERIOD        (1000lu * TIMER_PERIOD_MS) /* microseconds */
#define MIN_BTN_PRESS_CNT   1
#define MIN_BTN_RELEASE_CNT 1
#define BUZZER_DURATION     1

/**********************************************************************
 * Temporary spaghetti until Wokwi bug is fixed
 *********************************************************************/
#define MYGPIO_NUM_LEDS      10
#define MYGPIO_LED_MASK      (UINT16_MAX >> (16-MYGPIO_NUM_LEDS))
#define MYGPIO_LED_MAX_VAL   MYGPIO_LED_MASK
#define MYGPIO_NUM_SWITCHES  1

#define MYGPIO_PORT_A_BTN_MASK  0x01

void MYGPIO_setPinDirs( void );
void MYGPIO_writePortB(uint16_t value);
uint8_t MYGPIO_readPortA( void );
void MYGPIO_writeLed(bool state);

int cnt = 0;
int cnt_off = 0;


/** Define the states you need */
typedef enum 
{ 
  wait_st, 
  debounce_st,
  ring_st,
  wait_off_st 
} SM1_States_e;

/** 
 * Our main state machine tick function
 * Description: Create the doorbell functionality here. This is the
 * only place you need to modify this file.
 */
void tickStateMachine( void ) 
{
  uint8_t button = 0;  // port A inputs
  static SM1_States_e SM1_state = wait_st;
  static uint16_t cnt = 0;

  button = MYGPIO_readPortA() & MYGPIO_PORT_A_BTN_MASK; 
  switch(SM1_state) // Transitions
  {
    case wait_st:
      MYGPIO_writeLed(false);
      printf("wait");
      cnt = 0;
      if (!button) {
        SM1_state = wait_st;
      } else if (button) {
        SM1_state = debounce_st;
      }
      break;
    case debounce_st:
      printf("debounce");
      if (cnt < 6 && button) {
        cnt++;
        SM1_state = debounce_st;
      } else if (!button) {
        SM1_state = wait_st;
      } else if (cnt >= 6) {
        SM1_state = ring_st;
        cnt = 0;
      }
      break;
    case ring_st:
      printf("ring");
      MYGPIO_writeLed(true);
      if (cnt < 10 || button) {
        cnt++;
        SM1_state = ring_st;
      } else {
        SM1_state = wait_off_st;
        cnt_off = 0;
      }
      break;
    case wait_off_st:
      if (cnt_off < 5 && !button){
        cnt_off++;
        SM1_state = wait_off_st;
      } else if (button){
        SM1_state = ring_st;
      } else {
        SM1_state = wait_st;
      }
      break;
    default: // of course, this should never happen
      SM1_state = wait_st;
  } // Transitions

//   switch(SM1_state) // State actions
//   { 
//     case SM1_s1:
//       break;
//     case SM1_s2:
//     default: // ADD default behavior below
//     break;
//   } // State actions
}

static volatile uint8_t m_flag = 0;
/* This is our timer ISR. It will tick at the set interval described above. */
void timerCallback(void *param)
{
  tickStateMachine();
}

/* Create the timer arguments struct variable */
const esp_timer_create_args_t m_myTimerArgs = 
{
  .callback = &timerCallback,
  .name = "My Timer"
};

/* Create the timer handler (ISR) variable */
esp_timer_handle_t m_timerHandler;

/** The main task to be executed by the RTOS */
void app_main() 
{
  MYGPIO_setPinDirs(); // establish inputs and outputs on the pins

  /* Create a timer using the arguments and handler declared previously */
  esp_timer_create(&m_myTimerArgs, &m_timerHandler);
  /* Set the timer to go periodically. This will have it repeat indefinitely
     at the period specified (in us) */
  esp_timer_start_periodic(m_timerHandler, TIMER_PERIOD);

  while(true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS); // simulate a tick function
  }
} // app_main()


/**********************************************************************
 * Temporary spaghetti until Wokwi bug is fixed
 *********************************************************************/
 /* LUT for my synthesized LED port (port B) */
static uint32_t m_ledbarXref[MYGPIO_NUM_LEDS] = 
{
  GPIO_NUM_35,  // assigning B0
  GPIO_NUM_36,  // assigning B1
  GPIO_NUM_37,  // assigning B2
  GPIO_NUM_38,  // assigning B3
  GPIO_NUM_39,  // assigning B4
  GPIO_NUM_40,  // assigning B5
  GPIO_NUM_41,  // assigning B6
  GPIO_NUM_42,  // assigning B7
  GPIO_NUM_2,   // assigning B8
  GPIO_NUM_1,   // assigning B9
};

/* LUT for my sythensized switch port (port A) */
static uint32_t m_btnXref[MYGPIO_NUM_SWITCHES] = 
{
  GPIO_NUM_11  // assiging A0
};

/** Set the pin directions (the long way). ESP has an easier way of doing
 * this if you know all the details up-front. */
void MYGPIO_setPinDirs( void )
{
  /* Set the pin directions (the long way for demo purposes) */
  // 10-segment bar graph outputs
  uint8_t i;
  for( i = 0; i < MYGPIO_NUM_LEDS; i++ )
  {
    gpio_set_direction(m_ledbarXref[i], GPIO_MODE_OUTPUT);
  }

  /* Single LED */
  gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);

  /* Buttons/switches */
  for( i = 0; i < MYGPIO_NUM_SWITCHES; i++ )
  {
    gpio_set_direction(m_btnXref[i], GPIO_MODE_INPUT);
    gpio_set_pull_mode(m_btnXref[i], GPIO_PULLDOWN_ONLY);
  }
}

/** Synthesize a data port for the LED bargraph. Bit 0 is the bottom-most
 * LED. Bit 9 is the top-most LED. */
void MYGPIO_writePortB(uint16_t value)
{
  uint8_t i;
  
  /* Only loop through the number of bits in the display. These are 1-hot
   * encoded. */
  for(i = 0; i < MYGPIO_NUM_LEDS; i++)
  {
    // if high, set high
    if((value >> i) & 1)
    {
      gpio_set_level(m_ledbarXref[i], 1);
    }
    // if low, set low
    else
    {
      gpio_set_level(m_ledbarXref[i], 0);
    }
  }
}

/** Read the bits from my synthesized switch data port */
uint8_t MYGPIO_readPortA( void )
{
  uint8_t portVal = 0;
  uint8_t i;
  
  for( i = 0; i < MYGPIO_NUM_SWITCHES; i++ )
  {
    if( gpio_get_level(m_btnXref[i]) )
    {
      portVal |= (1 << i);
    }
  }
  return portVal;
}

/** Function to control the single LED */
void MYGPIO_writeLed(bool state)
{
  if(state)
  {
    gpio_set_level(GPIO_NUM_14, 1);
  }
  else
  {
    gpio_set_level(GPIO_NUM_14, 0);
  }
}