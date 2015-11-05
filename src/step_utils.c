#include "step_utils.h"

void RCC_Init()
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    // Enable Power Control clock
    __PWR_CLK_ENABLE();

    // The voltage scaling allows optimizing the power consumption when the
    // device is clocked below the maximum system frequency, to update the
    // voltage scaling value regarding system frequency refer to product
    // datasheet.
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Enable HSE Oscillator and activate PLL with HSE as source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    // This assumes the HSE_VALUE is a multiple of 1MHz. If this is not
    // your case, you have to recompute these PLL constants.

    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
    // clocks dividers
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
                                   | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

void STEP_Init()
{
    RCC_Init();

    /* LCD Initialization */
    BSP_LCD_Init();

    /* LCD Initialization */
    BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER);
    BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER+(BSP_LCD_GetXSize()*BSP_LCD_GetYSize()*4));

    /* Enable the LCD */
    BSP_LCD_DisplayOn();

    /* Select the LCD Background Layer  */
    BSP_LCD_SelectLayer(0);

    /* Clear the Background Layer */
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    /* Select the LCD Foreground Layer  */
    BSP_LCD_SelectLayer(1);

    /* Clear the Foreground Layer */
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    /* Configure the transparency for foreground and background :
         Increase the transparency */
    BSP_LCD_SetTransparency(0, 0);
    BSP_LCD_SetTransparency(1, 0xFF);

    BSP_LCD_SetFont(&FONT);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

    HAL_Delay(50); // more stable lcd init
}

void STEP_DBG_Osc(void)
{
    char buf[20] = "";
    sprintf(buf, "%d MHz", SystemCoreClock/1000000);
    STEP_Println(buf);
}

static volatile unsigned short line_num = 0;
static volatile unsigned short line_row = 0;
void STEP_Print(const char * str)
{
    BSP_LCD_DisplayStringAt(line_row * FONT.Width,
                            (line_num%26) * FONT.Height,
                            (uint8_t *)str,
                            LEFT_MODE);
    int i = 0;
    while(str[i++]) line_row++;
}

void STEP_Println(const char * str)
{
    STEP_Print(str);
    line_num++;
    BSP_LCD_ClearStringLine(line_num%26);
    line_row = 0;
}

void STEP_ClrScr(void)
{
    BSP_LCD_SelectLayer(1);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    line_num = 0;
    line_row = 0;
}

HAL_StatusTypeDef STEP_UART_Receive_IT(UART_HandleTypeDef *huart, RingBuffer *rBuffer)
{
    uint8_t rxByte;
    uint32_t tmp1, tmp2;

    tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
    tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
    /* UART in mode Receiver ---------------------------------------------------*/
    if((tmp1 != RESET) && (tmp2 != RESET))
    {
        if((huart->State == HAL_UART_STATE_BUSY_RX) || (huart->State == HAL_UART_STATE_BUSY_TX_RX))
        {
            if(huart->Init.WordLength == UART_WORDLENGTH_9B)
            {
                return HAL_ERROR; //implement your logic to handle 9B words
            }
            else
            {
                if(huart->Init.Parity == UART_PARITY_NONE)
                {
                    rxByte = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
                }
                else
                {
                    rxByte = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
                }
            }

            RingBuffer_PutByte(rBuffer, rxByte);
        }
    }
    else
    {
        HAL_UART_IRQHandler(huart); // вызываем обработчик прерывания в библиотеке
    }

    if (huart->State == HAL_UART_STATE_BUSY_TX)
    {
        huart->State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
        huart->State = HAL_UART_STATE_BUSY_RX;
    }

    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

    return HAL_OK;
}

