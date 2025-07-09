#pragma once
// #include <vector>

using namespace std;
class IStream {
public:
  virtual bool isAvaliable() = 0;
  virtual const char *getBuffer() = 0;
  virtual bool clearBuffer() = 0;
  virtual void out(const char *buffer, const unsigned int buffer_size) {};
  virtual void flush() {};
};
