#include "STM32StreamVCP.h"
#include "usbd_cdc_if.h" //access to the virtual com port buffer. DeInt of Vcp for dfu uploads
extern uint8_t UserRxBufferFS[]; // buffer where commands get written to
//
//
void StreamUSB ::out(const char *buffer, const unsigned int buffer_size) {
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


const char *StreamUSB::getBuffer() {
  return (const char *)UserRxBufferFS;
}



