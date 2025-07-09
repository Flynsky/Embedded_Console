#pragma once
// #include <vector>

using namespace std;
class IStream {
  // vector<char> buffer;
  bool init = 0;
  virtual bool setup() { return 1; };

public:
  virtual bool isAvaliable() = 0;
  virtual const char *getBuffer() = 0;
  virtual const char *clearBuffer() = 0;
  virtual void print(const char *) {};
  virtual void flush() {};
};
