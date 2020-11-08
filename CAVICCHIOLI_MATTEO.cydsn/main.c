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
#define PRECISION 1000
//#define Q_digit_To_MS2 M_digit_TO_MS2*4095 

#define MSB 1
#define LSB 0
#define DATA_READY 0b00001000 

#define HEAD 0xA0
#define TAIL 0xC0

#define TRANSMIT_SIZE 1+ACC_BYTES+1
#define ACC_BYTES 6
#define START 0
#define AX_M 1
#define AX_L 2    
#define AY_M 3
#define AY_L 4
#define AZ_M 5
#define AZ_L 6
#define END TRANSMIT_SIZE-1

#define ACC_AXIAL_SIZE 2





ErrorCode error;


uint8 check;
uint8 freq;
volatile uint8 flag_button = 1;

uint8_t Acc_X[ACC_AXIAL_SIZE];
uint8_t Acc_Y[ACC_AXIAL_SIZE];
uint8_t Acc_Z[ACC_AXIAL_SIZE];

int16 A_X_u;
int16 A_Y_u;
int16 A_Z_u;

int16 A_X;
int16 A_Y;
int16 A_Z;

float A_X_Conv;
float A_Y_Conv;
float A_Z_Conv;

uint8_t Acc_TOT[TRANSMIT_SIZE];




int main(void)
{
    EEPROM_Start();
    //EEPROM_UpdateTemperature();
    //EEPROM_WriteByte(0xC0,0x0000);
    
    
    freq = EEPROM_ReadByte(FREQ_ADRESS);
    switch(freq) //first start
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
            EEPROM_WriteByte(CTRL_REG1_FREQ_1,FREQ_ADRESS);
            break;
    }
    freq = EEPROM_ReadByte(FREQ_ADRESS);
    
    //char message[50] = {'\0'};
    //uint8_t ctrl_reg1;
    
    CyGlobalIntEnable; 
    
    UART_Start();  
    I2C_Peripheral_Start();
    CyDelay(5);
    
    

    error = I2C_Peripheral_WriteRegister(DEVICE_ADDRESS, LIS3DH_CTRL_REG4, CTRL_REG4_HR);
    if (error == NO_ERROR)
    {
        error = I2C_Peripheral_WriteRegister(DEVICE_ADDRESS, LIS3DH_CTRL_REG1, freq);
        if (error == NO_ERROR)
        {
            
            UART_PutString("ACCELEROMETER READY");
        }
        else
        {
            UART_PutString("ERROR OCCURRED DURING REGISTER SETTING");   
        }
    }

    Clock_DEB_Start();
    ISR_BUTTON_StartEx(Custom_DEBOUNCER_ISR);
    
    Acc_TOT[START] = HEAD;
    Acc_TOT[END] = TAIL;
    
    for(;;)
    {
        if (flag_button == BUTTON_PRESSED && stop_read == I2C_MSTR_NO_ERROR && stop_write == I2C_MSTR_NO_ERROR)  //voglio che tutte le operazioni di lettura siano terminate prima di scrivere il registro
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
            
            I2C_Peripheral_WriteRegister(DEVICE_ADDRESS, LIS3DH_CTRL_REG1, freq); //funzione bloccante
            flag_button = BUTTON_UNPRESSED;
        }
        if (flag_button == BUTTON_UNPRESSED && stop_read == I2C_MSTR_NO_ERROR && stop_write == I2C_MSTR_NO_ERROR)
        {
            
            I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_STATUS_REG, &check);
            
            if (check & DATA_READY) //dati pronti 
            {
                error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_X_L, &Acc_X[LSB]);
                if (error == NO_ERROR)
                {
                    error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_X_H, &Acc_X[MSB]);
                    if (error == NO_ERROR)
                    {
                        error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_Y_L, &Acc_Y[LSB]);
                        if (error == NO_ERROR)
                        {
                            error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_Y_H, &Acc_Y[MSB]);
                            if (error == NO_ERROR)
                            {
                                error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_Z_L, &Acc_Z[LSB]);
                                if (error == NO_ERROR)
                                {
                                    error = I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_OUT_Z_H, &Acc_Z[MSB]);
                                }
                            }
                        }
                    }
                }
                
                if (error == NO_ERROR)
                {
                    A_X_u = (int16)((Acc_X[LSB] | (Acc_X[MSB]<<8)))>>4;
                    A_Y_u = (int16)((Acc_Y[LSB] | (Acc_Y[MSB]<<8)))>>4;
                    A_Z_u = (int16)((Acc_Z[LSB] | (Acc_Z[MSB]<<8)))>>4;
                    
                    /*if (A_X_u >= 0 && A_X_u <=2047) 
                    {
                        A_X_Conv = (A_X_u*M_digit_TO_MS2)*1000;     
                    }
                    else if (A_X_u >2047 && A_X_u <4096)
                    {
                        A_X_Conv = (A_X_u*M_digit_TO_MS2 - Q_digit_To_MS2)*1000;
                    }*/
                    
                    A_X_Conv = (A_X_u*M_digit_TO_MS2)*PRECISION;
                    A_X = (int16)(A_X_Conv);
                    
                    Acc_TOT[AX_M] = A_X >> 8;
                    Acc_TOT[AX_L] = A_X & 0xFF;

                    
                    /*if (A_Y_u >= 0 && A_Y_u <=2047) 
                    {
                        A_Y_Conv = (A_Y_u*M_digit_TO_MS2)*1000;     
                    }
                    else if (A_Y_u >2047 && A_Y_u <4096)
                    {
                        A_Y_Conv = (A_Y_u*M_digit_TO_MS2 - Q_digit_To_MS2)*1000;
                    }*/
                    
                    A_Y_Conv = (A_Y_u*M_digit_TO_MS2)*PRECISION;
                    A_Y = (int16)(A_Y_Conv);

                    Acc_TOT[AY_M] = (uint8_t)(A_Y >> 8);
                    Acc_TOT[AY_L] = (uint8_t)(A_Y & 0xFF);
                    
                    
                    /*if (A_Z_u >= 0 && A_Z_u <=2047) 
                    {
                        A_Z_Conv = (A_Z_u*M_digit_TO_MS2)*1000;     
                    }
                    else if (A_Z_u >2047 && A_Z_u <4096)
                    {
                        A_Z_Conv = (A_Z_u*M_digit_TO_MS2 - Q_digit_To_MS2)*1000;
                    }*/
                    
                    A_Z_Conv = (A_Z_u*M_digit_TO_MS2)*PRECISION;
                    A_Z = (int16)(A_Z_Conv);
                    
                    Acc_TOT[AZ_M] = (uint8_t)(A_Z >> 8);
                    Acc_TOT[AZ_L] = (uint8_t)(A_Z & 0xFF);
                    
                    UART_PutArray(Acc_TOT,TRANSMIT_SIZE);
                }
                else 
                {
                    UART_PutString("Error occurred during I2C comunication to accelerations registers 1\r\n"); 
                }
            }
        }
    }     
}

/* [] END OF FILE */
