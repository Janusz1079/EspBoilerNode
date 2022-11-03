#include <Arduino.h>
#define BUFF_SIZE  10

class Buffer
{
  private:
  uint8_t size_;
  uint8_t head_, tail_;
  bool isFullFlag;
  bool isEmptyFlag;
  float *tab;

  public:
  Buffer();
  ~Buffer();

  void push(float);
  float pull();
  bool isFull();
  bool isEmpty();
  float avr();
};