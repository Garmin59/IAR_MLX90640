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
#ifndef _MLX90640_API_H_
#define _MLX90640_API_H_

#define MLX90640_ADDR               (0x66)  // default I2C address

#define MLX_REG_STATUS              (0x8000)  // Status register address
#define MLX_REG_STATUS_SUBPAGE      (0x0007)  // mask subpage bit
#define MLX_REG_STATUS_SUBPAGE_0    (0x0000)  // subpage 0
#define MLX_REG_STATUS_SUBPAGE_1    (0x0001)  // subpage 1
#define MLX_REG_STATUS_NEW_DATA     (0x0008)  // new data available
#define MLX_REG_STATUS_OVW          (0x0010)  // Data in RAM overwrite
#define MLX_REG_STATUS_OVW_EN       (0x0010)  // Data in RAM overwrite is enabled
#define MLX_REG_STATUS_OVW_DIS      (0x0000)  // Data in RAM overwrite is disabled
#define MLX_REG_STATUS_START_CONV   (0x0020)  // Start of conversion in step mode
#define MLX_REG_STATUS_ROM_BIST_RUN (0x0040)  // rom_bist_running
#define MLX_REG_STATUS_RAM_BIST_RUN (0x0080)  // ram_bist_running
#define MLX_REG_STATUS_RAM_BIST_FAIL (0x0100)  // ram_bist_failed

#define MLX_REG_CR1                 (0x800D) // Control register1 address
#define MLX_REG_CR1_EN_SUBP_MODE    (0x0001) // Subpade mode is activated (default)
#define MLX_REG_CR1_EN_STEP_MODE    (0x0002) // Enables the step mode. In this mode the STM will do one measurement and will wait for next command
#define MLX_REG_CR1_DATA_HOLD       (0x0004) // Transfer the data into storage RAM only if en_overwrite
#define MLX_REG_CR1_DATA_ENABLE     (0x0000) // Transfer the data into storage RAM at each measured frame (default)
#define MLX_REG_CR1_SUBP_REPEAT_OFF (0x0008) // Select subpage determines which subpage to be measured if Enable subpages mode = "1"
#define MLX_REG_CR1_SUBP_REPEAT_ON  (0x0000) // Toggles between subpage "0" and subpage "1" if Enable subpages mode = "1" (default)
#define MLX_REG_CR1_SEL_SUBPAGE     (0x0070) // Subpage bits
#define MLX_REG_CR1_SEL_SUBPAGE_0   (0x0000) // Subpage 0
#define MLX_REG_CR1_SEL_SUBPAGE_1   (0x0010) // Subpage 1
#define MLX_REG_CR1_REF_RATE        (0x0380) // Refresh rate control bits
#define MLX_REG_CR1_REF_RATE_05     (0x0000) // Refresh rate = 0.5 Hz
#define MLX_REG_CR1_REF_RATE_1      (0x0080) // Refresh rate = 1 Hz
#define MLX_REG_CR1_REF_RATE_2      (0x0100) // Refresh rate = 2 Hz (default)
#define MLX_REG_CR1_REF_RATE_4      (0x0180) // Refresh rate = 4 Hz
#define MLX_REG_CR1_REF_RATE_8      (0x0200) // Refresh rate = 8 Hz
#define MLX_REG_CR1_REF_RATE_16     (0x0280) // Refresh rate = 16 Hz
#define MLX_REG_CR1_REF_RATE_32     (0x0300) // Refresh rate = 32 Hz
#define MLX_REG_CR1_REF_RATE_64     (0x0380) // Refresh rate = 64 Hz
#define MLX_REG_CR1_RESOLUTION      (0x0C00) // Resolution control bits
#define MLX_REG_CR1_RESOLUTION_16   (0x0000) // ADC set to 16 bit resolution
#define MLX_REG_CR1_RESOLUTION_17   (0x0400) // ADC set to 17 bit resolution
#define MLX_REG_CR1_RESOLUTION_18   (0x0800) // ADC set to 18 bit resolution (default)
#define MLX_REG_CR1_RESOLUTION_19   (0x0C00) // ADC set to 19 bit resolution
#define MLX_REG_CR1_PATTERN         (0x1000) // Reading pattern
#define MLX_REG_CR1_INTERLEAVED     (0x0000) // Interleaved (TV) mode
#define MLX_REG_CR1_CHESS           (0x1000) // Chess pattern (default)

