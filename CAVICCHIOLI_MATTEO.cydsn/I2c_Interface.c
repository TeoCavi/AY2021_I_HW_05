/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "I2C_Interface.h" 
#include "I2C.h"

ErrorCode I2C_Peripheral_Start(void) 
    {
        I2C_Start();  
        return NO_ERROR;
    }
    
    
ErrorCode I2C_Peripheral_Stop(void)
{
    I2C_Stop();
    return NO_ERROR;
}

ErrorCode I2C_Peripheral_ReadRegister(uint8_t device_address, 
                                        uint8_t register_address,
                                        uint8_t* data)
{
    uint8_t error = I2C_MasterSendStart(device_address,I2C_WRITE_XFER_MODE);
    if (error == I2C_MSTR_NO_ERROR)
    {
        error = I2C_MasterWriteByte(register_address);
        if (error == I2C_MSTR_NO_ERROR)
        {
            error = I2C_MasterSendRestart(device_address, I2C_READ_XFER_MODE);
            if (error == I2C_MSTR_NO_ERROR)
            {
                *data = I2C_MasterReadByte(I2C_NAK_DATA);
            }
        }
    }
    stop_read = I2C_MasterSendStop(); //variabile aggiunta per controllare effettiva uscita dalla bloccante che attende il bit di stop
    return error ? ERROR : NO_ERROR;
}

ErrorCode I2C_Peripheral_WriteRegister(uint8_t device_address,
                                        uint8_t register_address,
                                        uint8_t data)
{
    uint8_t error = I2C_MasterSendStart(device_address, I2C_WRITE_XFER_MODE);
    if (error == I2C_MSTR_NO_ERROR)
    {
        error = I2C_MasterWriteByte(register_address);
        if (error == I2C_MSTR_NO_ERROR)
        {
            error = I2C_MasterWriteByte(data);
        }
    }
    stop_write = I2C_MasterSendStop(); //variabile aggiunta per controllare effettiva uscita dalla bloccante che attende il bit di stop
    return error ? ERROR : NO_ERROR;
}



/* [] END OF FILE */
