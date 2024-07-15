
#include <Arduino.h>
#include "pbx.h"
#include "config.h"
#include "dialtone.h"
#include "callerid.h"
#include "dtmf.h"
#include "ringer.h"

Ringer ringer = Ringer(RINGER_Q1, RINGER_Q2, RINGER_Q3, RINGER_RELAY, RINGER_FREQUENCY);

volatile boolean caller_hook_read = false;
volatile boolean dest_hook_read = false;

unsigned long caller_hook_last_transition = 0;
unsigned long dest_hook_last_transition = 0;
auto caller_off_hook = false;
auto dest_off_hook = false;
State state = UNINITIALIZED;
void caller_hook_isr();

void initialize_pbx()
{
  pinMode(HOOK_PIN, INPUT_PULLUP);
  pinMode(CALLER_HOOK_PIN, INPUT_PULLUP);
  pinMode(DEST_HOOK_PIN, INPUT_PULLUP);
  pinMode(CONNECT_RELAY, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(CALLER_HOOK_PIN), caller_hook_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DEST_HOOK_PIN), caller_hook_isr, CHANGE);
}

void handle_pbx()
{
  static State last_state = UNINITIALIZED;
  static unsigned long last_state_change = 0;
  static unsigned long last_tone_change = 0;
  static long rings = 0;
  static char called_number[64] = {'0', '1', '0', '1', '0', '0', '0', '0'};
  char *called_number_ptr = called_number + 8;
  int readn = 0;

  ringer.handle();

  if (millis() - caller_hook_last_transition > 300)
  {
    caller_off_hook = caller_hook_read;
  }

  if (millis() - dest_hook_last_transition > 300)
  {
    dest_off_hook = dest_hook_read;
  }

  switch (state)
  {
  case UNINITIALIZED:
    state = IDLE;
  case IDLE:
    if (caller_off_hook && dest_off_hook)
    {
      state = BUSY;
    }
    else if (caller_off_hook && !dest_off_hook)
    {
      memset(called_number, 0, sizeof(called_number));
      state = WAIT_FOR_DIAL;
    }
    break;

  case BUSY:
    if (!caller_off_hook && !dest_off_hook)
    {
      state = IDLE;
    }
    break;

    // "autodial" after 2s
  case WAIT_FOR_DIAL:
    rings = 0;
    if (!caller_off_hook)
    {
      state = IDLE;
    }
    else
    {

      readn = get_number(called_number_ptr, sizeof(called_number) - 8);
      if (readn > 0)
      {
        state = CALLING;
      }
    }

    // if (millis() - last_state_change > 2000)
    // {
    //   state = CALLING;
    // }
    break;

  case CALLING:
    if (!caller_off_hook)
    {
      state = IDLE;
    }
    if (caller_off_hook && dest_off_hook)
    {
      state = CONNECTED;
    }
    break;

  case CONNECTED:
    if (!caller_off_hook)
    {
      state = BUSY;
    }
  }

  if ((last_state == BUSY && state != BUSY) ||
      (last_state == CALLING && state != CALLING) ||
      (last_state == WAIT_FOR_DIAL && state != WAIT_FOR_DIAL))
  {
    dialtone_stop();
    ringer.stop();
    last_tone_change = 0;
  }

  if (state == WAIT_FOR_DIAL && last_state != WAIT_FOR_DIAL)
  {
    dialtone_start();
  }

  if (state == BUSY)
  {
    if (millis() - last_tone_change > 500)
    {
      dialtone_stop();
      dialtone_start(250);
      last_tone_change = millis();
    }
  }

  if (state == CALLING)
  {
    if (millis() - last_tone_change > 4000)
    {
      dialtone_stop();
      dialtone_start(1500);
      last_tone_change = millis();
      ringer.start(1500, []()
                   {
                  rings++;
   if (rings == 1)
      {
        delay(10);
        transmit_caller_id(called_number, strlen(called_number));
      } });
    }
  }

  if (state == CONNECTED)
  {
    ringer.stop();
    digitalWrite(CONNECT_RELAY, HIGH);
  }
  else
  {
    digitalWrite(CONNECT_RELAY, LOW);
  }

  if (last_state != state)
  {
    last_state = state;
    last_state_change = millis();
    Serial.print("State: ");
    String n = state_to_string(state);
    Serial.println(n);
  }
}

void caller_hook_isr()
{
  auto current_caller_hook_read = !digitalRead(CALLER_HOOK_PIN);
  auto current_dest_hook_read = !digitalRead(DEST_HOOK_PIN);
  if (current_caller_hook_read != caller_hook_read)
  {
    caller_hook_read = current_caller_hook_read;
    caller_hook_last_transition = millis();
  }
  if (current_dest_hook_read != dest_hook_read)
  {
    dest_hook_read = current_dest_hook_read;
    dest_hook_last_transition = millis();
  }
}

String state_to_string(State s)
{
  switch (s)
  {
  case UNINITIALIZED:
    return "UNINITIALIZED";
  case IDLE:
    return "IDLE";
  case BUSY:
    return "BUSY";
  case WAIT_FOR_DIAL:
    return "WAIT_FOR_DIAL";
  case CALLING:
    return "CALLING";
  case CONNECTED:
    return "CONNECTED";
  }
  return "UNKNOWN";
}
