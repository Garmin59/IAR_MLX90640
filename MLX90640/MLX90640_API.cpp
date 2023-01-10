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
#include <MLX90640_I2C_Driver.h>
#include <MLX90640_API.h>






// store constant parameters in FLASH area
#pragma default_variable_attributes = @"MLX90640_DATA"
const const_data_MLX90640 MLXdata;
#pragma default_variable_attributes = 





void    ExtractVDDParameters        (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractPTATParameters       (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractGainParameters       (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractTgcParameters        (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractResolutionParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractKsTaParameters       (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractKsToParameters       (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractAlphaParameters      (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractOffsetParameters     (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractKtaPixelParameters   (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractKvPixelParameters    (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractCPParameters         (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
void    ExtractCILCParameters       (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
int     ExtractDeviatingPixels      (MLX90640_conf *conf, paramsMLX90640 *mlx90640);
int     CheckAdjacentPixels         (u16 pix1, u16 pix2);
int     CheckEEPROMValid            (u16 *eeData);  
float   GetMedian                   (float *values, int n);
int     IsPixelBad                  (u16 pixel, paramsMLX90640 *params);
ErrorStatus ValidateFrameData           (ram_data *data);
ErrorStatus ValidateAuxData             (ram_data *data);

bool    compare_data                (MLX90640_conf *conf, const_data_MLX90640 const *MLXdata);
u8      FLASH_erase_MLX90640_pages  (void);






int MLX90640_read_ee_param (u8 slaveAddr, u16 *eeData)
{
  return MLX90640_I2CRead (slaveAddr, EE_PARAM_START, EE_PARAM_SIZE, eeData);
}

int MLX90640_read_ee_pixels (u8 slaveAddr, u16 *eeData)
{
  return MLX90640_I2CRead (slaveAddr, EE_PIXEL_START, EE_PIXEL_SIZE, eeData);
}

int MLX90640_DumpEE (u8 slaveAddr, u16 *eeData)
{
  return MLX90640_I2CRead (slaveAddr, EEPROM_START, EEPROM_SIZE, eeData);
}

ErrorStatus MLX90640_GetFrameData (u8 slaveAddr, ram_data *frameData)
{
  ErrorStatus error = SUCCESS;
  
  MLX90640_I2CRead (slaveAddr, RAM_START    , RAM_SIZE, (u16 *)frameData);
  MLX90640_I2CRead (slaveAddr, MLX_REG_CR1  , 1       , (u16 *)&frameData->control);
  
  error = ValidateAuxData (frameData);  
  error = ValidateFrameData (frameData);
    
  return error; 
}

bool MLX90640_GetNewDataStatus (u8 slaveAddr)
{
  status_reg status;
  
  MLX90640_I2CRead (slaveAddr, MLX_REG_STATUS, 1, (u16 *)&status);
  if (status.new_data)
  {
    return true;
  }
  return false;
}


ErrorStatus ValidateFrameData (ram_data *data)
{
  u16 *frameData = (u16 *)data;
  u8 line = 0;
  
  for (int i = 0; i < 768; i += 32)
  {
    if ((frameData[i] == 0x7FFF) && ((line % 2) == data->status.subpage)) // frameData[833]))
    {
      return ERROR;
    }
    line++;
  }    
  return SUCCESS;    
}

ErrorStatus ValidateAuxData (ram_data *data)
{
  u16 *auxData = (u16 *)data->Ta_Vbe;
  
  if (data->Ta_Vbe == 0x7FFF) return ERROR;   
  
  for (int i = 8; i < 19; i++)
  {
    if(auxData[i] == 0x7FFF) return ERROR;
  }
  
  for (int i = 20; i < 23; i++)
  {
    if (auxData[i] == 0x7FFF) return ERROR;
  }
  
  for (int i = 24; i < 33; i++)
  {
    if (auxData[i] == 0x7FFF) return ERROR;
  }
  
  for (int i = 40; i < 51; i++)
  {
    if (auxData[i] == 0x7FFF) return ERROR;
  }
  
  for (int i = 52; i < 55; i++)
  {
    if (auxData[i] == 0x7FFF) return ERROR;
  }
  
  for (int i = 56; i < 64; i++)
  {
    if (auxData[i] == 0x7FFF) return ERROR;
  }
  
  return SUCCESS;   
}
  

int MLX90640_ExtractParameters (u16 *eeData, paramsMLX90640 *mlx90640)
{
  int error = 0;
  MLX90640_conf *conf = (MLX90640_conf *)eeData;
  
  ExtractVDDParameters            (conf, mlx90640);
  ExtractPTATParameters           (conf, mlx90640);
  ExtractGainParameters           (conf, mlx90640);
  ExtractTgcParameters            (conf, mlx90640);
  ExtractResolutionParameters     (conf, mlx90640);
  ExtractKsTaParameters           (conf, mlx90640);
  ExtractKsToParameters           (conf, mlx90640);
  ExtractCPParameters             (conf, mlx90640);
     
  if (!compare_data (conf, &MLXdata)) // compare data in FLASH to data in MLX90640
  {
    FLASH_Unlock ();
    FLASH_erase_MLX90640_pages ();
    
    ExtractAlphaParameters          (conf, mlx90640); // modified to write parameters to flash
    ExtractOffsetParameters         (conf, mlx90640); // modified to write parameters to flash
    ExtractKtaPixelParameters       (conf, mlx90640); // modified to write parameters to flash
    ExtractKvPixelParameters        (conf, mlx90640); // modified to write parameters to flash
    
    FLASH_ProgramHalfWord ((u32)(&MLXdata.dev_id1), conf->id1);
    FLASH_ProgramHalfWord ((u32)(&MLXdata.dev_id2), conf->id2);
    FLASH_ProgramHalfWord ((u32)(&MLXdata.dev_id3), conf->id3);
    FLASH_Lock ();
  }
    
  ExtractCILCParameters           (conf, mlx90640);
  error = ExtractDeviatingPixels  (conf, mlx90640);  
  return error; 
}

//------------------------------------------------------------------------------

int MLX90640_SetResolution (u8 slaveAddr, u8 resolution)
{
  union
  {
    control_reg cr_1;
    u16 reg;
  };
  int error;
  
  error = MLX90640_I2CRead (slaveAddr, MLX_REG_CR1, 1, (u16 *)&cr_1);
  
  if (!error)
  {
    cr_1.resolution = resolution;
    error = MLX90640_I2CWrite (slaveAddr, MLX_REG_CR1, reg);        
  }    
  
  return error;
}

//------------------------------------------------------------------------------

int MLX90640_GetCurResolution (u8 slaveAddr)
{
  control_reg cr_1;
  int resolutionRAM;
  int error;
  
  error = MLX90640_I2CRead (slaveAddr, MLX_REG_CR1, 1, (u16 *)&cr_1);
  if (error) return error;
  
  resolutionRAM = cr_1.resolution;
  
  return resolutionRAM; 
}

//------------------------------------------------------------------------------

int MLX90640_SetRefreshRate (u8 slaveAddr, u8 refreshRate)
{
  union
  {
    control_reg cr_1;
    u16         reg;
  };
  int error;
  
  error = MLX90640_I2CRead (slaveAddr, MLX_REG_CR1, 1, (u16 *)&cr_1);
  if (!error)
  {
    cr_1.refresh_rate = refreshRate;
    error = MLX90640_I2CWrite (slaveAddr, MLX_REG_CR1, reg);
  }    
  
  return error;
}

//------------------------------------------------------------------------------

int MLX90640_GetRefreshRate (u8 slaveAddr)
{
  control_reg cr_1;
  int refreshRate;
  int error;
  
  error = MLX90640_I2CRead (slaveAddr, MLX_REG_CR1, 1, (u16 *)&cr_1);
  if (error) return error;

  refreshRate = cr_1.refresh_rate;
  
  return refreshRate;
}

//------------------------------------------------------------------------------

int MLX90640_SetInterleavedMode (u8 slaveAddr)
{
  union
  {
    control_reg cr_1;
    u16 reg;
  };
  int error;
  
  error = MLX90640_I2CRead (slaveAddr, MLX_REG_CR1, 1, (u16 *)&cr_1);
  
  if (! error)
  {
    cr_1.chess = 0;
    error = MLX90640_I2CWrite (slaveAddr, MLX_REG_CR1, reg);        
  }    
  
  return error;
}

//------------------------------------------------------------------------------

int MLX90640_SetChessMode (u8 slaveAddr)
{
  union
  {
    control_reg cr_1;
    u16 reg;
  };
  int error;
  
  error = MLX90640_I2CRead (slaveAddr, MLX_REG_CR1, 1, (u16 *)&cr_1);
  
  if (!error)
  {
    cr_1.chess = 1;
    error = MLX90640_I2CWrite (slaveAddr, MLX_REG_CR1, reg);        
  }    
  
  return error;
}

//------------------------------------------------------------------------------

int MLX90640_GetCurMode (u8 slaveAddr)
{
  control_reg cr_1;
  int modeRAM;
  int error;
  
  error = MLX90640_I2CRead (slaveAddr, MLX_REG_CR1, 1, (u16 *)&cr_1);
  if (error) return error;
   
  modeRAM = cr_1.chess;
  
  return modeRAM; 
}

//------------------------------------------------------------------------------

void MLX90640_CalculateTo (ram_data *frameData, const paramsMLX90640 *params, s16 *result)
{
  float tr;
  float vdd;
  float ta;
  float ta4;
  float tr4;
  float taTr;
  float gain;
  float irDataCP[2];
  float irData;
  float alphaCompensated;
  float emissivity = params->emissivity;
  u8 mode;
  s8 ilPattern;
  s8 chessPattern;
  s8 pattern;
  s8 conversionPattern;
  float Sx;
  float To;
  float alphaCorrR[4];
  s8 range;
  u16 subPage;
  float ktaScale;
  float kvScale;
  float alphaScale;
  float kta;
  float kv;
  
  ta = MLX90640_GetTa (frameData, params); //Ta = 27.18
  tr = ta - 8.0f; //Ta – TA_SHIFT; //reflected temperature based on the sensor ambient temperature
  
  subPage = frameData->status.subpage;
  vdd = MLX90640_GetVdd (frameData, params);//  ta = MLX90640_GetTa (frameData, params);
    
  ta4 = (ta + 273.15);
  ta4 = ta4 * ta4;
  ta4 = ta4 * ta4;
  tr4 = (tr + 273.15);
  tr4 = tr4 * tr4;
  tr4 = tr4 * tr4;
  taTr = tr4 - (tr4 - ta4) / emissivity;
  
  ktaScale    = (float)(1 << params->const_data->ktaScale);
  kvScale     = (float)(1 << params->const_data->kvScale);
  alphaScale  = (float)(1 << params->const_data->alphaScale);

  
  alphaCorrR[0] = 1 / (1 + params->ksTo[0] * 40);
  alphaCorrR[1] = 1;
  alphaCorrR[2] = (1 + params->ksTo[1] * params->ct[1]);
  alphaCorrR[3] = alphaCorrR[2] * (1 + params->ksTo[2] * (params->ct[3] - params->ct[2]));
  
  //------------------------- Gain calculation -----------------------------------    
  gain = frameData->gain;
  gain = params->gainEE / gain; 
  
  //------------------------- To calculation -------------------------------------    
  mode = frameData->control.chess;
  
  irDataCP[0] = frameData->CP_SP0 * gain;  
  irDataCP[1] = frameData->CP_SP1 * gain;
  irDataCP[0] = irDataCP[0] - params->cpOffset[0] * (1 + params->cpKta * (ta - 25.0)) * (1 + params->cpKv * (vdd - 3.3));
  if (mode ==  params->calibrationModeEE)
  {
    irDataCP[1] = irDataCP[1] - params->cpOffset[1] * (1 + params->cpKta * (ta - 25.0)) * (1 + params->cpKv * (vdd - 3.3));
  }
  else
  {
    irDataCP[1] = irDataCP[1] - (params->cpOffset[1] + params->ilChessC[0]) * (1 + params->cpKta * (ta - 25.0)) * (1 + params->cpKv * (vdd - 3.3));
  }
  
  for (int pixelNumber = 0; pixelNumber < PIXEL_COUNT; pixelNumber++)
  {
    ilPattern = pixelNumber / 32 - (pixelNumber / 64) * 2; 
//    ilPattern = pixelNumber & 0x20;
    chessPattern = ilPattern ^ (pixelNumber - (pixelNumber / 2) * 2); 
//    chessPattern = ilPattern ^ (pixelNumber & 0x01);
    conversionPattern = ((pixelNumber + 2) / 4 - (pixelNumber + 3) / 4 + (pixelNumber + 1) / 4 - pixelNumber / 4) * (1 - 2 * ilPattern);
    
    if (mode == 0)
    {
      pattern = ilPattern; 
    }
    else 
    {
      pattern = chessPattern; 
    }               
    
    if (pattern == frameData->status.subpage)
    {    
      irData = frameData->pixel_data[pixelNumber];
      irData = irData * gain;
      
      kta = params->const_data->kta[pixelNumber] / ktaScale;
      kv = params->const_data->kv[pixelNumber] / kvScale;
      irData = irData - params->const_data->offset[pixelNumber] * (1 + kta * (ta - 25.0)) * (1 + kv * (vdd - 3.3));
      
      if (mode !=  params->calibrationModeEE)
      {
        irData = irData + params->ilChessC[2] * (2 * ilPattern - 1) - params->ilChessC[1] * conversionPattern; 
      }
      
      irData = irData - params->tgc * irDataCP[subPage];
      irData = irData / emissivity;     
      
      alphaCompensated = SCALEALPHA * alphaScale / params->const_data->alpha[pixelNumber];
      alphaCompensated = alphaCompensated * (1 + params->KsTa * (ta - 25.0));
            
      Sx = alphaCompensated * alphaCompensated * alphaCompensated * (irData + alphaCompensated * taTr);
      Sx = sqrt (sqrt (Sx)) * params->ksTo[1];            
      
      To = sqrt (sqrt (irData/(alphaCompensated * (1 - params->ksTo[1] * 273.15) + Sx) + taTr)) - 273.15;    
            
      if (To < params->ct[1])
      {
        range = 0;
      }
      else if (To < params->ct[2])   
      {
        range = 1;            
      }   
      else if (To < params->ct[3])
      {
        range = 2;            
      }
      else
      {
        range = 3;            
      }            
      To = sqrt (sqrt (irData / (alphaCompensated * alphaCorrR[range] * (1 + params->ksTo[range] * (To - params->ct[range]))) + taTr)) - 273.15;
      result[pixelNumber] = (s16)(To * 10.0f);
    }
  }
}

//------------------------------------------------------------------------------

void MLX90640_GetImage (ram_data *frameData, const paramsMLX90640 *params, u16 *result)
{
  float vdd;
  float ta;
  float gain;
  float irDataCP[2];
  float irData;
  float alphaCompensated;
  u8 mode;
  u8 ilPattern;
  u8 chessPattern;
  u8 pattern;
  u8 conversionPattern;
  u16 subPage;
  u16 image;
  float ktaScale;
  float kvScale;
  float kta;
  float kv;
    
  subPage = frameData->status.subpage;
  vdd = MLX90640_GetVdd (frameData, params);
  ta = MLX90640_GetTa (frameData, params);
  
  ktaScale = (float)(1 << params->const_data->ktaScale);
  kvScale = (float)(1 << params->const_data->kvScale);
  
  //------------------------- Gain calculation -----------------------------------    
  gain = frameData->gain;
  gain = params->gainEE / gain; 
  
  //------------------------- Image calculation -------------------------------------    
  mode = frameData->control.chess;
  
  irDataCP[0] = frameData->CP_SP0 * gain;  
  irDataCP[1] = frameData->CP_SP1 * gain;

  irDataCP[0] = irDataCP[0] - params->cpOffset[0] * (1 + params->cpKta * (ta - 25.0)) * (1 + params->cpKv * (vdd - 3.3));
  if (mode == params->calibrationModeEE)
  {
    irDataCP[1] = irDataCP[1] - params->cpOffset[1] * (1 + params->cpKta * (ta - 25.0)) * (1 + params->cpKv * (vdd - 3.3));
  }
  else
  {
    irDataCP[1] = irDataCP[1] - (params->cpOffset[1] + params->ilChessC[0]) * (1 + params->cpKta * (ta - 25.0)) * (1 + params->cpKv * (vdd - 3.3));
  }
  
  for (int pixelNumber = 0; pixelNumber < PIXEL_COUNT; pixelNumber++)
  {
    ilPattern = pixelNumber / 32 - (pixelNumber / 64) * 2; 
    chessPattern = ilPattern ^ (pixelNumber - (pixelNumber / 2) * 2); 
    conversionPattern = ((pixelNumber + 2) / 4 - (pixelNumber + 3) / 4 + (pixelNumber + 1) / 4 - pixelNumber / 4) * (1 - 2 * ilPattern);
    
    if (mode == 0)
    {
      pattern = ilPattern; 
    }
    else 
    {
      pattern = chessPattern; 
    }
    
    if (pattern == frameData->status.subpage)
    {    
      irData = frameData->pixel_data[pixelNumber];
      irData = irData * gain;
      
      kta = params->const_data->kta[pixelNumber] / ktaScale;
      kv = params->const_data->kv[pixelNumber] / kvScale;
      irData = irData - params->const_data->offset[pixelNumber] * (1 + kta * (ta - 25.0)) * (1 + kv * (vdd - 3.3));

      if (mode != params->calibrationModeEE)
      {
        irData = irData + params->ilChessC[2] * (2 * ilPattern - 1) - params->ilChessC[1] * conversionPattern; 
      }
      
      irData = irData - params->tgc * irDataCP[subPage];
      alphaCompensated =  params->const_data->alpha[pixelNumber];
      image = (u16)(irData * alphaCompensated);   
      result[pixelNumber] = image;
    }
  }
}

//------------------------------------------------------------------------------

float MLX90640_GetVdd (ram_data *frameData, const paramsMLX90640 *params)
{
  float vdd;
  float resolutionCorrection;
  int resolutionRAM;    
  
  vdd = frameData->VDD_pix;  // [810];
  resolutionRAM = frameData->control.resolution;
  resolutionCorrection = (float)(1 << params->resolutionEE) / (float)(1 << resolutionRAM);
  vdd = (resolutionCorrection * vdd - params->vdd25) / params->kVdd + 3.3;
  
  return vdd;
}

//------------------------------------------------------------------------------

float MLX90640_GetTa (ram_data *frameData, const paramsMLX90640 *params)
{
  float ptat;
  float ptatArt;
  float vdd;
  float ta;
  
  vdd = MLX90640_GetVdd (frameData, params);
  ptat = frameData->Ta_PTAT; // [800];
  ptatArt = frameData->Ta_Vbe; // [768];
  ptatArt = (ptat / (ptat * params->alphaPTAT + ptatArt)) * (float)(1 << 18);
  
  ta = (ptatArt / (1 + params->KvPTAT * (vdd - 3.3)) - params->vPTAT25);
  ta = ta / params->KtPTAT + 25.0;
  
  return ta;
}

//------------------------------------------------------------------------------

int MLX90640_GetSubPageNumber (ram_data *frameData)
{
  return frameData->status.subpage;    
}    

//------------------------------------------------------------------------------
void MLX90640_BadPixelsCorrection (u16 *pixels, float *to, int mode, paramsMLX90640 *params)
{   
  float ap[4];
  u8 pix;
  u8 line;
  u8 column;
  
  pix = 0;
  while (pixels[pix] != 65535)
  {
    line = pixels[pix] >> 5;
    column = pixels[pix] - (line << 5);
    
    if (mode == 1)
    {        
      if (line == 0)
      {
        if (column == 0)
        {        
          to[pixels[pix]] = to[33];                    
        }
        else if (column == 31)
        {
          to[pixels[pix]] = to[62];                      
        }
        else
        {
          to[pixels[pix]] = (to[pixels[pix] + 31] + to[pixels[pix] + 33]) / 2.0;                    
        }        
      }
      else if (line == 23)
      {
        if (column == 0)
        {
          to[pixels[pix]] = to[705];                    
        }
        else if (column == 31)
        {
          to[pixels[pix]] = to[734];                       
        }
        else
        {
          to[pixels[pix]] = (to[pixels[pix] - 33] + to[pixels[pix] - 31]) / 2.0;                       
        }                       
      } 
      else if (column == 0)
      {
        to[pixels[pix]] = (to[pixels[pix] - 31] + to[pixels[pix] + 33]) / 2.0;                
      }
      else if (column == 31)
      {
        to[pixels[pix]] = (to[pixels[pix] - 33] + to[pixels[pix] + 31]) / 2.0;                
      } 
      else
      {
        ap[0] = to[pixels[pix] - 33];
        ap[1] = to[pixels[pix] - 31];
        ap[2] = to[pixels[pix] + 31];
        ap[3] = to[pixels[pix] + 33];
        to[pixels[pix]] = GetMedian (ap, 4);
      }                   
    }
    else
    {        
      if (column == 0)
      {
        to[pixels[pix]] = to[pixels[pix] + 1];            
      }
      else if (column == 1 || column == 30)
      {
        to[pixels[pix]] = (to[pixels[pix] - 1] + to[pixels[pix] + 1]) / 2.0;                
      } 
      else if (column == 31)
      {
        to[pixels[pix]] = to[pixels[pix]-1];
      } 
      else
      {
        if (IsPixelBad (pixels[pix] - 2,params) == 0 && IsPixelBad (pixels[pix] + 2,params) == 0)
        {
          ap[0] = to[pixels[pix] + 1] - to[pixels[pix] + 2];
          ap[1] = to[pixels[pix] - 1] - to[pixels[pix] - 2];
          if (fabs (ap[0]) > fabs (ap[1]))
          {
            to[pixels[pix]] = to[pixels[pix] - 1] + ap[1];                        
          }
          else
          {
            to[pixels[pix]] = to[pixels[pix] + 1] + ap[0];                        
          }
        }
        else
        {
          to[pixels[pix]] = (to[pixels[pix] - 1] + to[pixels[pix] + 1]) / 2.0;                    
        }            
      }                      
    } 
    pix = pix + 1;    
  }    
}

//------------------------------------------------------------------------------

void ExtractVDDParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  s16 kVdd;
  s16 vdd25;
  
  kVdd = conf->Kv_Vdd;  // eeData->[51] & 0xFF00;
  kVdd = kVdd * 32;
  if (kVdd == 0) kVdd = -3168;
  
  vdd25 = conf->Vdd_25; // eeData[51] & 0x00FF;
  vdd25 = ((vdd25 - 256) << 5) - 8192;

  mlx90640->kVdd = kVdd;
  mlx90640->vdd25 = vdd25; 
}

//------------------------------------------------------------------------------

void ExtractPTATParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  float KvPTAT;
  float KtPTAT;
  float alphaPTAT;
  
  KvPTAT = (float)conf->Kv_Kt_ptat.data6 / 4096.0;
  
  KtPTAT = (float)conf->Kv_Kt_ptat.data10 / 8.0;
  
  alphaPTAT = conf->scale_OCC.data4 / 4 + 8.0f;
  
  mlx90640->KvPTAT = KvPTAT;
  mlx90640->KtPTAT = KtPTAT;    
  mlx90640->vPTAT25 = conf->ptat_25;
  mlx90640->alphaPTAT = alphaPTAT;   
}

//------------------------------------------------------------------------------

void ExtractGainParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  mlx90640->gainEE = conf->gain;    
}

//------------------------------------------------------------------------------

void ExtractTgcParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  float tgc = conf->TGC / 32.0;
  
  mlx90640->tgc = tgc;        
}

//------------------------------------------------------------------------------

void ExtractResolutionParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  mlx90640->resolutionEE = conf->Kv_Kta_scale.data2_1;  // eeData[56] & 0x3000) >> 12;  
}

//------------------------------------------------------------------------------

void ExtractKsTaParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  mlx90640->KsTa = (float)conf->KsTa / 8192.0;  
}

//------------------------------------------------------------------------------

void ExtractKsToParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  int KsToScale;
  s8 step;
  
  step = conf->CT_4_3.data2_1 * 10; // ( (eeData[63] & 0x3000) >> 12) * 10;
  
  mlx90640->ct[0] = -40;  // corner temperatures
  mlx90640->ct[1] = 0;
  mlx90640->ct[2] = conf->CT_4_3.data4_2 * step; // (eeData[63] & 0x00F0) >> 4;
  mlx90640->ct[3] = mlx90640->ct[2] + conf->CT_4_3.data4_3 * step; // (eeData[63] & 0x0F00) >> 8;
  mlx90640->ct[4] = 400;
  
  KsToScale = conf->CT_4_3.data4_1 + 8; // (eeData[63] & 0x000F) + 8;
  KsToScale = 1 << KsToScale;
  
  mlx90640->ksTo[0] = (float)conf->KsTo_range_1 / KsToScale; // eeData[61] & 0x00FF;
  mlx90640->ksTo[1] = (float)conf->KsTo_range_2 / KsToScale; // (eeData[61] & 0xFF00) >> 8;
  mlx90640->ksTo[2] = (float)conf->KsTo_range_3 / KsToScale; // eeData[62] & 0x00FF;
  mlx90640->ksTo[3] = (float)conf->KsTo_range_4 / KsToScale; // (eeData[62] & 0xFF00) >> 8;
  mlx90640->ksTo[4] = -0.0002;
}

//------------------------------------------------------------------------------

void ExtractAlphaParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  s32 accRow[24];
  s32 accColumn[32];
  s32 alphaRef;
  u8 alphaScale;
  u8 accRowScale;
  u8 accColumnScale;
  u8 accRemScale;
  s32 alpha;
  float alphacomp;
  float max;
  
  accRemScale = conf->scale_ACC.data1;      // eeData[32] & 0x000F;
  accColumnScale = conf->scale_ACC.data2;   // (eeData[32] & 0x00F0) >> 4;
  accRowScale = conf->scale_ACC.data3;      // (eeData[32] & 0x0F00) >> 8;
  alphaScale = conf->scale_ACC.data4 + 30;  // ((eeData[32] & 0xF000) >> 12) + 30;
  alphaRef = conf->pix_alpha_avg;           // eeData[33];
  
  for (int i = 0, p = 0; i < 6; i++)
  {
    accRow[p++] = conf->ACC_row[i].data1; // (eeData[34 + i] & 0x000F);
    accRow[p++] = conf->ACC_row[i].data2; // (eeData[34 + i] & 0x00F0) >> 4;
    accRow[p++] = conf->ACC_row[i].data3; // (eeData[34 + i] & 0x0F00) >> 8;
    accRow[p++] = conf->ACC_row[i].data4; // (eeData[34 + i] & 0xF000) >> 12;
  }
  
  for (int i = 0; i < 24; i++)
  {
    if (accRow[i] > 7)
    {
      accRow[i] = accRow[i] - 16; // -8...+7
    }
  }
  
  for (int i = 0, p = 0; i < 8; i++)
  {
    accColumn[p++] = conf->ACC_column[i].data1; // (eeData[40 + i] & 0x000F);
    accColumn[p++] = conf->ACC_column[i].data2; // (eeData[40 + i] & 0x00F0) >> 4;
    accColumn[p++] = conf->ACC_column[i].data3; // (eeData[40 + i] & 0x0F00) >> 8;
    accColumn[p++] = conf->ACC_column[i].data4; // (eeData[40 + i] & 0xF000) >> 12;
  }
  
  for (int i = 0; i < 32; i ++)
  {
    if (accColumn[i] > 7)
    {
      accColumn[i] = accColumn[i] - 16;
    }
  }

  // use g.temp to temporary save alpha array
  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 32; j ++)
    {
      int p = 32 * i + j;
      alpha = conf->calib[p].alpha; //(eeData[64 + p] & 0x03F0) >> 4;
      alpha = alpha << accRemScale;
      s32 row_add = accRow[i] << accRowScale;
      s32 col_add = accColumn[j] << accColumnScale;
      alpha = (alphaRef + row_add + col_add + alpha);
      alphacomp = (double)alpha / pow (2, (double)alphaScale);  
      alphacomp = alphacomp - mlx90640->tgc * (mlx90640->cpAlpha[0] + mlx90640->cpAlpha[1]) / 2.0;
      alphacomp = SCALEALPHA / alphacomp;
      g.temp[p] = alphacomp;
      if (p == 0)
      {
        max = alpha;
      }
      else if (alpha > max)
      {
        max = alpha; // max value
      }
    }
  }
  
  alphaScale = 0;
  while (max < 32767.4)
  {
    max *= 2.0;
    alphaScale++;
  } 
  
  for (int i = 0; i < PIXEL_COUNT; i++)
  {
    alphacomp = g.temp[i] * (float)(1 << alphaScale); 
    alphacomp += 0.5;
    u32 *p_raw = (u32 *)&alphacomp;
    FLASH_ProgramWord ((u32)&mlx90640->const_data->alpha[i], *p_raw);// need to write to flash
  }
  
  FLASH_ProgramHalfWord ((u32)&mlx90640->const_data->alphaScale, (u16)alphaScale);// need to write to flash
}

//------------------------------------------------------------------------------

void ExtractOffsetParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  s16 occRow[24];
  s16 occColumn[32];
  s16 offsetAwg;
  u8 occRowScale;
  u8 occColumnScale;
  u8 occRemScale;
  s16 offset;
  
  occRemScale = conf->scale_OCC.data1;    // (eeData[16] & 0x000F);
  occColumnScale = conf->scale_OCC.data2; // (eeData[16] & 0x00F0) >> 4;
  occRowScale = conf->scale_OCC.data3;    // (eeData[16] & 0x0F00) >> 8;
  offsetAwg = conf->pix_offset_avg;       // eeData[17];
  
  for (int i = 0, p = 0; i < 6; i++)
  {
    occRow[p++] = conf->OCC_row[i].data1; // (eeData[18 + i] & 0x000F);
    occRow[p++] = conf->OCC_row[i].data2; // (eeData[18 + i] & 0x00F0) >> 4;
    occRow[p++] = conf->OCC_row[i].data3; // (eeData[18 + i] & 0x0F00) >> 8;
    occRow[p++] = conf->OCC_row[i].data4; // (eeData[18 + i] & 0xF000) >> 12;
  }
  
  for (int i = 0; i < 24; i++)
  {
    if (occRow[i] > 7)
    {
      occRow[i] = occRow[i] - 16;
    }
  }
  
  for (int i = 0, p = 0; i < 8; i++)
  {
    occColumn[p++] = conf->OCC_column[i].data1; // (eeData[24 + i] & 0x000F);
    occColumn[p++] = conf->OCC_column[i].data2; // (eeData[24 + i] & 0x00F0) >> 4;
    occColumn[p++] = conf->OCC_column[i].data3; // (eeData[24 + i] & 0x0F00) >> 8;
    occColumn[p++] = conf->OCC_column[i].data4; // (eeData[24 + i] & 0xF000) >> 12;
  }
  
  for (int i = 0; i < 32; i ++)
  {
    if (occColumn[i] > 7)
    {
      occColumn[i] = occColumn[i] - 16;
    }
  }
  
  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 32; j ++)
    {
      int p = 32 * i + j;
      offset = conf->calib[p].offset; // (eeData[64 + p] & 0xFC00) >> 10;
      offset = offset << occRemScale;
      offset = (offsetAwg + (occRow[i] << occRowScale) + (occColumn[j] << occColumnScale) + offset);
      FLASH_ProgramHalfWord ((u32)&mlx90640->const_data->offset[p], (u16)offset);// need to write to flash
    }
  }  
}

