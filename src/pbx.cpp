
#include <Arduino.h>
#include "pbx.h"
#include "config.h"
#include "dialtone.h"
#include "callerid.h"
#include "dtmf.h"
#include "ringer.h"

volatile boolean PBX::caller_hook_read = false;
volatile boolean PBX::dest_hook_read = false;
volatile unsigned long PBX::caller_hook_last_transition = 0;
volatile unsigned long PBX::dest_hook_last_transition = 0;
volatile bool PBX::caller_off_hook = false;
volatile bool PBX::dest_off_hook = false;

PBX::PBX()
{
  ringer = Ringer(RINGER_Q1, RINGER_Q2, RINGER_Q3, RINGER_RELAY, RINGER_FREQUENCY);
  callerId = CallerId();
  dialtone = Dialtone();
  dtmf = Dtmf();
}

void PBX::begin()
{
  pinMode(CALLER_HOOK_PIN, INPUT_PULLUP);
  pinMode(DEST_HOOK_PIN, INPUT_PULLUP);
  pinMode(CONNECT_RELAY, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(CALLER_HOOK_PIN), PBX::caller_hook_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DEST_HOOK_PIN), PBX::dest_hook_isr, CHANGE);
  dtmf.begin();
}

void PBX::handle()
{
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

      readn = dtmf.get_number(called_number_ptr, sizeof(called_number) - 8, dialtone);
      if (readn > 0)
      {
        Serial.print("Dialed: ");
        Serial.println(called_number + 8);
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
    dialtone.stop();
    ringer.stop();
    last_tone_change = 0;
  }

  if (state == WAIT_FOR_DIAL && last_state != WAIT_FOR_DIAL)
  {
    dialtone.start();
  }

  if (state == BUSY)
  {
    if (millis() - last_tone_change > 500)
    {
      dialtone.stop();
      dialtone.start(250);
      last_tone_change = millis();
    }
  }

  if (state == CALLING)
  {
    if (millis() - last_tone_change > 4000)
    {
      last_tone_change = millis();
      dialtone.stop();
      dialtone.start(1500);
      ringer.start(1500, [this]()
                   {
                  rings++;
   if (rings == 1)
      {
        delay(10);
        callerId.transmit_caller_id(called_number, strlen(called_number));
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

void PBX::caller_hook_isr()
{
  caller_hook_read = !digitalRead(CALLER_HOOK_PIN);
  caller_hook_last_transition = millis();
}

void PBX::dest_hook_isr()
{
  dest_hook_read = !digitalRead(DEST_HOOK_PIN);
  dest_hook_last_transition = millis();
}

String PBX::state_to_string(State s)
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
