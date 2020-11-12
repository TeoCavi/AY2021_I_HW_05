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
#define BUTTON_UNPRESSED    1
#define BUTTON_PRESSED      0
#define FREQ_ADRESS         0x0000
#define FREQ_VARIATION      0x10 

#define DEVICE_ADDRESS      0x18

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

#define LIS3DH_OUT_X_L      0x28
#define LIS3DH_OUT_X_H      0x29
#define LIS3DH_OUT_Y_L      0x2A
#define LIS3DH_OUT_Y_H      0x2B
#define LIS3DH_OUT_Z_L      0x2C
#define LIS3DH_OUT_Z_H      0x2D

#define M_digit_TO_MS2      4*9.8/4095 //nella cartella è presente un grafico .jpg che presenta la linea di conversione
#define PRECISION           1000

#define MSB                 1
#define LSB                 0
#define DATA_READY          0b00001000 

#define HEAD                0xA0
#define TAIL                0xC0

#define TRANSMIT_SIZE       1+ACC_BYTES+1
#define ACC_BYTES           6
#define START               0
#define AX_M                1
#define AX_L                2    
#define AY_M                3
#define AY_L                4
#define AZ_M                5
#define AZ_L                6
#define END                 TRANSMIT_SIZE-1

#define ACC_AXIAL_SIZE      2


/* [] END OF FILE */
