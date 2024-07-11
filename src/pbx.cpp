
#include <Arduino.h>
#include "pbx.h"
#include "config.h"
#include "dialtone.h"
#include "callerid.h"

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
  pinMode(RINGER_RELAY, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(HOOK_PIN), caller_hook_isr, CHANGE);
}

void handle_pbx()
{
  static State last_state = UNINITIALIZED;
  static unsigned long last_state_change = 0;
  static unsigned long last_tone_change = 0;
  static unsigned long last_ringer_change = 0;
  static long rings = 0;

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
    if (millis() - last_state_change > 2000)
    {
      state = CALLING;
    }
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
    digitalWrite(RINGER_RELAY, LOW);
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
    }
    if (millis() - last_ringer_change > 2000)
    {
      if (rings == 1)
      {
        transmit_caller_id("1234");
      }
      digitalWrite(RINGER_RELAY, !digitalRead(RINGER_RELAY));
      last_ringer_change = millis();
      rings++;
    }
  }

  if (state == CONNECTED)
  {
    digitalWrite(RINGER_RELAY, LOW);
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
