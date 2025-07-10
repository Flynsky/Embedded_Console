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

#pragma once
#include "IStream.h"
#include "usbd_cdc_if.h" //access to the virtual com port buffer. DeInt of Vcp for dfu uploads

extern uint8_t UserRxBufferFS[]; // buffer where commands get written to

class StreamStmUSB : public IStream
{
public:
  StreamStmUSB() { clearBuffer(); };
  bool isAvaliable() { return (UserRxBufferFS[0]) ? true : false; };
  const char *getBuffer() { return (const char *)UserRxBufferFS; };
  bool clearBuffer()
  {
    UserRxBufferFS[0] = 0;
    return true;
  };
  void out(const char *buffer, const unsigned int buffer_size);
  void flush() {};
  void jumpToBootloader();
};

void inline StreamStmUSB ::out(const char *buffer,
                               const unsigned int buffer_size)
{
  const unsigned int RECONNECT_TRYS = 5;
  const unsigned int RECONNECT_TIMEOUT = 5;
  
  char status = USBD_FAIL;
  char trys = 0;
  while (trys < RECONNECT_TRYS && status != USBD_OK)
  {
    status = CDC_Transmit_FS(
        (uint8_t *)buffer, (uint16_t)buffer_size);
    trys++;
    HAL_Delay(RECONNECT_TIMEOUT);
  }
}

// software jump to bootloader
void inline StreamStmUSB::jumpToBootloader()
{
  extern USBD_HandleTypeDef hUsbDeviceFS;

  // look them up in AN2606
  const uint32_t bootloader_address = 0x1FFF0000; // STM32L4 system memory

  // out("dfu updating", 13);

  /* Disables CDC USB*/
  USBD_Stop(&hUsbDeviceFS);
  USBD_DeInit(&hUsbDeviceFS);

  // Disable all interrupts
  __disable_irq();

  /* Clear Interrupt Enable Register & Interrupt Pending Register */
  for (size_t i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
  }

  /* Re-enable all interrupts */
  __enable_irq();

  __set_MSP(*(volatile uint32_t *)bootloader_address);
  void (*bootloader_jump)(void) = (void (*)(void))(*(volatile uint32_t *)(bootloader_address + 4));
  bootloader_jump();
}