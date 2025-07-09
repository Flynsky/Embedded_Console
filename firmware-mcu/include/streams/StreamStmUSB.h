#include "IStream.h"

/**
 * This Stream handles the Virtual Com Port of an STM32.
 *
 * To use it just enable and initialize the Virtual Com Port, no other changes
 * needed!
 *
 * It is a barebone driver, so:
 * - New Messages overwrite the previous recieved
 * - Messages which are too long may cause overflow
 * - Messages starting with 0 don't get recieved properly
 * - No Interupt capability, needs to be manualy checked with isAvaliable()
 *
 * It uses the fact that new VCP messages always land in UserRxBufferFS[].
 * So if(UserRxBufferFS[0]==0) before a message, the new message will overwrite
 * the 0 and it won't be true anymore.
 */

#include "usbd_cdc_if.h" //access to the virtual com port buffer. DeInt of Vcp for dfu uploads
extern uint8_t UserRxBufferFS[]; // buffer where commands get written to

class StreamStmUSB : IStream {
public:
  StreamStmUSB () { clearBuffer(); };
  bool isAvaliable() { return (UserRxBufferFS[0]) ? true : false; };
  const char *getBuffer() { return (const char *)UserRxBufferFS; };
  bool clearBuffer() {
    UserRxBufferFS[0] = 0;
    return true;
  };
  void out(const char *buffer, const unsigned int buffer_size);
  void flush() {};
};

void inline StreamUSB ::out(const char *buffer,
                            const unsigned int buffer_size) {
  const unsigned int RECONNECT_TRYS = 5;
  const unsigned int RECONNECT_TIMEOUT = 5;
  char status = 1;
  char trys = 0;
  do {
    HAL_Delay(RECONNECT_TIMEOUT);
    status = CDC_Transmit_FS(
        (uint8_t *)buffer, (uint16_t)buffer_size -
                               1); // Send data via USB}while(status != USBD_OK)
    trys++;
    if (trys > RECONNECT_TRYS) {
      break;
    }
  } while (status == USBD_BUSY);
}