//------------------------------------------------------------------------------

void ExtractKtaPixelParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  s8 KtaRC[4];
  u8 ktaScale;
  u8 ktaScale1;
  u8 ktaScale2;
  u8 split;
  float Kta;
  float Kta_abs;
  float max;
  
  KtaRC[0] = conf->Kta_Ro_Co; // (eeData[54] & 0xFF00) >> 8; 
  KtaRC[2] = conf->Kta_Re_Co; // (eeData[54] & 0x00FF);
  KtaRC[1] = conf->Kta_Ro_Ce; // (eeData[55] & 0xFF00) >> 8;
  KtaRC[3] = conf->Kta_Re_Ce; // (eeData[55] & 0x00FF);
  
  ktaScale1 = conf->Kv_Kta_scale.data4_2 + 8; // ((eeData[56] & 0x00F0) >> 4) + 8;
  ktaScale2 = conf->Kv_Kta_scale.data4_1;     // (eeData[56] & 0x000F);
  
  // use g.temp for temporary values
  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 32; j ++)
    {
      int p = 32 * i + j;
      split = 2 * (p / 32 - (p / 64) * 2) + p % 2;
      Kta = conf->calib[p].kta; // (eeData[64 + p] & 0x000E) >> 1;
      if (Kta > 3)
      {
        Kta -= 8.0;
      }
      Kta *= (1 << ktaScale2);
      Kta += KtaRC[split];
      Kta /= (1 << ktaScale1);
      g.temp[p] = Kta;
      Kta_abs = fabs (Kta);
      if (p == 0)
      {
        max = Kta_abs;
      }
      else if (Kta_abs > max)
      {
        max = Kta_abs;
      }
    }
  }
      
  ktaScale = 0;
  while (max < 63.4)
  {
    max *= 2.0;
    ktaScale++;
  }    
   
  for (int i = 0; i < PIXEL_COUNT; i++)
  {
    Kta = g.temp[i] * (1 << ktaScale);
    if (Kta < 0)
    {
      Kta -= 0.5f;
    }
    else
    {
      Kta += 0.5f;
    }        
    u32 *p_raw = (u32 *)&Kta;
    FLASH_ProgramWord ((u32)&mlx90640->const_data->kta[i], *p_raw); // need to write to flash 
  } 
  
  FLASH_ProgramHalfWord ((u32)&mlx90640->const_data->ktaScale, (u16)ktaScale);// need to write to flash  
}

