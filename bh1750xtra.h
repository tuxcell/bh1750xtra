/*
* bh1750xtra.h
*
* bh1750xtra - BH1750FVI library for the Arduino exploring all range/sensitivity levels, from 0.11 lx to 100000 lx
* Author: Jose Gama 2015

*
* This library is free software; you can redistribute it
* and/or modify it under the terms of the GNU Lesser
* General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* This library is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the
* implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of the GNU Lesser
* General Public License along with this library; if not,
* write to the Free Software Foundation, Inc.,
* 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*
*/

#ifndef bh1750xtra_h
#define bh1750xtra_h
#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
#else
    #include <WProgram.h>
#endif
#include <Wire.h>


// BH1750FVI Instruction Set Architecture
// Reference: Digital 16bit Serial Output Type Ambient Light Sensor IC, 2011.11 - Rev.D
// (c) 2011 ROHM Co., Ltd. All rights reserved.

#define BH1750FVI_Power_Down 0b00000000 // No active state.

#define BH1750FVI_Power_On 0b00000001 // Waiting for measurement command.

#define BH1750FVI 0b00000111 // Reset Data register value. Reset command is not acceptable in Power Down mode.
// Reset command is for only reset Illuminance data register. It is used for removing previous measurement result.

#define BH1750FVI_Continuously_H_Resolution_Mode 0b00010000 // Start measurement at 1lx resolution. Measurement Time is typically 120ms.
#define BH1750FVI_Continuously_H_Resolution_Mode2 0b00010001 // Start measurement at 0.5lx resolution. Measurement Time is typically 120ms.
#define BH1750FVI_Continuously_L_Resolution_Mode 0b00010011 // Start measurement at 4lx resolution. Measurement Time is typically 16ms.

#define BH1750FVI_One_Time_H_Resolution_Mode 0b00100000 // Start measurement at 1lx resolution. Measurement Time is typically 120ms. It is automatically set to Power Down mode after measurement.
#define BH1750FVI_One_Time_H_Resolution_Mode2 0b00100001 // Start measurement at 0.5lx resolution. Measurement Time is typically 120ms. It is automatically set to Power Down mode after measurement.
#define BH1750FVI_One_Time_L_Resolution_Mode 0b00100011 // Start measurement at 4lx resolution. Measurement Time is typically 16ms. It is automatically set to Power Down mode after measurement.

#define BH1750FVI_Change_Measurement_Time_H 0b01000000 // ( High bit ) 01000_MT[7,6,5] Change measurement time.
// MTreg (measurement time register), 0100_0101 (default).
// Min.					Typ.		Max.
// 0001_1111				0100_0101	1111_1110
// ( sensitivity : default * 0.45 )	default		( sensitivity : default * 3.68 )

#define BH1750FVI_Change_Measurement_Time_L 0b01100000 // ( Low bit ) 011_MT[4,3,2,1,0] Change measurement time.

// I2C Slave Address determined by ADDR Terminal
#define BH1750FVI_I2C_Slave_Address_H 0b01011100 // ADDR = "H" ( ADDR >= 0.7VCC ) -> 1011100
#define BH1750FVI_I2C_Slave_Address_L 0b00100011 // ADDR = "L" ( ADDR <= 0.3VCC ) -> 0100011

// Resolution Modes (L, H and H mode 2), these values are for simplifying the code
#define BH1750FVI_HResolution_Mode  0b00
#define BH1750FVI_HResolution_Mode2  0b01
#define BH1750FVI_LResolution_Mode 0b11

// Continuously vs One_Time, these values are for simplifying the code
#define BH1750FVI_Continuously 0b00010000
#define BH1750FVI_One_Time 0b00100000

class bh1750xtra
{
    public:
    // constructor
    bh1750xtra( void ); // constructor, default
    bh1750xtra( uint8_t ); // constructor, set I2C Slave Address
    void Set_I2C_Slave_Address(uint8_t I2Cadd); // set I2C Slave Address
    uint8_t Get_I2C_Slave_Address(); // get I2C Slave Address
    float Get_Read_Lux(); // get read buffer data in Lux
    uint16_t Get_Read_Buffer(); // get read buffer raw data
    uint8_t Get_Resolution_Mode(); // get Resolution Mode
    uint8_t Get_MTreg(); // get MTreg (measurement time register)
    void Power_Down(); // sleep mode
    void Power_On(); // power on
    void Write_I2C(uint8_t BH1750FVIinstruction); // I2C write data
    uint8_t Read_I2C_To_Buffer(); // I2C read data to buffer
    void Change_Resolution_Mode(uint8_t BH1750FVI_Resolution_Mode); // Change Resolution Mode
    void Change_Sensor_MTreg(uint8_t BH1750FVI_MTreg); // Sensor Measurement Time = MTreg (measurement time register)
    float Calculate_Sensitivity(); // Sensitivity
    float Calculate_Max_lx(); // max Lux
    float Calculate_Min_lx(); // min Lux
    float Calculate_Typical_Measurement_Time_ms();
    float Calculate_Typical_Measurement_Time_ms(uint8_t MTregIN);
    float Calculate_Typical_Measurement_Time_ms(uint8_t MTregIN, uint8_t Resolution_Mode_IN);
    float Calculate_Max_Measurement_Time_ms(); // Max Measurement Time
    
    void Set_Continuously(); // change the measurements to "Continuously"
    void Set_One_Time(); // change the measurements to "One Time"
    void Set_To_Max_Resolution_Measurement(); // highest resolution (lx) measurement
    void Set_To_Fastest_Measurement(); // fastest measurement
    void Set_To_Highest_Range(); //  highest range (100000lx)
    void Set_Highest_Resolution_From_Measurement_Time(float millisec); // highest resolution measurements for a particular measurement time in ms
    void Set_Highest_Resolution_From_MTreg(uint8_t MTregIN); // highest resolution measurements for a particular MTreg(measurement time register)
    void Set_Highest_Resolution_From_Range(float Max_Range); // highest resolution measurement for the max range in lx
    void Set_Fastest_Measurement_From_Resolution(float Max_Resolution); //  fastest measurement for a particular resolution (lx)
    void Set_Fastest_Measurement_From_Max_Range(float Max_Range); // fastest measurements for a particular range in lx
    
    private:
    uint8_t I2C_Slave_Address; // I2C Slave Address (in this implementation, the default is H)
    uint8_t MTreg; // MTreg (measurement time register), adjusts sensor sensitivity, valid MTreg values = 31 to 254 (default 69)
    uint8_t Resolution_Mode; // Resolution Mode
    uint16_t BH1750FVI_Read_Buffer; // buffer for I2C read data
    uint16_t Continuously; //
};

#endif

