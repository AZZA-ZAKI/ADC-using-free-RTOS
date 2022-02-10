


#include"LIB\LSTD_TYPES.h"

#include"MCAL\MDIO\MDIO_Interface.h"
#include"MCAL\MADC\MADC_Interface.h"

#include"HAL\HCLCD\HCLCD_Interface.h"

#include"Free_RTOS\FreeRTOS.h"
#include"Free_RTOS\task.h"
#include"Free_RTOS\semphr.h"

#include<util\delay.h>

void ADC_SetNotification(void);
void APP_LCD_Display(void*pv);
void APP_ADC_Task(void*pv);
u32 ADC_Value=0;
xSemaphoreHandle ADCSem;
int main(void)
{
	HCLCD_Vid4Bits_Init();
	HCLCD_VidWriteString_4Bits("Welcome...");
	_delay_ms(1000);
	HCLCD_VidWriteCommand_4Bits(0x01>>4);
	HCLCD_VidWriteCommand_4Bits(0x01);

	ADCSem=xSemaphoreCreateCounting(1,0);

	xTaskCreate(APP_ADC_Task,NULL,100,NULL,0,NULL);
	xTaskCreate(APP_LCD_Display,NULL,250,NULL,1,NULL);

	MGIE_VidEnable();

	vTaskStartScheduler();

	while(1)
	{

	}
	return 0;
}
void ADC_SetNotification(void)
{
	xSemaphoreGive(ADCSem);
}
void APP_LCD_Display(void*pv)
{
	HCLCD_VidWriteString_4Bits("POT=");
	HCLCD_VidSetPosition_4BitsMode(1,10);
	HCLCD_VidWriteString_4Bits("Ohm");
	while(1)
	{
		HCLCD_VidSetPosition_4BitsMode(1,5);
		HCLCD_VidWriteNumber_4Bits(ADC_Value);
		vTaskDelay(100);
	}
}
void APP_ADC_Task(void*pv)
{
	u8 ADC_SemState=0;
	u16 DigitalValue;

	MADC_VidSetCallBack(ADC_SetNotification);
	MADC_VidInit();
	MADC_u16ADC_StartConversion_With_Interrupt(CHANNEL_0);
	MADC_VidADCInterruptEnable();
	while(1)
	{
		ADC_SemState=xSemaphoreTake(ADCSem,10);
		if(ADC_SemState==pdPASS)
		{
			DigitalValue=MADC_u16ADCRead();
			ADC_Value=((DigitalValue*5000UL)/1024);
			MADC_u16ADC_StartConversion_With_Interrupt(CHANNEL_0);
		}
		vTaskDelay(10);
	}
}
