
#include "Console.h"
// #include "StreamStmUSB.h"
#include "main.h"

void sendMDIO(float freq_kHz);

bool Console::recieveCommands()
{
  bool result = false;
  /*checks if 0 is overridden -> new message in buffer*/
  if (stream.isAvaliable())
  {
    // printf("Received data: %s\n", UserRxBufferFS);

    /*decode message*/
    char command[4] = {0};
    float param0 = -1, param1 = -1, param2 = -1, param3 = -1;

    int num_params = sscanf(stream.getBuffer(), "%s %f %f %f %f", command,
                            &param0, &param1, &param2, &param3);
    if (num_params)
    {

      // Print the command and the parameters
      printf("~rec:%s|%f|%f|%f|%f|\n", command, param0, param1, param2, param3);

      // printf("r:%i,n:%i\n",com_encoded, (int)('d' << 24 | 'f' << 16 | 'u' <<
      // 8 | 0));

      // encode command to int to use switch case
      int com_encoded =
          (int)((command[0] << 24) | (command[1] << 16) | (command[2] << 8) |
                command[3]); // encodes 4 char in one int to be compared by
                             // switch case
      switch (com_encoded)
      {
      /**here are the executions of all the commands */
      /*help*/
      case (int)('?' << 24 | 0):
      {
        printf("\n--help--\n");
        printf("-[str command]_[4x float param]\n");
        printf("-?|this help screen\n");
        printf("-dfu|Device Firmware Update\n");
        printf("-pa [freq] [dfu \\%]|Sets Phase A Freq\n");
        printf("\n");
        result = true;
        break;
      }

      case (int)('m' << 24 | 0):
      {
        printf(">Send MDIO\n");
        sendMDIO(param0);
        result = true;
        break;
      }

      case (int)('b' << 24 | 0):
      {
        printf(">Battery Voltage:69V");
        printf("\n");
        result = true;
        break;
      }

      /*dfu update*/
      case (int)('d' << 24 | 'f' << 16 | 'u' << 8 | 0):
      {
        printf("\n--DFU update--\n");
        stream.jumpToBootloader();
        result = false;
        break;
      }

      default:
      {
        printf("unknown commnad\n");
        break;
      }
      }
      stream.clearBuffer();
    }
  }
  return result;
}

void Console::startupMessage()
{
  printf("\033[35m");
  printf("  .-')    .-') _   _   .-')\n");
  printf(" ( OO ). (  OO) ) ( '.( OO )_\n");
  printf(" (_)---\\_)/     '._ ,--.   ,--.).-----.  .-----.  \n");
  printf("/    _ | |'--...__)|   `.'   |/  -.   \\/ ,-.   \\ \n");
  printf(" \\  :` `. '--.  .--'|         |'-' _'  |'-'  |  | \n");
  printf(" '..`''.)   |  |   |  |'.'|  |   |_  <    .'  /  \n");
  printf(" .-._)   \\   |  |   |  |   |  |.-.  |  | .'  /__  \n");
  printf("  \\       /   |  |   |  |   |  |\\ `-'   /|       | \n");
  printf(" `-----'    `--'   `--'   `--' `----'' `-------' \n");
  printf("\033[0m");
}

extern TIM_HandleTypeDef htim1;
int count = 0;
void sendMDIO(float freq_kHz)
{
  // STM32 HAL: T = 875n us / 1.2MHz
  // __disable_irq();
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_SET);
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_RESET);
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_SET);
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_RESET);
  //     HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_SET);
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_RESET);
  //     HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_SET);
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_RESET);
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_SET);
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_RESET);
  //     HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_SET);
  //  HAL_GPIO_WritePin(MCLK_GPIO_Port, MCLK_Pin, GPIO_PIN_RESET);
  // __enable_irq();

  // NO HALL ARM M4: T = 83n / 12MHz
  // needs in CMalke.List: add_compile_options(-O3 -funroll-loops)
  __disable_irq();

  volatile uint32_t *bsrr = (uint32_t *)0x40020418;
#define PIN_MCLK 8
#define PIN_MDIO 9

#define SET_MCLK (1 << (PIN_MCLK))
#define RESET_MCLK (1 << (PIN_MCLK + 16))

#define SET_MDIO (1 << (PIN_MDIO))
#define RESET_MDIO (1 << (PIN_MDIO + 16))

#define HIGH_BIT                 \
  *bsrr = RESET_MCLK + SET_MDIO; \
  __DSB();                       \
  *bsrr = SET_MCLK + SET_MDIO;   \
  __DSB();

