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
#include "project.h"
#include "I2C_Interface.h"
#include "stdio.h"
#include "InterruptRoutine.h"


#define LIS3DH_OUT_X_L      0x28
#define LIS3DH_OUT_X_H      0x29
#define LIS3DH_OUT_Y_L      0x2A
#define LIS3DH_OUT_Y_H      0x2B
#define LIS3DH_OUT_Z_L      0x2C
#define LIS3DH_OUT_Z_H      0x2D

//#define M_digit_TO_MS2 2*9.8/2047
#define M_digit_TO_MS2      4*9.8/4095
//#define Q_digit_To_MS2 M_digit_TO_MS2*4095 



ErrorCode error;

uint8 freq;
uint8 check;
//volatile uint8 flag_button = 1;

uint8_t Acc_X[2];
uint8_t Acc_Y[2];
uint8_t Acc_Z[2];
int16 A_X_u;
int16 A_Y_u;
int16 A_Z_u;
int16 A_X;
int16 A_Y;
int16 A_Z;
float A_X_Conv;
float A_Y_Conv;
float A_Z_Conv;

uint8_t Acc_TOT[8];



int main(void)
{
    
    UART_Start();
    I2C_Peripheral_Start();
    
    CyDelay(5);
    
    EEPROM_Start();
    //EEPROM_UpdateTemperature();
    //EEPROM_WriteByte(0xC0,0x0000);
    
    
    freq = EEPROM_ReadByte(0x0000);
    switch(freq)
    {
        case CTRL_REG1_FREQ_1:
        case CTRL_REG1_FREQ_10:
        case CTRL_REG1_FREQ_25:
        case CTRL_REG1_FREQ_50:
        case CTRL_REG1_FREQ_100:
        case CTRL_REG1_FREQ_200:
            break;
        default:
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(CTRL_REG1_FREQ_1,0x0000);
            break;
    }
    freq = EEPROM_ReadByte(0x0000);
    
    char message[50] = {'\0'};
    
    CyGlobalIntEnable; 
    
    uint8_t ctrl_reg1;
    
    error = I2C_Peripheral_WriteRegister(DEVICE_ADDRESS, 
                              LIS3DH_CTRL_REG4, 
                              CTRL_REG4_HR);

    /*if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 4 successfully written as: 0x%02X\r\n", CTRL_REG4_HR);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to set control register 4\r\n");   
    }*/

    error = I2C_Peripheral_WriteRegister(DEVICE_ADDRESS, 
                      LIS3DH_CTRL_REG1, 
                      freq);
    
    error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 1 after overwrite operation: 0x%02X\r\n", ctrl_reg1);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read control register 1\r\n");   
    }
    
    /*if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", LIS3DH_CTRL_REG1_ODR);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to set control register 4\r\n");   
    }*/
    
    Clock_DEB_Start();
    ISR_BUTTON_StartEx(Custom_DEBOUNCER_ISR);
    
    Acc_TOT[0] = 0xA0;
    Acc_TOT[7] = 0xC0;
    
    for(;;)
    {
        
        I2C_Peripheral_ReadRegister(DEVICE_ADDRESS,
                                    LIS3DH_STATUS_REG,
                                    &check);
        if (check & 0b00001000)
        {
            error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_X_L, &Acc_X[0]);
            if (error == NO_ERROR)
            {
                error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_X_H, &Acc_X[1]);
                if (error == NO_ERROR)
                {
                    error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_Y_L, &Acc_Y[0]);
                    if (error == NO_ERROR)
                    {
                        error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_Y_H, &Acc_Y[1]);
                        if (error == NO_ERROR)
                        {
                            error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_Z_L, &Acc_Z[0]);
                            if (error == NO_ERROR)
                            {
                                error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_Z_H, &Acc_Z[1]);
                            }
                        }
                    }
                }
            }
            
            if (error == NO_ERROR)
            {
                A_X_u = (int16)((Acc_X[0] | (Acc_X[1]<<8)))>>4;
                A_Y_u = (int16)((Acc_Y[0] | (Acc_Y[1]<<8)))>>4;
                A_Z_u = (int16)((Acc_Z[0] | (Acc_Z[1]<<8)))>>4;
                
                /*if (A_X_u >= 0 && A_X_u <=2047) 
                {
                    A_X_Conv = (A_X_u*M_digit_TO_MS2)*1000;     
                }
                else if (A_X_u >2047 && A_X_u <4096)
                {
                    A_X_Conv = (A_X_u*M_digit_TO_MS2 - Q_digit_To_MS2)*1000;
                }*/
                
                A_X_Conv = (A_X_u*M_digit_TO_MS2)*1000;
                A_X = (int16)(A_X_Conv);
                
                Acc_TOT[1] = A_X & 0xFF;
                Acc_TOT[2] = A_X >> 8;

                
                /*if (A_Y_u >= 0 && A_Y_u <=2047) 
                {
                    A_Y_Conv = (A_Y_u*M_digit_TO_MS2)*1000;     
                }
                else if (A_Y_u >2047 && A_Y_u <4096)
                {
                    A_Y_Conv = (A_Y_u*M_digit_TO_MS2 - Q_digit_To_MS2)*1000;
                }*/
                
                A_Y_Conv = (A_Y_u*M_digit_TO_MS2)*1000;
                A_Y = (int16)(A_Y_Conv);

                Acc_TOT[3] = A_Y & 0xFF;
                Acc_TOT[4] = A_Y >> 8;
                
                /*if (A_Z_u >= 0 && A_Z_u <=2047) 
                {
                    A_Z_Conv = (A_Z_u*M_digit_TO_MS2)*1000;     
                }
                else if (A_Z_u >2047 && A_Z_u <4096)
                {
                    A_Z_Conv = (A_Z_u*M_digit_TO_MS2 - Q_digit_To_MS2)*1000;
                }*/
                
                A_Z_Conv = (A_Z_u*M_digit_TO_MS2)*1000;
                A_Z = (int16)(A_Z_Conv);
                
                Acc_TOT[5] = (uint8_t)(A_Z & 0xFF);
                Acc_TOT[6] = (uint8_t)(A_Z >> 8);
                
                
                UART_PutArray(Acc_TOT,8);
            }
            else 
            {
            UART_PutString("Error occurred during I2C comm to axis register 1\r\n"); 
            }
        }
    }     
}

/* [] END OF FILE */
