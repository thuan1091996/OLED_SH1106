// *****************************************************************************
//
//------------------------- 0.Driver information -------------------------------
// This library is used to communicate between TM4C123 and I2C devices via I2C
// Created on: Nov, 6th  2019
// Author : TRAN MINH THUAN
// Driver associate with TM4C123, CCS version 9.x
// Hardware options:
//        |  SDA  |  SCL
// -------+-------+--------
//  I2C0  |  PB3  |  PB2
//  I2C1  |  PA7  |  PA6
//  I2C2  |  PE5  |  PE4
//  I2C3  |  PD1  |  PD0
//  Note: This version only allow to use 1 I2Cmodule.
//******************************************************************************

#include "Userlibs.h"
#include "I2C.h"

// *****************************************************************************
//
//! Initialize I2C module.
//!                     I2C_Init(bool b_FastMode)
//! \param:  b_FastMode: Setup for fast data transfer:
//!                    - true: I2C run with fast mode (400kbps).
//!                    - false: I2C run without fast mode (100kbps).
//! This function is used to initialize the I2C module that define in I2C.h.
//! \return None.
//
//******************************************************************************
void I2C_Init(bool b_FastMode)
{
    //
    //! Enable peripheral
    //
    SysCtlPeripheralEnable(I2C_PERIPH);
    while(!SysCtlPeripheralReady(I2C_PERIPH));
    SysCtlPeripheralEnable(I2C_GPIO_PERIPH);
    while(!SysCtlPeripheralReady(I2C_GPIO_PERIPH));
    //
    //! GPIO Configure for I2C pins
    //
    GPIOPinTypeI2CSCL(I2C_GPIO_PORT, SCL_PIN);                      //Configuration SCL pin for I2C communication
    GPIOPinConfigure(SCL_GPIO_CONFIG);
    GPIOPadConfigSet(I2C_GPIO_PORT, SCL_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPinTypeI2C(I2C_GPIO_PORT, SDA_PIN);                         //Configuration SDA pin for I2C communication
    GPIOPinConfigure(SDA_GPIO_CONFIG);
    GPIOPadConfigSet(I2C_GPIO_PORT, SDA_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD);
    //
    //! I2C configure
    //
    I2CMasterInitExpClk(I2C_PORT, SysCtlClockGet(), b_FastMode);    //Use System clock without fast mode (100Kbps)
}

// *****************************************************************************
//
//! Write 1 byte data to Slave devices.
//!       uint32_t I2C_Write_Byte(uint8_t ui8Slave_Addr, uint8_t ui8Data)
//! \param: ui8Slave_Addr: Is the 7-bit slave's address.
//! \param: ui8Data: Data to send to slave devices that are addressed.
//! This function is used to send 1 byte data to slave devices.
//! \return: The transmission status which is one of the following value:
//!          I2C_MASTER_ERR_NONE,I2C_MASTER_ERR_ADDR_ACK, I2C_MASTER_ERR_DATA_ACK, or
//!          I2C_MASTER_ERR_ARB_LOST.
//
//******************************************************************************
uint32_t I2C_Write_Byte(uint8_t ui8Slave_Addr, uint8_t ui8Data)
{
    //
    // Set slave address and operation (Write to slave)
    //
    I2CMasterSlaveAddrSet(I2C_PORT, ui8Slave_Addr, false);  //Send to slave
    //
    // Send data  to slave
    //
    I2CMasterDataPut(I2C_PORT, ui8Data);                    //Place data in Data register
    I2CMasterControl(I2C_PORT, I2C_MASTER_CMD_SINGLE_SEND); //Send only 1 byte
    while(I2CMasterBusy(I2C_PORT));                         //Wait for the transmission completes
    //
    // Check the transmission status
    //
    return I2CMasterErr(I2C_PORT);
}

// *****************************************************************************
//
//! Write multiple bytes data to Slave devices.
//! uint32_t I2C_Write_Bytes(uint8_t ui8Slave_Addr, uint8_t *ptData, uint16_t ui16NumbByte)
//! \param: ui8Slave_Addr: Is the 7-bit slave's address.
//! \param: *pt_Data: Pointer to the data to send to slave devices that are addressed.
//! \param: ui16NumbByte: The number of byte that need to be send to slave devices.
//! This function is used to send multiple bytes of data to slave devices.
//! \return: The transmission status
//!          - 0: if there are errors occur during transmission.
//!          - 1: if the successfully transmit the data.
//
//******************************************************************************
uint32_t I2C_Write_Bytes(uint8_t ui8Slave_Addr, uint8_t *ptData, uint16_t ui16NumbByte)
{
    uint32_t status=0;
    uint16_t sent_count=0;
    //
    // If just send 1 byte, call I2C_Write_Byte
    //
    if (ui16NumbByte==1)                                               //Make this function more universal
    {
        status=I2C_Write_Byte(ui8Slave_Addr,*ptData);
        if(status==I2C_MASTER_ERR_NONE) return 1;
        else                            return 0;
    }
    //
    // If just multiple bytes
    //
    else
    {
        //
        // Set slave address and operation (Write to slave)
        //
        I2CMasterSlaveAddrSet(I2C_PORT, ui8Slave_Addr, false);         //Send data to slave
        //
        // Send first byte data to slave
        //
        I2CMasterDataPut(I2C_PORT, *ptData);
        I2CMasterControl(I2C_PORT, I2C_MASTER_CMD_BURST_SEND_START);
        sent_count++;
        while(I2CMasterBusy(I2C_PORT));                                //Wait for the first transmission completes
        status = I2CMasterErr(I2C_PORT);                               //Check the transmission error
        //
        // Send the next data bytes continuously if there is no error
        //
        while(status==I2C_MASTER_ERR_NONE)
        {
            //
            // If this is not the last byte, continue send the data
            //
            if(sent_count< (ui16NumbByte-1) )
            {
                I2CMasterDataPut(I2C_PORT, *(ptData+sent_count));                //Send next byte of the data
                I2CMasterControl(I2C_PORT, I2C_MASTER_CMD_BURST_SEND_CONT);
                sent_count++;
                while(I2CMasterBusy(I2C_PORT));                                 //Wait for the transmission completes
                status = I2CMasterErr(I2C_PORT);                                //Check the transmission error
            }
            //
            // If encounter the last data byte
            //
            else
            {
                I2CMasterDataPut(I2C_PORT, *(ptData+sent_count));                //Send next byte of the data
                I2CMasterControl(I2C_PORT, I2C_MASTER_CMD_BURST_SEND_FINISH);
                while(I2CMasterBusy(I2C_PORT));                                 //Wait for the transmission completes
                status = I2CMasterErr(I2C_PORT);                                //Check the transmission error
                break;
            }
        }
        //
        // If there is no error -> return 1. If there is error stop the transmission and return 0
        //
        if(status==I2C_MASTER_ERR_NONE) return 1;
        else
        {
            I2CMasterControl(I2C_PORT, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
            return 0;
        }
    }
}


/***************************************************************
* Read 1 byte data from I2C Slave
* uint8_t I2C_Read_Byte(uint8_t ui8Slave_Addr, uint8_t ui8Reg_Addr).
* Input:
    - ui8Slave_Addr - Slave address.
    - ui8Reg_Addr - Register address to read data.
* Output: Return the read data.
***************************************************************/
uint8_t I2C_Read_Byte(uint8_t ui8Slave_Addr, uint8_t ui8Reg_Addr)
{
    uint8_t ui8_datarecv=0;
    I2CMasterSlaveAddrSet(I2C_PORT, ui8Slave_Addr, false);         //Setup slave address and force Master to transmitter mode
    I2CMasterDataPut(I2C_PORT, ui8Reg_Addr);                        //Put data into Master Data reg
    I2CMasterControl(I2C_PORT, I2C_MASTER_CMD_SINGLE_SEND);         //Pump the data from Master Data reg to I2C buses
    while(I2CMasterBusy(I2C_PORT));                                 //Wait for transmission complete
    I2CMasterSlaveAddrSet(I2C_PORT, ui8Slave_Addr, true);          //Setup new communication but this time Master have receiver mode
    I2CMasterControl(I2C_PORT, I2C_MASTER_CMD_SINGLE_RECEIVE);      //Receive data from I2C buses
    while(I2CMasterBusy(I2C_PORT));                                 //wait for receiving process complete
    ui8_datarecv=I2CMasterDataGet(I2C_PORT);                        //Read the data
    return ui8_datarecv;
}
