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
#include "Constant_Values.h"

ErrorCode error;

uint8 check;
uint8 freq;
volatile uint8 flag_button = 1;

uint8_t Acc_X[ACC_AXIAL_SIZE];
uint8_t Acc_Y[ACC_AXIAL_SIZE];
uint8_t Acc_Z[ACC_AXIAL_SIZE];

int16 A_X_raw;
int16 A_Y_raw;
int16 A_Z_raw;

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
    
    /*alla primissima inizializzazione leggo il dato residuo contenuto nella EEPROM 
      e lo sovrascrivo con 1HZ (0x17) se è diverso dai valori ammessi */
    
    freq = EEPROM_ReadByte(FREQ_ADRESS);
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
            EEPROM_WriteByte(CTRL_REG1_FREQ_1,FREQ_ADRESS);
            break;
    }
    freq = EEPROM_ReadByte(FREQ_ADRESS); //memorizzo la frequenza per andare ad inserirla nel CONTROL_REG1 registro

    CyGlobalIntEnable; 
    
    //attivo l'I2C e l'UART, lasciando all'I2C un delay per completare l'inizializzazione
    UART_Start();  
    I2C_Peripheral_Start();
    CyDelay(5);
    
    //Scrittura in cascata dei registri 
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
        /*se ho ricevuto un ISR significa che il pulsante è stato premuto, e quindi cambio la frequenza 
         (N.B. lo faccio, per sicurezza, solo quando tutti i byte di stop di letture e scritture in corso sono stati ricevuti
          per evitare che un eventuale dato sul bus mi faccia perdere il byte di stop rimanendo
          in un loop infinito (WAITING su UART)*/
        
        if (flag_button == BUTTON_PRESSED && stop_read == I2C_MSTR_NO_ERROR && stop_write == I2C_MSTR_NO_ERROR) 
        {
            
            freq = EEPROM_ReadByte(FREQ_ADRESS);
            
            switch(freq)
            {
                //se sono <200Hz vado a sovrascrivere EEPROM con in dato
                case CTRL_REG1_FREQ_200:
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(CTRL_REG1_FREQ_1, FREQ_ADRESS);
                    break;
                
                //se supero 200Hz riparto da 1Hz
                default:
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(freq + FREQ_VARIATION, FREQ_ADRESS);
                    break;
            }
            
            freq = EEPROM_ReadByte(FREQ_ADRESS);
            
            //scrivo la frequenza modificata dalla pressione del tasto nel registro dell'accellerometro e rimetto il flag basso
            I2C_Peripheral_WriteRegister(DEVICE_ADDRESS, LIS3DH_CTRL_REG1, freq);
            flag_button = BUTTON_UNPRESSED;
        }
        
        /*in condizione di pulsante non premuto vado a leggere i dati del registro una volta pronti (controllo su STATUS_REG bit4)
          e li converto da digit ad accelelrazione in m/s^2 */
        
        if (flag_button == BUTTON_UNPRESSED && stop_read == I2C_MSTR_NO_ERROR && stop_write == I2C_MSTR_NO_ERROR)
        {
            
            I2C_Peripheral_ReadRegister(DEVICE_ADDRESS, LIS3DH_STATUS_REG, &check);
            
            if (check & DATA_READY)
            {
                //lettura in cascata dei dati (per leggerli in sequenza ed evitare errori (variabile error di controllo)
                
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
                
                //se tutto è stato letto correttamente converto
                
                if (error == NO_ERROR)
                {
                    
                    A_X_raw = (int16)((Acc_X[LSB] | (Acc_X[MSB]<<8)))>>4;
                    A_Y_raw = (int16)((Acc_Y[LSB] | (Acc_Y[MSB]<<8)))>>4;
                    A_Z_raw = (int16)((Acc_Z[LSB] | (Acc_Z[MSB]<<8)))>>4;
                    
                    A_X_Conv = (A_X_raw*M_digit_TO_MS2)*PRECISION;
                    A_X = (int16)(A_X_Conv);
                    
                    Acc_TOT[AX_M] = A_X >> 8;
                    Acc_TOT[AX_L] = A_X & 0xFF;

                    A_Y_Conv = (A_Y_raw*M_digit_TO_MS2)*PRECISION;
                    A_Y = (int16)(A_Y_Conv);

                    Acc_TOT[AY_M] = (uint8_t)(A_Y >> 8);
                    Acc_TOT[AY_L] = (uint8_t)(A_Y & 0xFF);
                  
                    A_Z_Conv = (A_Z_raw*M_digit_TO_MS2)*PRECISION;
                    A_Z = (int16)(A_Z_Conv);
                    
                    Acc_TOT[AZ_M] = (uint8_t)(A_Z >> 8);
                    Acc_TOT[AZ_L] = (uint8_t)(A_Z & 0xFF);
                    
                    
                    UART_PutArray(Acc_TOT,TRANSMIT_SIZE);
                }
                
                //se qualcosa è andato storto (es. rottura cavo) invio un messaggio d'errore
                
                else 
                {
                    UART_PutString("Error occurred during I2C comunication to accelerations registers 1\r\n"); 
                }
            }
        }
    }     
}

/* [] END OF FILE */
