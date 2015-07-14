/*
* bh1750xtra.cpp
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

#include "bh1750xtra.h"

bh1750xtra::bh1750xtra(void) { // constructor, default
    I2C_Slave_Address = BH1750FVI_I2C_Slave_Address_L; // I2C Slave Address (in this implementation, the default is L)
    Resolution_Mode=BH1750FVI_Continuously_H_Resolution_Mode;
    MTreg=69;
    Continuously=BH1750FVI_Continuously;
}

bh1750xtra::bh1750xtra(uint8_t I2Cadd) { // constructor, set I2C Slave Address
    Set_I2C_Slave_Address(I2Cadd); // set I2C Slave Address
    Resolution_Mode=BH1750FVI_Continuously_H_Resolution_Mode;
    MTreg=69;
    Continuously=BH1750FVI_Continuously;
}

uint8_t bh1750xtra::Get_I2C_Slave_Address() { // get I2C Slave Address
    return(I2C_Slave_Address);
}

uint8_t bh1750xtra::Get_Resolution_Mode() { // get Resolution Mode
    return(Resolution_Mode);
}

uint8_t bh1750xtra::Get_MTreg() { // get MTreg (measurement time register)
    return(MTreg);
}

void bh1750xtra::Set_I2C_Slave_Address(uint8_t I2Cadd)  // set I2C Slave Address
{
    if ((I2Cadd == BH1750FVI_I2C_Slave_Address_H)||(I2Cadd == BH1750FVI_I2C_Slave_Address_L))
        I2C_Slave_Address = I2Cadd; // there are only 2 valid addresses
    else I2C_Slave_Address = BH1750FVI_I2C_Slave_Address_L; // I2C Slave Address (in this implementation, the default is L)
    }

float bh1750xtra::Get_Read_Lux() { // get read buffer data in Lux
    return(BH1750FVI_Read_Buffer / (((Resolution_Mode==BH1750FVI_Continuously_L_Resolution_Mode) ||
    (Resolution_Mode==BH1750FVI_One_Time_L_Resolution_Mode))?2.4:1.2));
}

uint16_t bh1750xtra::Get_Read_Buffer() { // get read buffer raw data
    return(BH1750FVI_Read_Buffer);
}

void bh1750xtra::Power_Down() { // sleep mode
    Write_I2C(BH1750FVI_Power_Down);
}

void bh1750xtra::Power_On() { // power on
    Write_I2C(BH1750FVI_Power_On);
    Change_Resolution_Mode(Resolution_Mode);
    Change_Sensor_MTreg(MTreg);
}

void bh1750xtra::Write_I2C(uint8_t BH1750FVIinstruction) // I2C write data
{
    Wire.beginTransmission(I2C_Slave_Address); // begin I2C transmission
    #if (ARDUINO >= 100)
        Wire.write(BH1750FVIinstruction); // transmit BH1750FVI instruction
    #else
        Wire.send(BH1750FVIinstruction); // transmit BH1750FVI instruction
    #endif
    Wire.endTransmission(); // end I2C transmission
}

uint8_t bh1750xtra::Read_I2C_To_Buffer() // I2C read data to buffer
{
    uint8_t nBytes=0;
    uint8_t Buffer[2];
    Wire.beginTransmission(I2C_Slave_Address); // begin I2C transmission
    Wire.requestFrom(I2C_Slave_Address, 2u); // request I2C transmission of 2 bytes
    while((nBytes<2)) //Wire.available() &
    #if (ARDUINO >= 100)
        Buffer[1-nBytes++] = Wire.read(); // receive BH1750FVI data
    #else
        Buffer[1-nBytes++] = Wire.receive(); // receive BH1750FVI data
    #endif
    Wire.endTransmission(); // end I2C transmission
    memcpy(&BH1750FVI_Read_Buffer, Buffer, 2); // store data in read buffer
    return nBytes; // return number of read bytes (size of data, not the data itself!)
}

void bh1750xtra::Change_Resolution_Mode(byte BH1750FVI_Resolution_Mode){ // Change Resolution Mode
    // Available modes (continuous or one-time):
    // Low Resolution Mode
    // High Resolution Mode
    // High Resolution Mode 2
    if ((BH1750FVI_Resolution_Mode==BH1750FVI_Continuously_L_Resolution_Mode)||
        (BH1750FVI_Resolution_Mode==BH1750FVI_Continuously_H_Resolution_Mode)||
    (BH1750FVI_Resolution_Mode==BH1750FVI_Continuously_H_Resolution_Mode2)||
    (BH1750FVI_Resolution_Mode==BH1750FVI_One_Time_L_Resolution_Mode)||
    (BH1750FVI_Resolution_Mode==BH1750FVI_One_Time_H_Resolution_Mode)||
    (BH1750FVI_Resolution_Mode==BH1750FVI_One_Time_H_Resolution_Mode2)){
        Resolution_Mode=BH1750FVI_Resolution_Mode;
    } else Resolution_Mode=BH1750FVI_Continuously_H_Resolution_Mode;
    Write_I2C(Resolution_Mode); // Changing the Resolution Mode
}

void bh1750xtra::Change_Sensor_MTreg(byte BH1750FVI_MTreg){ // Sensor Measurement Time = MTreg (measurement time register)
    if ((MTreg>=69)&(MTreg<=254)){ // valid MTreg in [69, 254]
        byte MTreg_H=BH1750FVI_Change_Measurement_Time_H, MTreg_L=BH1750FVI_Change_Measurement_Time_L;
        MTreg=BH1750FVI_MTreg; // steore the new value of MTreg
        MTreg_H|=(MTreg>>5); // Sensor instruction for High bit of MTreg
        MTreg_L|=(MTreg && 0b00011111); // Sensor instruction for Low bit of MTreg
        Write_I2C(MTreg_H); // Changing High bit of MTreg
        Write_I2C(MTreg_L); // Changing Low bit of MTreg
    }
}

float bh1750xtra::Calculate_Sensitivity() // Sensitivity
{ return (MTreg/69.0); }

float bh1750xtra::Calculate_Max_lx() // min Lux
{
    return (((Resolution_Mode==BH1750FVI_Continuously_H_Resolution_Mode2) ||
    (Resolution_Mode==BH1750FVI_One_Time_H_Resolution_Mode2))?1884131.0/MTreg: 3768262.0/MTreg);
    // 1.0/1.2*(69.0/MTreg)*65535.0 This is the max resolution in lx for L and H
    // 1.0/1.2*(69.0/MTreg/2)*65535.0 This is the max resolution in lx for H mode 2
}

float bh1750xtra::Calculate_Min_lx() // max Lux
{
    return (((Resolution_Mode==BH1750FVI_Continuously_H_Resolution_Mode2) ||
    (Resolution_Mode==BH1750FVI_One_Time_H_Resolution_Mode2))?28.75/MTreg: 57.5/MTreg);
    // 1.0/1.2*(69.0/MTreg) This is the minimum non-zero value == resolution in lx for L and H
    // 1.0/1.2*(69.0/MTreg/2) This is the minimum non-zero value == resolution in lx for H mode 2
}

float bh1750xtra::Calculate_Typical_Measurement_Time_ms() {
    return (Calculate_Typical_Measurement_Time_ms(MTreg,Resolution_Mode)); // Typical Measurement Time
}

float bh1750xtra::Calculate_Typical_Measurement_Time_ms(uint8_t MTregIN) {
    return (Calculate_Typical_Measurement_Time_ms(MTregIN,Resolution_Mode)); // Typical Measurement Time
}

float bh1750xtra::Calculate_Typical_Measurement_Time_ms(uint8_t MTregIN, uint8_t Resolution_Mode_IN) { // Typical Measurement Time
    float t;
    t=(Resolution_Mode_IN==0)?16:120;
    return (t*MTregIN/69.0);
}

float bh1750xtra::Calculate_Max_Measurement_Time_ms() { // Max Measurement Time
    float t=(Resolution_Mode==0)?24:180;
    return (t*MTreg/69.0);
}

void bh1750xtra::Set_Continuously(){ // change the measurements to "Continuously"
    Continuously=BH1750FVI_Continuously;
    Resolution_Mode &=~BH1750FVI_One_Time;
    Resolution_Mode |=Continuously;
    Change_Resolution_Mode(Resolution_Mode);
}

void bh1750xtra::Set_One_Time(){ // change the measurements to "One Time"
    Continuously=BH1750FVI_One_Time;
    Resolution_Mode &=~BH1750FVI_Continuously;
    Resolution_Mode |=Continuously;
    Change_Resolution_Mode(Resolution_Mode);
}

void bh1750xtra::Set_To_Max_Resolution_Measurement() { // highest resolution (lx) measurement
    // set Sensor Measurement Time = MTreg (measurement time register) to highest value (254)
    // in H-Resolution Mode 2
    // the slowest measurement: at 0.11lx resolution takes 441 ms in H-Resolution Mode 2
    // the range is 0 to 7417.84 lx
    Change_Resolution_Mode(BH1750FVI_HResolution_Mode2 | Continuously);
    Change_Sensor_MTreg(254);
}

void bh1750xtra::Set_To_Fastest_Measurement() { // fastest measurement
    // set Sensor Measurement Time = MTreg (measurement time register) to lowest value (31)
    // in L-Resolution Mode
    // the fastest measurement takes 7 ms in L-Resolution Mode
    // the range is 0 to 121556.9 lx
    Change_Resolution_Mode(BH1750FVI_LResolution_Mode | Continuously);
    Change_Sensor_MTreg(31);
}

void bh1750xtra::Set_To_Highest_Range() {
    // measuring at the highest range (100000lx)
    // set Sensor Measurement Time = MTreg (measurement time register) to lowest value (31)
    // in H-Resolution Mode
    // measurement at the highest range takes 54 ms in H-Resolution Mode
    // the range is 0 to 121556.9 lx
    Change_Resolution_Mode(BH1750FVI_HResolution_Mode | Continuously);
    Change_Sensor_MTreg(31);
}

void bh1750xtra::Set_Highest_Resolution_From_Measurement_Time(float millisec) {
    // making the highest resolution measurements for a particular measurement time in ms
    // 7 to 53.8 ms L-Resolution Mode, 53.9 to 441 ms H-Resolution Mode 2
    uint8_t MTreg;
    if (millisec<=53.8){
        Change_Resolution_Mode(BH1750FVI_LResolution_Mode | Continuously);
        MTreg=round(millisec*4.3125);
        if (MTreg<31) MTreg=31; else if (MTreg>254) MTreg=254;
    } else {
        Change_Resolution_Mode(BH1750FVI_HResolution_Mode2 | Continuously);
        MTreg=round(millisec*0.575);
        if (MTreg<31) MTreg=31; else if (MTreg>254) MTreg=254;
    }
    Change_Sensor_MTreg(MTreg);
}

void bh1750xtra::Set_Highest_Resolution_From_MTreg(uint8_t MTregIN=NAN) {
    // making the highest resolution measurements for a particular MTreg(measurement time register)
    // 7 to 53.8 ms L-Resolution Mode, 53.9 to 441 ms H-Resolution Mode 2
    if (isnan(MTregIN)) MTregIN=MTreg; else MTreg=MTregIN;
        if (MTreg<31) MTreg=31; else if (MTreg>254) MTreg=254;
    Change_Sensor_MTreg(MTreg);
    float millisec=Calculate_Typical_Measurement_Time_ms();
    if (millisec<=53.8){
        Change_Resolution_Mode(BH1750FVI_LResolution_Mode | Continuously);
    } else {
        Change_Resolution_Mode(BH1750FVI_HResolution_Mode2 | Continuously);
    }
}

void bh1750xtra::Set_Highest_Resolution_From_Range(float Max_Range) {
    // making the highest resolution measurement for the max range in lx
    // H-Resolution Mode range max 14835.68 to 121556.85
    // H-Resolution Mode 2 range max 7417.840 to 60778.427
    // H-Resolution Mode 2 for range max < 14836
    // H-Resolution Mode for range max > 14836
    uint8_t MTreg;
    if (Max_Range<14836){
        Change_Resolution_Mode(BH1750FVI_HResolution_Mode2 | Continuously);
        MTreg=round(1884131.0/Max_Range);
    } else {
        Change_Resolution_Mode(BH1750FVI_HResolution_Mode | Continuously);
        MTreg=round(3768262.0/Max_Range);
    }
    if (MTreg<31) MTreg=31; else if (MTreg>254) MTreg=254;
    Change_Sensor_MTreg(MTreg);
}

void bh1750xtra::Set_Fastest_Measurement_From_Resolution(float Max_Resolution) {
    // making the fastest measurement for a particular resolution (lx)
    // L-Resolution Mode 0.2263780 to 1.8548387 lx
    // H-Resolution Mode 2 0.1131890 to 0.9274194 lx
    // L-Resolution Mode 0.2263780 to 1.8548387 lx, H-Resolution Mode 2 0.1131890 to 0.2246094 lx
    uint8_t MTreg;
    if (Max_Resolution<0.2263780){
        Change_Resolution_Mode(BH1750FVI_HResolution_Mode2 | Continuously);
        MTreg=round(28.75/Max_Resolution);
    } else {
        Change_Resolution_Mode(BH1750FVI_LResolution_Mode | Continuously);
        MTreg=round(57.5/Max_Resolution);
    }
    if (MTreg<31) MTreg=31; else if (MTreg>254) MTreg=254;
    Change_Sensor_MTreg(MTreg);
}

void bh1750xtra::Set_Fastest_Measurement_From_Max_Range(float Max_Range) {
    // making the fastest measurements for a particular range in lx
    // L-Resolution Mode max range 14835.68 to 121556.9 lx
    // H-Resolution Mode 2 max range  7417.841 to 60778.43 lx
    // L-Resolution Mode 14835.68 to 121556.9 lx, H-Resolution Mode 2 7417.841 to 14719.775 lx
    uint8_t MTreg;
    if (Max_Range<14835.68){
        Change_Resolution_Mode(BH1750FVI_HResolution_Mode2 | Continuously);
        MTreg=round(1884131.0/Max_Range);
    } else {
        Change_Resolution_Mode(BH1750FVI_LResolution_Mode | Continuously);
        MTreg=round(3768262.0/Max_Range);
    }
    if (MTreg<31) MTreg=31; else if (MTreg>254) MTreg=254;
    Change_Sensor_MTreg(MTreg);
}

