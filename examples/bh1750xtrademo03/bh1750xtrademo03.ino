/*

* bh1750xtra - BH1750FVI library for the Arduino exploring all range/sensitivity levels, from 0.11 lx to 100000 lx
* Author: Jose Gama 2015

* demo for reading data in low, high and high 2 modes
* to show the raw data vs Lux values for all modes

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

#include <bh1750xtra.h>
#include <Wire.h>

bh1750xtra bhSensor; // instantiate bh1750xtra object

void setup() {
    uint8_t error;
    Serial.begin(9600); // initialize serial com at 9600 bauds
    Wire.begin(); // initialize I2C com
    Serial.println("bh1750xtra");
    
    // idea based on i2c_scanner by unknown author + Krodal +  louarnold + Nick Gammon
    // test to see if the bh1750 I2C Ids are available
    Wire.beginTransmission(35);
    error = Wire.endTransmission();
    if (error == 0) Serial.println("I2C device found at address 0x23"); else Serial.println("I2C device NOT found at address 0x23");
        Wire.beginTransmission(92);
    error = Wire.endTransmission();
    if (error == 0) Serial.println("I2C device found at address 0x5C"); else Serial.println("I2C device NOT found at address 0x5C");
        
    delay(1000);               // wait for a second
}

// the loop routine runs over and over again forever:
void loop() {
    int8_t nBytesRead;
    
    Serial.print("Low resolution mode   ");
    bhSensor.Change_Resolution_Mode(BH1750FVI_Continuously_L_Resolution_Mode);
    nBytesRead=bhSensor.Read_I2C_To_Buffer(); // read from sensor to buffer
    if (nBytesRead != 2)  Serial.print("I2C reading error "); else { // error!
        Serial.print(bhSensor.Get_Read_Buffer()); Serial.print(" (raw data) ");
        Serial.print(bhSensor.Get_Read_Lux()); Serial.println(" lx "); // read buffer value in Lux
    }
    delay(20);
    
    Serial.print("High resolution mode  ");
    bhSensor.Change_Resolution_Mode(BH1750FVI_Continuously_H_Resolution_Mode);
    nBytesRead=bhSensor.Read_I2C_To_Buffer(); // read from sensor to buffer
    if (nBytesRead != 2)  Serial.print("I2C reading error "); else { // error!
        Serial.print(bhSensor.Get_Read_Buffer()); Serial.print(" (raw data) ");
        Serial.print(bhSensor.Get_Read_Lux()); Serial.println(" lx "); // read buffer value in Lux
    }
    delay(150);
    
    Serial.print("High resolution mode 2 ");
    bhSensor.Change_Resolution_Mode(BH1750FVI_Continuously_H_Resolution_Mode2);
    nBytesRead=bhSensor.Read_I2C_To_Buffer(); // read from sensor to buffer
    if (nBytesRead != 2)  Serial.print("I2C reading error "); else { // error!
        Serial.print(bhSensor.Get_Read_Buffer()); Serial.print(" (raw data) ");
        Serial.print(bhSensor.Get_Read_Lux()); Serial.println(" lx "); // read buffer value in Lux
    }
    
    delay(1000);               // wait for a second
}