//------------------------------------------------------------------------------

void ExtractKvPixelParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  int p = 0;
  s8 KvT[4];
  s8 KvRoCo;
  s8 KvRoCe;
  s8 KvReCo;
  s8 KvReCe;
  u8 kvScale;
  u8 split;
  float Kv;
  float Kv_abs;
  float max;
  
  KvRoCo = conf->Kv_avg.data4;  // (eeData[52] & 0xF000) >> 12;
  if (KvRoCo > 7)
  {
    KvRoCo = KvRoCo - 16;
  }
  KvT[0] = KvRoCo;
  
  KvReCo = conf->Kv_avg.data3;  // (eeData[52] & 0x0F00) >> 8;
  if (KvReCo > 7)
  {
    KvReCo = KvReCo - 16;
  }
  KvT[2] = KvReCo;
  
  KvRoCe = conf->Kv_avg.data2;  // (eeData[52] & 0x00F0) >> 4;
  if (KvRoCe > 7)
  {
    KvRoCe = KvRoCe - 16;
  }
  KvT[1] = KvRoCe;
  
  KvReCe = conf->Kv_avg.data1;  // (eeData[52] & 0x000F);
  if (KvReCe > 7)
  {
    KvReCe = KvReCe - 16;
  }
  KvT[3] = KvReCe;
  
  kvScale = conf->Kv_Kta_scale.data4_3; // (eeData[56] & 0x0F00) >> 8;
  
  // use g.temp for temporary store values
  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 32; j ++)
    {
      p = 32 * i +j;
      split = 2 * (p / 32 - (p / 64) * 2) + p % 2;
      Kv = KvT[split];
      Kv /= (1 << kvScale);
      g.temp[p] = Kv;
      Kv_abs = fabs (Kv);
      if (p == 0)
      {
        max = Kv_abs;
      }
      else if (Kv_abs > max)
      {
        max = Kv_abs;
      }
    }
  }
  
  kvScale = 0;
  if (max == 0.0) max = 1.0;
  while (max < 63.4)
  {
    max *= 2;
    kvScale++;
  }    
   
  for(int i = 0; i < PIXEL_COUNT; i++)
  {
    Kv = g.temp[i] * ( 1 << kvScale);
    if (Kv < 0)
    {
      Kv -= 0.5f;
    }
    else
    {
      Kv += 0.5f;
    }        
        
    u32 *p_raw = (u32 *)&Kv;
    FLASH_ProgramWord ((u32)&mlx90640->const_data->kv[i], *p_raw);// need to write to flash
  } 
  FLASH_ProgramHalfWord ((u32)&mlx90640->const_data->kvScale, (u16)kvScale);// need to write to flash        
}

