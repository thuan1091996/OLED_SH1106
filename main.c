// *****************************************************************************
//
//------------------------- 0.Project information ------------------------------
// OLED SH1106 Display
// Author : TRAN MINH THUAN
// Date: November 6th 2019
// Project associate with TM4C123, CCS version 9.
//
//******************************************************************************

// *****************************************************************************
//
//------------------------- 1.System requirement -------------------------------
//
// I2C communication
// Display char/string to OLED
//
//******************************************************************************


// *****************************************************************************
//
//---------------------- 2.Pre-Processor Directives ----------------------------
//
#include "UserLibraries/Userlibs.h"
#include "UserLibraries/I2C.h"
#include "UserLibraries/Fonts.h"

#define SH1106_SLAVEADDR        0x3C
#define SH1106_COMMAND          0x00
#define SH1106_DATA             0x40    //0b0100 0000: Co=0, D/C=1 -> last control byte since only 2 bytes send at a time

#define OLED_COLUMNSIZE 128
#define OLED_HEIGHT     64
#define OLED_PAGESIZE   8
// Note: Some Commands is double bytes command. The first byte
// is set mode and the second byte usually the data.
// Check datasheet for more information.
//  (2) stand for double bytes command
//  (2ds) stand for double-byte command and search the second byte prototype
// in datasheet
//  (d) stand for default setting
#define PUMP_VOLTAGE_7V8                            0x30    //(d)
#define PUMP_VOLTAGE_8V                             0x31
#define PUMP_VOLTAGE_8V4                            0x32
#define PUMP_VOLTAGE_9V                             0x33
#define CONTRAST_CONTROL_MODE                       0x81    //(2ds)
#define SEGMENT_REMAP_R                             0xA0    //(d)
#define SEGMENT_REMAP_L                             0xA1
#define ENTIRE_DISPLAY_ON                           0xA5
#define ENTIRE_DISPLAY_OFF                          0xA4    //(d)
#define NORMAL_DISPLAY                              0xA6    //(d)
#define REVERSE_DISPLAY                             0xA7
#define MULTIPLEX_RATION_MODE                       0xA8    //(2ds)
#define DC_DC_CONTROL_MODE                          0xAD    //(2)
#define DC_DC_CONTROL_MODE_ON                       0x8B    //(d)
#define DC_DC_CONTROL_MODE_OFF                      0x8A
#define DISPLAY_ON                                  0xAF
#define DISPLAY_OFF                                 0xAE    //(d)
#define COMMON_OUTPUT_SCAN_DIRECTION_0              0xC0    //(d)
#define COMMON_OUTPUT_SCAN_DIRECTION_1              0xC8
#define DISPLAY_OFFSET_MODE                         0xD3    //(2ds)
#define DISPLAY_DIVIDE_RATIO_OSC_MODE               0xD5    //(2ds)
#define DISCHARGE_PRECHARGE_PERIOD_MODE             0xD9    //(2ds)
#define COMMON_PADS_HARDWARE_CONFIG_MODE            0xDA    //(2ds)
#define COMMON_PADS_HARDWARE_CONFIG_MODE_SEQ        0x02
#define COMMON_PADS_HARDWARE_CONFIG_MODE_ALT        0x12    //(d)
#define VCOM_DESELECT_LEVEL_MODE                    0xDB    //(2ds)
#define READ_MODIFY_WRITE                           0xE0
#define END                                         0xEE
#define NOP                                         0xE3
//Enter or exit the sleep mode
#define ENTER                                       true
#define EXIT                                        false
/* For Oled_UpdateScreen and Oled_Clear function */
#define WHOLE_SCREEN                                0,0,OLED_COLUMNSIZE,OLED_HEIGHT //update whole screen
/* Oled_Draw8Pixel */
#define VERTICAL                                    true
#define HORIZONTAL                                  false
/* Oled_circle.c */
/* Oled_ellipse.c */
#define DRAW_UPPER_RIGHT 0x01
#define DRAW_UPPER_LEFT  0x02
#define DRAW_LOWER_LEFT 0x04
#define DRAW_LOWER_RIGHT  0x08
#define DRAW_ALL (DRAW_UPPER_RIGHT|DRAW_UPPER_LEFT|DRAW_LOWER_RIGHT|DRAW_LOWER_LEFT)
//******************************************************************************


// *****************************************************************************
//
//--------------------------- 3.Global Declarations ----------------------------
//
//******************************************************************************
uint8_t SH1106_Write_Command(uint8_t ui8Command);
uint8_t SH1106_Write_Data(uint8_t ui8Data);
void SH1106_Init(void);
void SH1106_RefeshGRAM(void);
void SH1106_Clear(uint8_t ui8DispData);
void SH1106_SetPosition(uint8_t ui8ColAddr, uint8_t ui8PageAddr);
void SH1106_DrawPoint(uint8_t ui8chXpos, uint8_t ui8chYpos, uint8_t chPoint);
void SH1106_DisplayChar(uint8_t ui8chXpos, uint8_t ui8chYpos, uint8_t ui8chChr, uint8_t ui8chSize, uint8_t ui8chMode);