#define MLX_CR1_RESOLUTION_16_BIT   (u16)(0) // ADC set to 16 bit resolution
#define MLX_CR1_RESOLUTION_17_BIT   (u16)(1) // ADC set to 17 bit resolution
#define MLX_CR1_RESOLUTION_18_BIT   (u16)(2) // ADC set to 18 bit resolution (default)
#define MLX_CR1_RESOLUTION_19_BIT   (u16)(3) // ADC set to 19 bit resolution
#define MLX_CR1_REF_RATE_05_HZ      (u16)(0) // Refresh rate = 0.5 Hz
#define MLX_CR1_REF_RATE_1_HZ       (u16)(1) // Refresh rate = 1 Hz
#define MLX_CR1_REF_RATE_2_HZ       (u16)(2) // Refresh rate = 2 Hz (default)
#define MLX_CR1_REF_RATE_4_HZ       (u16)(3) // Refresh rate = 4 Hz
#define MLX_CR1_REF_RATE_8_HZ       (u16)(4) // Refresh rate = 8 Hz
#define MLX_CR1_REF_RATE_16_HZ      (u16)(5) // Refresh rate = 16 Hz
#define MLX_CR1_REF_RATE_32_HZ      (u16)(6) // Refresh rate = 32 Hz
#define MLX_CR1_REF_RATE_64_HZ      (u16)(7) // Refresh rate = 64 Hz

   
   
#define MLX_REG_CR2                 (0x800E) // Control register1 address


#define MLX_REG_CONF                (0x800F) // I2C configuration register (0x800F)
#define MLX_REG_CONF_FM_ENABLE      (0x0000) // FM+ mode enabled (default)
#define MLX_REG_CONF_FM_DISABLE     (0x0001) // FM+ mode disabled
#define MLX_REG_CONF_VDD_REF        (0x0000) // VDD reffered threshold (normal mode) (default)
#define MLX_REG_CONF_VDD_1V8        (0x0002) // 1.8V reffered threshold (1.8V mode)
#define MLX_REG_CONF_SDA_CURR_ON    (0x0000) // SDA driver current limit is ON (default)
#define MLX_REG_CONF_SDA_CURR_OFF   (0x0004) // SDA driver current limit is OFF

#define MLX_REG_I2C_ADDR            (0x8010) // Melexis internal use (8 bit) I2C_Address (8bit)

#define MLX_RAM_START               (0x0400) // Start address of RAM
#define MLX_RAM_SIZE                (0x0300) // Size RAM

#define MLX_RAM_TA_VBE_ADDR         (0x0700) // 0x0700=Ta_Vbe
#define MLX_RAM_SP0_ADDR            (0x0708) // 0x0708=CP(SP 0)
#define MLX_RAM_GAIN_ADDR           (0x070A) // 0x070A=GAIN
#define MLX_RAM_TA_PTAT_ADDR        (0x0720) // 0x0720=Ta_PTAT
#define MLX_RAM_SP1_ADDR            (0x0728) // 0x0728=CP(SP1)
#define MLX_RAM_VDDPIX_ADDR         (0x072A) // 0x072A=VDDpix

#define SCALEALPHA      (0.000001)

#define ROM_START       (0x0000)
#define ROM_SIZE        (0x0400)
#define RAM_START       (0x0400)
#define RAM_SIZE        (0x0340)
#define EEPROM_START    (0x2400)
#define EEPROM_SIZE     (0x0340)
#define EE_PARAM_START  (0x2400)
#define EE_PARAM_SIZE   (0x0040)
#define EE_PIXEL_START  (0x2440)
#define EE_PIXEL_SIZE   (0x0300)

#define PIXEL_COUNT     (768)


typedef struct
{
  u16 subpage       :3; // subpage number
  u16 new_data      :1; // 1 - new data available in RAM
  u16 en_overwrite  :1; // 1- data in RAM overwrite enabled
  u16 start_conv    :1; // 1- start of conversions
  u16 rom_bist_run  :1; // 
  u16 ram_bist_run  :1; // 
  u16 ram_bist_fail :1; // 
} status_reg;

typedef struct
{
  u16 en_subpage      :1; // 1 - in subpage mode. 0 - no subpages, measure only one page.
  u16 en_step_mode    :1; // 1 - Enables the step mode. In this mode the STM will do one measurement and will wait for next command.
  u16 en_data_hold    :1; // 1 - the state machine will transfer the data to storage RAM only if en_overwrite = 1
  u16 en_subp_repeat  :1; // 1 - the state machine will measure only the sub-page 
                          // seleected by sel_sub_page<2:0>. The bit is active only if en_sub_page = 1.
  u16 subpage_num     :3; // Address of sub_page for continuous measurement. 
                          // If the state machine is configured to measure only one sub_page 
                          // (en_subp_repeat = 1) then these bits must give the address of sub_page to be measured
  u16 refresh_rate    :3; // Refresh rate control: 0-0.5Hz, 1 -	1Hz, 2-2Hz, 3-4Hz, 4-8Hz, 5-16Hz, 6-32Hz, 7-64Hz.
  u16 resolution      :2; // Resolution Control: 0-16bit, 1-17bit, 2-18bit, 3-19bit
  u16 chess           :1; // 1 - Chess pattern, 0 - IL (TV) mode
  u16 bit_13          :1; // reserved
  u16 bit_14          :1; // reserved
  u16 bit_15          :1; // reserved
} control_reg;

