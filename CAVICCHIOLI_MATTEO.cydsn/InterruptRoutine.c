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
    flag_button = BUTTON_PRESSED;
}
/* [] END OF FILE */
