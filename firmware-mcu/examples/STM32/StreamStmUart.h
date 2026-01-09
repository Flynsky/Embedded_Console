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
 * It uses the fact that new VCP messages always land in UART1_rxBuffer[].
 * So if(UART1_rxBuffer[0]==0) before a message, the new message will overwrite
 * the 0 and it won't be true anymore.
 */

#pragma once
#include "IStream.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"

extern uint8_t UART1_rxBuffer[];

extern UART_HandleTypeDef huart1;


class StreamStmUart : public IStream {
public:
  StreamStmUart() { clearBuffer(); };
  bool isAvaliable() { return (UART1_rxBuffer[0]) ? true : false; };
  const char *getBuffer() { return (const char *)UART1_rxBuffer; };
  bool clearBuffer() {
    UART1_rxBuffer[0] = 0;
    return true;
  };
  void out(const char *buffer, const unsigned int buffer_size);
  void flush() {};
  void jumpToBootloader();
};

void inline StreamStmUart ::out(const char *buffer,
                               const unsigned int buffer_size) {
  const unsigned int TIMEOUT = 10;

  char status = 0;
  status = HAL_UART_Transmit(&huart1,(uint8_t *)buffer, (uint16_t)buffer_size, TIMEOUT);
}

// software jump to bootloader
void inline StreamStmUart::jumpToBootloader() {
  // extern USBD_HandleTypeDef hUsbDeviceFS;

  // look system memory adress up in AN2606
  const uint32_t bootloader_address = 0x1FFF0000; // STM32L4

  // out("dfu updating", 13);

  /* Disables CDC USB*/
  // USBD_Stop(&hUsbDeviceFS);
  // USBD_DeInit(&hUsbDeviceFS);

  // Disable all interrupts
  __disable_irq();

  /* Clear Interrupt Enable Register & Interrupt Pending Register */
  for (size_t i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); i++) {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
  }

  /* Re-enable all interrupts */
  __enable_irq();

  __set_MSP(*(volatile uint32_t *)bootloader_address);
  void (*bootloader_jump)(void) =
      (void (*)(void))(*(volatile uint32_t *)(bootloader_address + 4));
  bootloader_jump();
}
