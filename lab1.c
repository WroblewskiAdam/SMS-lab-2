#include <stdint.h>
#include "stm32f10x.h"
#include "lcd_hd44780.h"


#define DELAY_TIME 400000

void RCC_Config(void);
void GPIO_Config(void);
void LEDOn(void);
void LEDOff(void);
void Delay(unsigned int);
void ADC_Config(void);
unsigned int readADC( void );


int main(void) {
    RCC_Config() ; // konfiguracja RCC
    GPIO_Config() ; // konfiguracja GPIO
    GPIO_WriteBit(GPIOB , GPIO_Pin_9 , Bit_RESET);
    LCD_Initialize();

    unsigned char text1[7] = {"Hello\0"};
    unsigned char text2[8] = {"World!\0"};

    int a = readADC();

    // LCD_WriteText(&text1);
    // LCD_GoTo(0, 1);
    // LCD_WriteText(&text2);

    LCD_WriteText(&a);

    // timerInitStructure.TIM_Prescaler = 0; // prescaler = 0
    // timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up ; // zliczanie w gore
    // timerInitStructure.TIM_Period = 4095;
    // timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ; // dzielnik c z e s t o t l i w o s c i = 1
    // timerInitStructure.TIM_RepetitionCounter = 0; // brak powtorzen
    // TIM_TimeBaseInit (TIM4, &timerInitStructure ) ; // in ic ja li za cj a timera TIM4
    // TIM_Cmd (TIM4 , ENABLE ); // aktywacja timera TIM4
    //
    // // konfiguracja kanalu timera
    // outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1 ; // tryb PWM1
    // outputChannelInit.TIM_Pulse = 1024; // wypelnienie 1 0 2 4 / 4 0 9 5 * 1 0 0 % = 25%
    // outputChannelInit.TIM_OutputState = TIM_OutputState_Enable ; // stan Enable
    // outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High ; // polaryzacja Active High
    // TIM_OC3Init (TIM4, &outputChannelInit ); // in ic ja li za cj a kanalu 3 timera TIM4
    // TIM_OC3PreloadConfig (TIM4 , TIM_OCPreload_Enable );


    while(1) { // petla glowna programu
    LEDOn() ; // wlaczenie diody
    Delay(DELAY_TIME); // odczekanie 1s
    LEDOff() ; // wylaczenie diody
    Delay(DELAY_TIME); // odczekanie 1s

    if (GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_0) == 0)
        {
            GPIO_WriteBit(GPIOB , GPIO_Pin_9 , Bit_SET);
            LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT);
        }
    }
}


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void RCC_Config(void) {
    ErrorStatus HSEStartUpStatus;
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    HSEStartUpStatus = RCC_WaitForHSEStartUp() ;
    if(HSEStartUpStatus == SUCCESS){
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);

        RCC_HCLKConfig(RCC_SYSCLK_Div8);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1 , RCC_PLLMul_9);

        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        while (RCC_GetSYSCLKSource() != 0x08);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

        RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 , ENABLE );
        RCC_ADCCLKConfig( RCC_PCLK2_Div6 );
    }
}


void GPIO_Config (void) {
    GPIO_InitTypeDef GPIO_InitStructure ;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_AF;
    GPIO_Init(GPIOB , &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // wejscie w trybie pull -up
    GPIO_Init(GPIOA, & GPIO_InitStructure);

    GPIO_Init(GPIOC , &GPIO_InitStructure);
}

void LEDOn(void) {
    GPIO_WriteBit(GPIOB , GPIO_Pin_8 , Bit_SET);
}

void LEDOff(void) {
    GPIO_WriteBit(GPIOB , GPIO_Pin_8 , Bit_RESET);
}

void Delay(unsigned int counter ){
    while (counter--) { // sprawdzenie warunku
    __NOP(); // No Operation
    __NOP(); // No Operation
}
}


void ADC_Config ( void ) {
    ADC_InitTypeDef ADC_InitStructure ;
    GPIO_InitTypeDef GPIO_InitStructure ;

    ADC_DeInit( ADC1 ); // reset ustawien ADC1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ; // pin 4
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ; // szybkosc 50 MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ; // wyjscie w floating
    GPIO_Init(GPIOC , &GPIO_InitStructure );

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent ; // niezalezne dzialanie ADC 1 i 2
    ADC_InitStructure.ADC_ScanConvMode = DISABLE ; // pomiar pojedynczego kanalu
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE ; // pomiar na zadanie
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None ; // programowy start
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right ; // pomiar wyrownany do prawej
    ADC_InitStructure.ADC_NbrOfChannel = 1; // jeden kanal
    ADC_Init (ADC1 , &ADC_InitStructure); // i nic ja li za cj a ADC1
    ADC_RegularChannelConfig (ADC1 , 14 , 1, ADC_SampleTime_1Cycles5 ); // ADC1 , kanal 14 ,
    // 1.5 cyklu
    ADC_Cmd (ADC1 , ENABLE ); // aktywacja ADC1

    ADC_ResetCalibration( ADC1 ); // reset rejestru kalibracji ADC1
    while ( ADC_GetResetCalibrationStatus( ADC1 )) ; // oczekiwanie na koniec resetu
    ADC_StartCalibration( ADC1 ); // start kalibracji ADC1
    while ( ADC_GetCalibrationStatus( ADC1 )) ; // czekaj na koniec kalibracji
}


unsigned int readADC( void ){
    ADC_SoftwareStartConvCmd (ADC1 , ENABLE ) ; // start pomiaru
    while ( ADC_GetFlagStatus (ADC1 , ADC_FLAG_EOC ) == RESET ); // czekaj na koniec pomiaru
    return ADC_GetConversionValue ( ADC1 ); // odczyt pomiaru (12 bit )
}