#define LOW_BIT                    \
  *bsrr = RESET_MCLK + RESET_MDIO; \
  __DSB();                         \
  *bsrr = SET_MCLK + RESET_MDIO;   \
  __DSB();

  *bsrr = SET_MCLK + SET_MDIO;
  *bsrr = SET_MCLK + SET_MDIO;
  __DSB();

  /*PRESCAMBLE*/
#define TOGGLE_MDIO   \
  *bsrr = RESET_MDIO; \
  __DSB();            \
  *bsrr = SET_MDIO;   \
  __DSB();

  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO

  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO

  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO

  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO

  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO

  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO
  TOGGLE_MDIO

  TOGGLE_MDIO
  TOGGLE_MDIO

  /*START*/
  HIGH_BIT
  LOW_BIT

  /*OPCODE*/
  HIGH_BIT
  HIGH_BIT

  /*PHY_ADRESS*/
  LOW_BIT
  LOW_BIT
  LOW_BIT
  HIGH_BIT
  LOW_BIT

  /*REG_ADRESS*/
  LOW_BIT
  LOW_BIT
  LOW_BIT
  HIGH_BIT
  HIGH_BIT

  /*TA*/
  HIGH_BIT
  LOW_BIT

  /*DATA*/
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  LOW_BIT
  HIGH_BIT
  LOW_BIT
  HIGH_BIT

  /*end*/
  *bsrr = RESET_MDIO + RESET_MCLK;
  __enable_irq();

  // timer: T= 5,2us / 192 kHz
  //  __HAL_RCC_TIM1_CLK_ENABLE();

  // htim1.Instance = TIM1;
  // htim1.Init.Prescaler = 8399; // 84MHz / (8399 + 1) = 10kHz
  // htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  // htim1.Init.Period = 9999; // 10kHz / (9999 + 1) = 1Hz overflow
  // htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  // htim1.Init.RepetitionCounter = 0;
  // htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  // if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  //   Error_Handler();
  // count = 0;
  // uint32_t i = (uint32_t)freq_kHz;
  // htim1.Instance = TIM1;
  // htim1.Init.Prescaler = 0;
  // htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  // htim1.Init.Period = i;
  // htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  // htim1.Init.RepetitionCounter = 0;
  // htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  // HAL_TIM_Base_Init(&htim1);

  // printf("htim1.Init.Period:%lu\n", htim1.Init.Period);

  // HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
  // HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
  // HAL_TIM_Base_Start_IT(&htim1);
}

void TIM1_UP_TIM10_IRQHandler(void)
{
  // HAL_TIM_IRQHandler(&htim1);

  TIM_HandleTypeDef *htim = &htim1;
  uint32_t itsource = htim->Instance->DIER;
  uint32_t itflag = htim->Instance->SR;

  /* TIM Update event */
  if ((itflag & (TIM_FLAG_UPDATE)) == (TIM_FLAG_UPDATE))
  {
    if ((itsource & (TIM_IT_UPDATE)) == (TIM_IT_UPDATE))
    {
      __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_UPDATE);

      HAL_TIM_PeriodElapsedCallback(htim);
    }
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (count == 32)
  {
    HAL_NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
  }
  else
  {
    if (htim->Instance == TIM1)
    {
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);

      // // Toggle Assembly:
      // static uint8_t pin_state = 0;

      // // Fast pin toggle using BSRR
      // if (pin_state)
      // {
      //   GPIOB->BSRR = (1 << (9 + 16)); // Reset PB9
      //   pin_state = 0;
      // }
      // else
      // {
      //   GPIOB->BSRR = (1 << 9); // Set PB9
      //   pin_state = 1;
      // }
    }
  }
  count++;
}

// extern USBD_HandleTypeDef hUsbDeviceFS;
// extern USBD_DescriptorsTypeDef FS_Desc;

// /**
//  * Trigger DFU bootloader via Software
//  */
// void Console::jumpToBootloader() {
//       // Disables CDC USB
//     USBD_Stop(&hUsbDeviceFS);
//     USBD_DeInit(&hUsbDeviceFS);

//     // Disable all interrupts
//     __disable_irq();

//     // Reset USB peripheral (optional, but good practice)
//     RCC->APB1ENR1 &= ~RCC_APB1ENR1_USBFSEN;
//     RCC->APB1ENR1 |= RCC_APB1ENR1_USBFSEN;

//     // Set the vector table MSP and jump to bootloader
//     uint32_t bootloader_address = 0x1FFF0000; // STM32L4 system memory

//     __set_MSP(*(volatile uint32_t *)bootloader_address);
//     void (*bootloader_jump)(void) = (void (*)(void))(*(volatile uint32_t *)(bootloader_address + 4));
//     bootloader_jump();
// }