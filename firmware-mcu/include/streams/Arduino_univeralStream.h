#pragma once
#include "IStream.h"
#include <Arduino.h>
 /**
	* maps Arduino Stream Libary (https://github.com/arduino/ArduinoCore-API/blob/master/api/Stream.h) to IStream
	*/

using namespace std;
class StreamArduino {
  Stream &streamArduino;
  StreamArduino(Stream &s) : streamArduino(s) {};

public:
  bool isAvaliable() { return streamArduino.available(); };
  const char *getBuffer() { return streamArduino.readLine(); };
  bool clearBuffer() {};
  void out(const char *buffer, const unsigned int buffer_size);
  void flush() { streamArduino.flush(); };
};

inline const char *getBuffer() {
  // ToDo
  // return streamArduino.readLine();
};
