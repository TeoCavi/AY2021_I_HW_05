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



CY_ISR(Custom_DEBOUNCER_ISR)
{
    
    freq = EEPROM_ReadByte(FREQ_ADRESS);
    
    
    switch(freq)
    {
        case CTRL_REG1_FREQ_200:
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(CTRL_REG1_FREQ_1, FREQ_ADRESS);
            break;
        default:
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(freq + FREQ_VARIATION, FREQ_ADRESS);
            break;
    }
    
    freq = EEPROM_ReadByte(FREQ_ADRESS);
    flag_button = BUTTON_PRESSED;
    /*I2C_Peripheral_WriteRegister(DEVICE_ADDRESS, 
                                 LIS3DH_CTRL_REG1, 
                                 freq);*/
    
}
/* [] END OF FILE */