typedef struct
{
  u16 vdd         :2; // 0-3.0V, 1-3.3V, 2-3.6V, 3-na
  u16 aperture    :2; // 0-opt, 1-40град, 2-55, 3-110
  u16 filter      :2; // 0-LWP, 1-no filter, 2-5.5-14um, 3-8.0-14um
  u16 device      :1; // 0-90640, 1-90641
  u16 max_t       :2; // 0-optional, 1-85C, 2-125C, 3-150C
  u16 gaz_fill    :2; // 0-no gaz, 1-Nitrogen, 2-Xenon, 3-opt
  u16 calib_mode  :1; // 0-chess calib mode, 1-interleaved calib mode
  u16 calib_seq   :4; // number of calibration sequence 0-15
} options_reg;

typedef struct
{
  u16 outlier :1;
  s16 kta     :3;
  s16 alpha   :6;
  s16 offset  :6;
} mlx_data_1_3_6_6;

typedef struct
{
  u16 data4_1 :4;
  u16 data4_2 :4;
  u16 data4_3 :4;
  u16 data2_1 :2;
  u16 data2_2 :2;
} mlx_data_4_4_4_2_2;

typedef struct
{
  u16 data6   :6;
  u16 data5_1 :5;
  u16 data5_2 :5;
} mlx_data_6_5_5;


typedef struct
{
  u16 data1  :4;
  u16 data2  :4;
  u16 data3  :4;
  u16 data4  :4;    
} mlx_data_4x4;

typedef struct
{
  s16 data10 :10;
  s16 data6  :6;   
} mlx_data_6_10;



typedef struct
{
  u16                 Osc_trim;       // Melexis reserved
  u16                 Ana_trim;       // Melexis reserved
  u16                 res3;           // Melexis reserved
  u16                 conf;           // Configuration register
  u16                 res5;           // After "tm_unlock" key I2C Address is fixed at 0x33. It is possible to use another one without "tm_unlock" command.
  u16                 crop_page_addr; // default code for all array -> 0x0320 
  u16                 crop_cell_addr; // default code for all array -> 0x03E0
  u16                 id1;            // Device ID1
  u16                 id2;            // Device ID2
  u16                 id3;            // Device ID3
  options_reg         options;        // Device Options
  u16                 res7;           // Melexis reserved
  u16                 cr1;            // Control register_1
  u16                 cr2;            // Control register_2
  u16                 i2c_conf;       // I2CConfReg
  u16                 i2c_addr;       // I2C_Address
  mlx_data_4x4        scale_OCC;      //
  s16                 pix_offset_avg;     //
  mlx_data_4x4        OCC_row[6];     // OCC_row_01…24 (6 x 4 x 3bit+sign)
  mlx_data_4x4        OCC_column[8];  // OCC_column_01…32 (8 x 4 x 3bit+sign)
  mlx_data_4x4        scale_ACC;
  s16                 pix_alpha_avg;
  mlx_data_4x4        ACC_row[6];     // ACC_row_01…24 (6 x 4 x 3bit+sign)
  mlx_data_4x4        ACC_column[8];  // ACC_column_01…32 (8 x 4 x 3bit+sign)
  s16                 gain;
  s16                 ptat_25;
  mlx_data_6_10       Kv_Kt_ptat;
  s8                  Vdd_25;
  s8                  Kv_Vdd;
  mlx_data_4x4        Kv_avg;
  mlx_data_6_5_5      Chess_1_3;
  s8                  Kta_Re_Co;
  s8                  Kta_Ro_Co;
  s8                  Kta_Re_Ce;
  s8                  Kta_Ro_Ce;
  mlx_data_4_4_4_2_2  Kv_Kta_scale;
  mlx_data_6_10       Alfa_CP_1_2;
  mlx_data_6_10       Off_CP_1_2;
  s8                  Kta_CP;
  s8                  Kv_CP;
  s8                  TGC;
  s8                  KsTa;
  s8                  KsTo_range_1;
  s8                  KsTo_range_2;
  s8                  KsTo_range_3;
  s8                  KsTo_range_4;
  mlx_data_4_4_4_2_2  CT_4_3;
  mlx_data_1_3_6_6    calib[PIXEL_COUNT];
} MLX90640_conf;