//------------------------------------------------------------------------------

void ExtractCPParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  float alphaSP[2];
  s16 offsetSP[2];
  float cpKv;
  float cpKta;
  u16 alphaScale;
  u16 ktaScale;
  u16 kvScale;
  
  alphaScale = conf->scale_ACC.data4 + 27; // ((eeData[32] & 0xF000) >> 12) + 27;
  
  offsetSP[0] = conf->Off_CP_1_2.data10;  //(eeData[58] & 0x03FF);
  offsetSP[1] = offsetSP[0] + conf->Off_CP_1_2.data6; 
  
  alphaSP[0] = conf->Alfa_CP_1_2.data10;
  alphaSP[0] = (double)alphaSP[0] /  pow (2, (double)alphaScale);
  alphaSP[1] = conf->Alfa_CP_1_2.data6;
  alphaSP[1] = (1 + alphaSP[1] / 128) * alphaSP[0];
  
  cpKta = conf->Kta_CP; // (eeData[59] & 0x00FF);

  ktaScale = conf->Kv_Kta_scale.data4_2 + 8; // ((eeData[56] & 0x00F0) >> 4) + 8;    
  mlx90640->cpKta = cpKta / pow (2, (double)ktaScale);
  
  cpKv = conf->Kv_CP;
  kvScale = conf->Kv_Kta_scale.data4_3; // (eeData[56] & 0x0F00) >> 8;
  mlx90640->cpKv = cpKv / pow (2, (double)kvScale);
  
  mlx90640->cpAlpha[0] = alphaSP[0];
  mlx90640->cpAlpha[1] = alphaSP[1];
  mlx90640->cpOffset[0] = offsetSP[0];
  mlx90640->cpOffset[1] = offsetSP[1];  
}

