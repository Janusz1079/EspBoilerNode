#include <Arduino.h>
#include "Buffer.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class Context;
class State
{
  protected:
  Context *context_;

  public:
  virtual ~State(){}

  void setContext(Context *context);
  

  virtual void entryFunction();
  virtual void isrTimer();
  virtual void isrBufferFull();
  virtual void isrBufferEmpty();
  virtual void exitFunction();
  virtual void loopFunction();
};

class Context
{
  private:
  State *state_;

  public:
  Context (State *state);
  ~Context();
  float temptemp;
  enum isrFlags : uint8_t
  {
    ISR_ZERO,
    ISR_BufferFull, 
    ISR_BufferEmpty, 
    ISR_Timer1
  };
  isrFlags ISR_REGISTER;
  OneWire *oneWire;
  DallasTemperature *sensors;
  DeviceAddress sensor1;
  DeviceAddress sensor2;
  Buffer *sensor1_buff;    // Czujnik temperatury wody kotła
  Buffer *sensor2_buff;    // Czujnik termperatur wody powrotu
  
  void printAddress(DeviceAddress deviceAddres);
  void readSensorsData(){};
  void sendSensorsData(){};
  void transitionToState(State *state);
  void entryFunction();
  void isrTimer();
  void isrBufferFull();
  void isrBufferEmpty();
  void loopFunction();
  void exitFunction();
  //void checkBuffers();
};

class Idle : public State
{
  void entryFunction();
  void isrTimer();
  void isrBufferFull();
  void isrBufferEmpty(){}
  void loopFunction(){}
  void exitFunction();
};

class DataRead : public State
{
  void entryFunction();
  void isrTimer();
  void isrBufferFull();
  void isrBufferEmpty(){}
  void loopFunction(){}
  void exitFunction();
};
/*
class DataReceive : public State
{
  void entryFunction();
  void isrTimer();
  void isrBufferFull();
  void isrBufferEmpty();
  void exitFunction();
};
*/
class DataSend : public State
{
  void entryFunction();
  void isrTimer(){}
  void isrBufferFull();
  void isrBufferEmpty(){}
  void loopFunction(){}
  void exitFunction();
};

class Sensor1 : public State
{
  void entryFunction();
  void isrTimer(){}
  void isrBufferFull(){}
  void isrBufferEmpty(){}
  void exitFunction(){}
};

class Sensor2 : public State
{
  void entryFunction();
  void isrTimer(){}
  void isrBufferFull(){}
  void isrBufferEmpty(){}
  void exitFunction(){}
};