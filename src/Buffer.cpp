#include <Arduino.h>
#include "Buffer.h"

Buffer::Buffer() : size_(BUFF_SIZE), head_(0), tail_(0), tab(nullptr)
{
  tab = new float [size_];
  isFullFlag = false;
  isEmptyFlag = true;
}

Buffer::~Buffer()
{
  //delete [] tab;
}

void Buffer::push(float data)
{
  if(!isFullFlag)
  {
    tab[head_] = data;
    isEmptyFlag = false;
    head_ = (head_+1)%size_;
  }
  isFull();
}

float Buffer::pull()
{
  float temp = -10;
  if(!isEmptyFlag)
  {
    temp = tab[tail_];
    isFullFlag = false;
    tail_ = (tail_+1)%size_;
  }
  isEmpty();
  return temp;
}

bool Buffer::isFull()
{
  uint8_t temp_t = ((tail_-1)+size_)%size_;
  if(head_ == temp_t)
  {
    head_ = temp_t;
    isFullFlag = true;
    return 1;
  }
  isFullFlag = false;
  return 0;
}

float Buffer::avr()
{
  if(isFullFlag)
  {
    float temp = 0.0;
    while(!isEmptyFlag)
    {
      temp += pull(); 
    }
    return (temp/size_);
  }
  else return -20.0;
}

bool Buffer::isEmpty()
{
  if(head_==tail_) 
  {
    isEmptyFlag = true;
    //tail_++;
    return 1;
  }
  return 0;
}