//------------------------------------------------------------------------------

void ExtractCILCParameters (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  float ilChessC[3];
  
  ilChessC[0] = conf->Chess_1_3.data6;
  if (ilChessC[0] > 31)
  {
    ilChessC[0] = ilChessC[0] - 64;
  }
  ilChessC[0] = ilChessC[0] / 16.0f;
  
  ilChessC[1] = conf->Chess_1_3.data5_1;
  if (ilChessC[1] > 15)
  {
    ilChessC[1] = ilChessC[1] - 32;
  }
  ilChessC[1] = ilChessC[1] / 2.0f;
  
  ilChessC[2] = conf->Chess_1_3.data5_2;
  if (ilChessC[2] > 15)
  {
    ilChessC[2] = ilChessC[2] - 32;
  }
  ilChessC[2] = ilChessC[2] / 8.0f;
  
  mlx90640->calibrationModeEE = conf->options.calib_mode ^ 0x01;
  mlx90640->ilChessC[0] = ilChessC[0];
  mlx90640->ilChessC[1] = ilChessC[1];
  mlx90640->ilChessC[2] = ilChessC[2];
}

//------------------------------------------------------------------------------

int ExtractDeviatingPixels (MLX90640_conf *conf, paramsMLX90640 *mlx90640)
{
  u16 pixCnt = 0;
  u16 brokenPixCnt = 0;
  u16 outlierPixCnt = 0;
  int warn = 0;
  int i;
  
  for (pixCnt = 0; pixCnt < 5; pixCnt++)
  {
    mlx90640->brokenPixels[pixCnt] = 0xFFFF;
    mlx90640->outlierPixels[pixCnt] = 0xFFFF;
  }
  
  pixCnt = 0;    
  while (pixCnt < PIXEL_COUNT && brokenPixCnt < 5 && outlierPixCnt < 5)
  {
    if ((conf->calib[pixCnt].alpha == 0)
        && (conf->calib[pixCnt].kta == 0)
        && (conf->calib[pixCnt].offset == 0))
    {
      mlx90640->brokenPixels[brokenPixCnt] = pixCnt;
      brokenPixCnt = brokenPixCnt + 1;
    }    
    else if ((conf->calib[pixCnt].outlier) != 0)
    {
      mlx90640->outlierPixels[outlierPixCnt] = pixCnt;
      outlierPixCnt = outlierPixCnt + 1;
    }    
    
    pixCnt = pixCnt + 1; 
  } 
  
  if (brokenPixCnt > 4)  
  {
    warn = -3;
  }         
  else if (outlierPixCnt > 4)  
  {
    warn = -4;
  }
  else if ( (brokenPixCnt + outlierPixCnt) > 4)  
  {
    warn = -5;
  } 
  else
  {
    for (pixCnt = 0; pixCnt < brokenPixCnt; pixCnt++)
    {
      for (i = pixCnt + 1; i < brokenPixCnt; i++)
      {
        warn = CheckAdjacentPixels (mlx90640->brokenPixels[pixCnt] ,mlx90640->brokenPixels[i]);
        if (warn) return warn; 
      }    
    }
    
    for (pixCnt = 0; pixCnt < outlierPixCnt; pixCnt++)
    {
      for (i = pixCnt + 1; i < outlierPixCnt; i++)
      {
        warn = CheckAdjacentPixels (mlx90640->outlierPixels[pixCnt], mlx90640->outlierPixels[i]);
        if (warn) return warn;   
      }    
    } 
    
    for (pixCnt = 0; pixCnt < brokenPixCnt; pixCnt++)
    {
      for (i = 0; i < outlierPixCnt; i++)
      {
        warn = CheckAdjacentPixels (mlx90640->brokenPixels[pixCnt], mlx90640->outlierPixels[i]);
        if (warn) return warn; 
      }    
    }    
  }    
  return warn; 
}

