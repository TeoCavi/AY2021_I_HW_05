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
#ifndef __INTERRUPT_ROUTINE_H__
    #define __INTERRUPT_ROUTINE_H__
    
    #include "cytypes.h"
    #include "project.h"
    
    #define DEVICE_ADDRESS 0x18

    #define LIS3DH_STATUS_REG   0x27
    #define LIS3DH_CTRL_REG0    0x1E
    #define LIS3DH_CTRL_REG1    0x20
    #define LIS3DH_CTRL_REG4    0x23

    #define CTRL_REG4_HR        0x08
    
    #define CTRL_REG1_FREQ_1    0x17
    #define CTRL_REG1_FREQ_10   0x27
    #define CTRL_REG1_FREQ_25   0x37
    #define CTRL_REG1_FREQ_50   0x47
    #define CTRL_REG1_FREQ_100  0x57
    #define CTRL_REG1_FREQ_200  0x67
    
    CY_ISR_PROTO(Custom_DEBOUNCER_ISR);
    
    //volatile uint8 flag_button;
    
#endif
/* [] END OF FILE */
