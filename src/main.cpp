#include <Arduino.h>
#include "FSM.h"

Context *FSM = new Context (new Idle);
hw_timer_t *timer1 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer1 ()
{
  if(FSM->ISR_REGISTER == FSM->ISR_ZERO)
    FSM->ISR_REGISTER = FSM->ISR_Timer1;
}

void FSM_loop ()
{

  switch(FSM->ISR_REGISTER)
  {
    case FSM->ISR_ZERO:
      FSM->loopFunction();
      break;
    case FSM->ISR_BufferFull:
      FSM->isrBufferFull();
      break;
    case FSM->ISR_BufferEmpty:
      FSM->isrBufferFull();
      break;
    case FSM->ISR_Timer1:
      FSM->isrTimer();
      break;
    default:
      Serial.println("Brak aktywnych flag");
      break;
  }
  
  // if(ISR_BUFFER_FULL)
  //   FSM->isrBufferFull();
  // else if (ISR_TIMER_FLAG)
  //   FSM->isrBufferFull();
  // else
  //   FSM->loopFunction();

}

void setup() 
{
  Serial.begin(921600);
  delay(1000);
  timer1 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 2000000, true);
  timerAlarmEnable(timer1);

  WiFi.mode(WIFI_STA);
  FSM->listNetworks();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  FSM->sensors->begin();
    if(FSM->sensors->getAddress(FSM->sensor1, 0))
    {
        Serial.print("\nFound device ");
        Serial.print("with address: ");
        FSM->printAddress(FSM->sensor1);

        Serial.println();
    }

    if(FSM->sensors->getAddress(FSM->sensor2, 1))
    {
        Serial.print("\nFound device ");
        Serial.print("with address: ");
        FSM->printAddress(FSM->sensor2);
        Serial.println();
    }
 }

void loop() 
{
 
  FSM_loop();
  delay(500);
}