//------------------------------------------------------------------------------

int CheckAdjacentPixels (u16 pix1, u16 pix2)
{
  int pixPosDif;
  
  pixPosDif = pix1 - pix2;
  if (pixPosDif > -34 && pixPosDif < -30)
  {
    return -6;
  } 
  if (pixPosDif > -2 && pixPosDif < 2)
  {
    return -6;
  } 
  if (pixPosDif > 30 && pixPosDif < 34)
  {
    return -6;
  }
  
  return 0;    
}

//------------------------------------------------------------------------------

int CheckEEPROMValid (u16 *eeData)  
{
  int deviceSelect;
  deviceSelect = eeData[10] & 0x0040;
  if (deviceSelect == 0)
  {
    return 0;
  }
  
  return -7;    
} 

//------------------------------------------------------------------------------

float GetMedian (float *values, int n)
{
  float temp;
  
  for (int i = 0; i < n - 1; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      if (values[j] < values[i]) 
      {                
        temp = values[i];
        values[i] = values[j];
        values[j] = temp;
      }
    }
  }
  
  if (n % 2==0) 
  {
    return ( (values[n / 2] + values[n / 2 - 1]) / 2.0);
  } 
  else 
  {
    return values[n / 2];
  }
}           

//------------------------------------------------------------------------------

int IsPixelBad (u16 pixel, paramsMLX90640 *params)
{
  for (int i = 0; i < 5; i++)
  {
    if (pixel == params->outlierPixels[i] || pixel == params->brokenPixels[i])
    {
      return 1;
    }    
  }   
  return 0;     
}     




bool compare_data (MLX90640_conf *conf, const_data_MLX90640 const *MLXdata)
{
  bool result = true;
  if (conf->id1 != MLXdata->dev_id1)
  {
    result = false;
  }
  if (conf->id2 != MLXdata->dev_id2)
  {
    result = false;
  }
  if (conf->id3 != MLXdata->dev_id3)
  {
    result = false;
  }
  return result;
}



u8 FLASH_erase_MLX90640_pages (void)
{
  FLASH_Status status;
  const u32 start_address     = 0x0801D000; // start page
  const u32 end_address = 0x08020000; // end of flash
  const u32 page_size   = 0x400;
  
  for (int addr = start_address; addr < end_address; addr += page_size)
  {
    status = FLASH_ErasePage (addr);
  }
  return (u8)status;
}

