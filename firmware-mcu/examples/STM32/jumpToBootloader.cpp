
extern USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef FS_Desc;

/**
 * Trigger DFU bootloader via Software
 */
void jump_to_dfu_bootloader()
{
    /* Disables CDC USB*/
    USBD_Stop(&hUsbDeviceFS);
    USBD_DeInit(&hUsbDeviceFS);

    // Disable all interrupts
    __disable_irq();

    // Reset USB peripheral (optional, but good practice)
    RCC->APB1ENR1 &= ~RCC_APB1ENR1_USBFSEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USBFSEN;

    // Set the vector table MSP and jump to bootloader
    uint32_t bootloader_address = 0x1FFF0000; // STM32L4 system memory

    __set_MSP(*(volatile uint32_t *)bootloader_address);
    void (*bootloader_jump)(void) = (void (*)(void))(*(volatile uint32_t *)(bootloader_address + 4));
    bootloader_jump();
}
