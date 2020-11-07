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
#include "InterruptRoutine.h"
#include "I2C_Interface.h"

uint8 freq;

CY_ISR(Custom_DEBOUNCER_ISR)
{
    //flag_button = 0;
    freq = EEPROM_ReadByte(0x0000);
    
    
    switch(freq)
    {
        case CTRL_REG1_FREQ_200:
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(CTRL_REG1_FREQ_1, 0x0000);
            break;
        default:
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(freq + 0x10, 0x0000);
            break;
    }
    
    freq = EEPROM_ReadByte(0x0000);
    I2C_Peripheral_WriteRegister(DEVICE_ADDRESS, 
                                 LIS3DH_CTRL_REG1, 
                                 freq);
    //flag_button = 1;
    
}
/* [] END OF FILE */
