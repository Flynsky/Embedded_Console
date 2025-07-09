#pragma once

#include "IStream.h"

class Console {
  IStream &stream;
  Console(IStream &Stream) : stream(Stream) {startupMessage();}

public:
  bool recieveCommands();
  void manageStatusLED();
  bool printf(const char *__restrict format, ...);
  void startupMessage();
  void jumpToBootloader();
};

