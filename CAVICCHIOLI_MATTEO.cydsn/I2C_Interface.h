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
#ifndef I2C_Interface_H
    #define I2C_Interface_H
    
    
    #include "cytypes.h"
    #include "Error.h"
    
    uint8_t stop_read; //
    uint8_t stop_write;
    
    ErrorCode I2C_Peripheral_Start(void);
    ErrorCode I2C_Peripheral_Stop(void);
    
    ErrorCode I2C_Peripheral_ReadRegister(uint8_t device_address, 
                                            uint8_t register_address,
                                            uint8_t* data);

    ErrorCode I2C_Peripheral_WriteRegister(uint8_t device_address,
                                            uint8_t register_address,
                                            uint8_t data);
    
#endif
    

/* [] END OF FILE */
