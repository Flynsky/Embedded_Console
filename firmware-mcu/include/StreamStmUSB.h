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
  char status = 1;
  char trys = 0;
  do
  {
    HAL_Delay(RECONNECT_TIMEOUT);
    status = CDC_Transmit_FS(
        (uint8_t *)buffer, (uint16_t)buffer_size); // Send data via USB}while(status != USBD_OK)
    trys++;
    if (trys > RECONNECT_TRYS)
    {
      break;
    }
  } while (status == USBD_BUSY);
}

extern USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef FS_Desc;

// software jump to bootloader
void inline StreamStmUSB::jumpToBootloader()
{
  out("dfu updating", 13);
  // Disables CDC USB
  USBD_Stop(&hUsbDeviceFS);
  USBD_DeInit(&hUsbDeviceFS);

  // HAL_DeInit();
  // HAL_RCC_DeInit();

  // Disable all interrupts
  __disable_irq();

  // Reset USB peripheral (optional, but good practice)
  // RCC->APB1ENR1 &= ~RCC_APB1ENR1_USBFSEN;
  // RCC->APB1ENR1 |= RCC_APB1ENR1_USBFSEN;

  // Set the vector table MSP and jump to bootloader
  // look them up in AN2606
  // uint32_t bootloader_address = 0x1FFF0000; // STM32L4 system memory
  uint32_t bootloader_address = 0x1FFFC400; // STM32F4 system memory

  __set_MSP(*(volatile uint32_t *)bootloader_address);
  void (*bootloader_jump)(void) = (void (*)(void))(*(volatile uint32_t *)(bootloader_address + 4));
  bootloader_jump();
}