static uint8_t s_chDispalyBuffer[128][8];


// *****************************************************************************
//
//------------------------ 4. Subroutines definition ---------------------------
//
// *****************************************************************************

void main(void)
{
    int i;
    unsigned char ii;
    SysCtlClockSet(SYSCTL_SYSDIV_2_5| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //80MHz
    I2C_Init(false);                                    //initialize I2C module without fast transfer
    SH1106_Init();
    SH1106_Clear(0);
    SH1106_DisplayChar(1,1,16,5,1);
    SH1106_DisplayChar(1,1,'T',16,0);

//    for(i=0;i<128;i++)
//    {
//        SH1106_DrawPoint(i, 0, 1);
//    }
//    for(i=0;i<128;i++)
//    {
//        SH1106_DrawPoint(i, 63, 1);
//    }
//    for(i=0;i<64;i++)
//    {
//        SH1106_DrawPoint(0, i, 1);
//    }
//    for(i=0;i<64;i++)
//    {
//        SH1106_DrawPoint(127, i, 1);
//    }
    SH1106_RefeshGRAM();

    while(1)
    {


    }
}

uint8_t SH1106_Write_Command(uint8_t ui8Command)
{
    uint8_t status=0;
    uint8_t data_2send[2]={SH1106_COMMAND,ui8Command};
    status=I2C_Write_Bytes(SH1106_SLAVEADDR, &data_2send[0], 2);
    return status;
}

uint8_t SH1106_Write_Data(uint8_t ui8Data)
{
    uint8_t status=0;
    uint8_t data_2send[2]={SH1106_DATA,ui8Data};
    status=I2C_Write_Bytes(SH1106_SLAVEADDR, &data_2send[0], 2);
    return status;
}

void SH1106_Init(void)
{

//=====================================================================//
//    SysCtlDelay(SysCtlClockGet()/2500);     //Power on time
//    SH1106_Write_Command(DISPLAY_OFF);      //Turn off display
//    SH1106_Write_Command(COMMON_PADS_HARDWARE_CONFIG_MODE);
//    SH1106_Write_Command(COMMON_PADS_HARDWARE_CONFIG_MODE_ALT);
//    SH1106_Write_Command(NORMAL_DISPLAY);
//    SH1106_Write_Command(MULTIPLEX_RATION_MODE);
//    SH1106_Write_Command(0x3F);
//    SH1106_Write_Command(DISPLAY_DIVIDE_RATIO_OSC_MODE);
//    SH1106_Write_Command(0x50);
//    SH1106_Write_Command(VCOM_DESELECT_LEVEL_MODE);
//    SH1106_Write_Command(0x35);
//    SH1106_Write_Command(CONTRAST_CONTROL_MODE);
//    SH1106_Write_Command(PUMP_VOLTAGE_7V8);
//    SH1106_Write_Command(DC_DC_CONTROL_MODE);
//    SH1106_Write_Command(DC_DC_CONTROL_MODE_ON);
//    SysCtlDelay(SysCtlClockGet()/30);
//    //Set start line for COM0 -> 0
//    SH1106_Write_Command(0x40);
//    SH1106_Clear(1);
//    //Turn on the display
//    SH1106_Write_Command(DISPLAY_ON);
//    SH1106_Write_Command(COMMON_OUTPUT_SCAN_DIRECTION_1);
//    SH1106_Write_Command(SEGMENT_REMAP_L);
//    SysCtlDelay(SysCtlClockGet()/60);
//=====================================================================//

    SysCtlDelay(SysCtlClockGet()/2500); //Power on time
    SH1106_Write_Command(DISPLAY_OFF);  //turn off oled panel
    SH1106_Write_Command(0x00);         //-set low column address  (Col 0)
    SH1106_Write_Command(0x10);         //-set high column address (Col 0)
    SH1106_Write_Command(0x40);         //set start line address (Line 0)
    SH1106_Write_Command(0x81);         //set contrast control register
    SH1106_Write_Command(0xCF);         // Set SEG Output Current Brightness
    SH1106_Write_Command(0xA1);         //Set SEG/Column Mapping, left rotate
    SH1106_Write_Command(0xC0);         //Set COM/Row Scan Direction
    SH1106_Write_Command(0xA6);         //set normal display
    SH1106_Write_Command(0xA8);         //set multiplex ratio(1 to 64)
    SH1106_Write_Command(0x3f);         //1/64 duty
    SH1106_Write_Command(0xD3);         //-set display offset Shift Mapping RAM Counter (0x00~0x3F)
    SH1106_Write_Command(0x00);         //-not offset
    SH1106_Write_Command(0xd5);         //set display clock divide ratio/oscillator frequency
    SH1106_Write_Command(0x80);         //set divide ratio, Set Clock as 100 Frames/Sec
    SH1106_Write_Command(0xD9);         //set pre-charge period
    SH1106_Write_Command(0xF1);         //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    SH1106_Write_Command(0xDA);         //set com pins hardware configuration
    SH1106_Write_Command(0x12);
    SH1106_Write_Command(0xDB);         //set vcomh
    SH1106_Write_Command(0x40);         //Set VCOM Deselect Level
    SH1106_Write_Command(0x20);         //-Set Page Addressing Mode (0x00/0x01/0x02)
    SH1106_Write_Command(0x02);         //
    SH1106_Write_Command(0x8D);         //set Charge Pump enable/disable
    SH1106_Write_Command(0x14);         //set(0x10) disable
    SH1106_Write_Command(0xA4);         // Disable Entire Display On (0xa4/0xa5)
    SH1106_Write_Command(0xA6);         // Disable Inverse Display On (0xa6/a7)
    SH1106_Write_Command(0xAF);         //turn on oled panel
    SH1106_Clear(00);
}

void SH1106_SetPosition(uint8_t ui8ColAddr, uint8_t ui8PageAddr)
{
    //plus 2 to the column_address to match with the display column
    SH1106_Write_Command(ui8PageAddr | 0xB0);
    SH1106_Write_Command((ui8ColAddr + 2) & 0x0F);
    SH1106_Write_Command(0x10 | ((ui8ColAddr + 2) >> 4));
}

void SH1106_Clear(uint8_t ui8DispData)
{
    uint8_t page_index;
    uint8_t column_index;
    for(page_index=0;page_index<8;page_index++)
    {
        SH1106_Write_Command(0xb0+page_index);
        SH1106_Write_Command(0x10);
        SH1106_Write_Command(0x02);
        for(column_index=0;column_index<128;column_index++)
        {
            SH1106_Write_Data(ui8DispData);
        }
    }
}

void SH1106_DrawPoint(uint8_t ui8chXpos, uint8_t ui8chYpos, uint8_t ui8chPoint)
{
    uint8_t chPos, chBx, chTemp = 0;

    if (ui8chXpos > 127 || ui8chYpos > 63)
    {
        return;
    }
    chPos = 7 - ui8chYpos / 8;
    chBx = ui8chYpos % 8;
    chTemp = 1 << (7 - chBx);

    if (ui8chPoint)
    {
        s_chDispalyBuffer[ui8chXpos][chPos] |= chTemp;

    }
    else
    {
        s_chDispalyBuffer[ui8chXpos][chPos] &= ~chTemp;
    }
}

void SH1106_DisplayChar(uint8_t ui8chXpos, uint8_t ui8chYpos, uint8_t ui8chChr, uint8_t ui8chSize, uint8_t ui8chMode)
{
    uint8_t i, j;
    uint8_t chTemp, chYpos0 = ui8chYpos;
    ui8chChr = ui8chChr - ' ';
    for (i = 0; i < ui8chSize; i ++)
    {
        if (ui8chSize == 12)
        {
            if (ui8chMode)
            {
                chTemp = c_chFont1206[ui8chChr][i];
            }
            else
            {
                chTemp = ~c_chFont1206[ui8chChr][i];
            }
        }
        else
        {
            if (ui8chMode)
            {
                chTemp = c_chFont1608[ui8chChr][i];
            }
            else
            {
                chTemp = ~c_chFont1608[ui8chChr][i];
            }
        }
        for (j = 0; j < 8; j ++)
        {
            if (chTemp & 0x80)
            {
                SH1106_DrawPoint(ui8chXpos, ui8chYpos, 1);
            }
            else
            {
                SH1106_DrawPoint(ui8chXpos, ui8chYpos, 0);
            }
            chTemp <<= 1;
            ui8chYpos ++;
            if ((ui8chYpos - chYpos0) == ui8chSize)
            {
                ui8chYpos = chYpos0;
                ui8chXpos ++;
                break;
            }
        }
    }
}


void SH1106_RefeshGRAM(void)
{
    uint8_t i, j;

    for (i = 0; i < 8; i ++) {
        SH1106_Write_Command(0xB0 + i);
        SH1106_Write_Command(0x02);
        SH1106_Write_Command(0x10);
        for (j = 0; j < 128; j ++)
        {
            SH1106_Write_Data(s_chDispalyBuffer[j][i]);
        }
    }
}
