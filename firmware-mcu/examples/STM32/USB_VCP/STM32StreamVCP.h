#include "IStream.h"

#include "usbd_cdc_if.h" //access to the virtual com port buffer. DeInt of Vcp for dfu uploads
extern uint8_t UserRxBufferFS[]; // buffer where commands get written to

class StreamUSB : IStream {
public:
  StreamUSB() { 
  UserRxBufferFS[0] = 0;
 };
  bool isAvaliable();
  const char *getBuffer();
  const char *clearBuffer();
  void out(const char *buffer, const unsigned int buffer_size);
  void flush() {};
};
