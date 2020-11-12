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
    #include "Constant_Values.h"

    volatile uint8 flag_button;
    
    CY_ISR_PROTO(Custom_DEBOUNCER_ISR);
    
#endif
/* [] END OF FILE */
