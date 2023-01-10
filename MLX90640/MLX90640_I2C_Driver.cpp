/**
* @copyright (C) 2017 Melexis N.V.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
#include  "global.h"

//I2C i2c (p9, p10);

void MLX90640_I2CInit (void)
{   
//  i2c.stop(); // Run once to init I2C
}

void MLX90640_I2CFreqSet (int freq)
{
//  i2c.frequency (1000 * freq);  // No need. Can be used with MLX90640_I2CInit
}

int MLX90640_I2CRead (u8 slaveAddr, u16 startAddress, u16 nMemAddressRead, u16 *data)
{                          
  ErrorStatus ack;                               
  u8 cmd[2] = {0,0};

  cmd[0] = startAddress >> 8;
  cmd[1] = startAddress & 0x00FF;
    
  ack = i2c_write (slaveAddr, cmd, 2, 1);
  if (ack != SUCCESS) 
  {
    i2c_init ();
    return -1;
  }
  ack = i2c_read16 (slaveAddr, data, nMemAddressRead);
//  ack = i2c_read (slaveAddr, (u8 *)data, nMemAddressRead * 2);
  if (ack != SUCCESS)
  {
    i2c_init ();
    return -1;
  }
  return 0;   
} 

int MLX90640_I2CWrite (u8 slaveAddr, u16 writeAddress, u16 data)
{
  ErrorStatus ack;
  u8 cmd[4] = {0,0,0,0};
  u16 dataCheck;
  
  cmd[0] = writeAddress >> 8;
  cmd[1] = writeAddress & 0x00FF;
  cmd[2] = data >> 8;
  cmd[3] = data & 0x00FF;
     
  ack = i2c_write (slaveAddr, cmd, 4, 0);
  
  if (ack == ERROR)
  {
    i2c_init ();
    return -1;         
  }
  
  MLX90640_I2CRead (slaveAddr, writeAddress, 1, &dataCheck);
  
  if (dataCheck != data)
  {
    return -2;
  }     
  return 0;
}

