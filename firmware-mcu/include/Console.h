#pragma once

#include "IStream.h"

class Console {
  IStream &stream;
  Console(IStream &Stream) : stream(Stream) { startupMessage(); }
public:
  bool recieveCommands();
  void manageStatusLED();
  bool printf(const char *__restrict format, ...);
  void startupMessage();
  void jumpToBootloader();
};

#include <cstdarg>
#include <cstdio>
bool inline Console::printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  // Compute required buffer buffer_size
  int buffer_size = vsnprintf(nullptr, 0, format, args);
  va_end(args); // Must call again before re-using

  if (buffer_size < 0)
    return false;

  char *buffer = new char[buffer_size + 1]; // +1 for null terminator
  if (!buffer)
    return false;

  va_start(args, format);
  vsnprintf(buffer, buffer_size + 1, format, args);
  va_end(args);

  stream.out(buffer, buffer_size);
  delete[] buffer;
  return true;
}