typedef struct
{
  u16     dev_id1;
  u16     dev_id2;
  u16     dev_id3;
  float   alpha[768];    
  s16     offset[768];    
  float   kta[768];    
  float   kv[768];
  u16     alphaScale;
  u16     ktaScale;
  u16     kvScale;
} const_data_MLX90640;

   
typedef struct
{
  s16     kVdd;
  s16     vdd25;
  float   KvPTAT;
  float   KtPTAT;
  s16     vPTAT25;
  float   alphaPTAT;
  s16     gainEE;
  float   tgc;
  float   cpKv;
  float   cpKta;
  u8      resolutionEE;
  u8      calibrationModeEE; // 0-chess, 1 -onterleaved
  float   KsTa;
  float   ksTo[5];
  s16     ct[5];
  const const_data_MLX90640 *const_data;
  float   cpAlpha[2];
  s16     cpOffset[2];
  float   ilChessC[3]; 
  u16     brokenPixels[5];
  u16     outlierPixels[5]; 
  float   emissivity;
} paramsMLX90640;

typedef struct
{
  s16 pixel_data[PIXEL_COUNT];
  s16 Ta_Vbe; // 0x0700
  u16 res701;
  u16 res702;
  u16 res703;
  u16 res704;
  u16 res705;
  u16 res706;
  u16 res707;
  s16 CP_SP0; // 0x0708
  u16 res709;
  s16 gain;   // 0x070A
  u16 res70B;
  u16 res70C;
  u16 res70D;
  u16 res70E;
  u16 res70F;
  u16 res710;
  u16 res711;
  u16 res712;
  u16 res713;
  u16 res714;
  u16 res715;
  u16 res716;
  u16 res717;
  u16 res718;
  u16 res719;
  u16 res71A;
  u16 res71B;
  u16 res71C;
  u16 res71D;
  u16 res71E;
  u16 res71F;
  s16 Ta_PTAT;  // 0x720
  u16 res721;
  u16 res722;
  u16 res723;
  u16 res724;
  u16 res725;
  u16 res726;
  u16 res727;
  s16 CP_SP1;   // 0x728
  u16 res729;
  s16 VDD_pix;  // 0x72A
  u16 res72B;
  u16 res72C;
  u16 res72D;
  u16 res72E;
  u16 res72F;
  u16 res730;
  u16 res731;
  u16 res732;
  u16 res733;
  u16 res734;
  u16 res735;
  u16 res736;
  u16 res737;
  u16 res738;
  u16 res739;
  u16 res73A;
  u16 res73B;
  u16 res73C;
  u16 res73D;
  u16 res73E;
  u16 res73F;
  control_reg control;
  status_reg  status;
} ram_data;

extern const const_data_MLX90640 MLXdata;


int     MLX90640_DumpEE               (u8 slaveAddr, u16 *eeData);
int     MLX90640_read_ee_param        (u8 slaveAddr, u16 *eeData);
int     MLX90640_read_ee_pixels       (u8 slaveAddr, u16 *eeData);
ErrorStatus MLX90640_GetFrameData     (u8 slaveAddr, ram_data *frameData);
bool    MLX90640_GetNewDataStatus     (u8 slaveAddr); 
int     MLX90640_ExtractParameters    (u16 *eeData, paramsMLX90640 *mlx90640);
float   MLX90640_GetVdd               (ram_data *frameData, const paramsMLX90640 *params);
float   MLX90640_GetTa                (ram_data *frameData, const paramsMLX90640 *params);
void    MLX90640_GetImage             (ram_data *frameData, const paramsMLX90640 *params, u16 *result);
void    MLX90640_CalculateTo          (ram_data *frameData, const paramsMLX90640 *params, s16 *result);
int     MLX90640_SetResolution        (u8 slaveAddr, u8 resolution);
int     MLX90640_GetCurResolution     (u8 slaveAddr);
int     MLX90640_SetRefreshRate       (u8 slaveAddr, u8 refreshRate);   
int     MLX90640_GetRefreshRate       (u8 slaveAddr);  
int     MLX90640_GetSubPageNumber     (ram_data *frameData);
int     MLX90640_GetCurMode           (u8 slaveAddr); 
int     MLX90640_SetInterleavedMode   (u8 slaveAddr);
int     MLX90640_SetChessMode         (u8 slaveAddr);
void    MLX90640_BadPixelsCorrection  (u16 *pixels, float *to, int mode, paramsMLX90640 *params);
    
#endif
