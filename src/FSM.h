#include <Arduino.h>

class Context;
class State
{
  protected:
  Context *context_;

  public:
  virtual ~State();

  void setContext(Context *context);
  virtual void IsrTimer() = 0;
  virtual void IsrBufferFull() = 0;
};

class Context
{
  private:
  State *state_;

  public:
  Context (State *state);
  ~Context();
  void transitionToState(State *state);
  void IsrTimer();
  void IsrBufferFull();
};

class Idle : public State
{
  void IsrTimer();
  void IsrBufferFull();
};

class DataRead : public State
{
  void IsrTimer();
  void IsrBufferFull();
};

class DataReceive : public State
{
  void IsrTimer();
  void IsrBufferFull();
};

class DataSend : public State
{
  void IsrTimer();
  void IsrBufferFull();
};

