#define BUFF_SIZE  10

class Buffer
{
  private:
  uint8_t size_;
  uint8_t head_, tail_;
  
  float *tab;

  public:
  Buffer();
  ~Buffer();
  bool isFullFlag;
  bool isEmptyFlag;
  void push(float);
  float pull();
  bool isFull();
  bool isEmpty();
  float avr();
};