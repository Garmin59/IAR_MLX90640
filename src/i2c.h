//----------------------------------------------------------------------------
// Файл i2c.h
//----------------------------------------------------------------------------

#ifndef	_I2C_H_
#define	_I2C_H_

typedef enum
{
  I2C_STOP = 0,     // I2C ready
  I2C_MW_START,     // start write sequence
  I2C_MW_ADDRESS,   // send i2c address
  I2C_MW_DATA,      // send write data
  I2C_MW_STOP,      // send write stop
  I2C_MR_START,
  I2C_MR_ADDRESS,
  I2C_MR_DATA,
  I2C_MR_STOP,
  I2C_MMR_START,
  I2C_MMR_ADDRESS,
  I2C_MMR_REGISTER,
  I2C_MMR_RESTART,
  I2C_MMR_READDRESS,
  I2C_MMR_READ,
  I2C_MMR_STOP
} i2c_status;

typedef struct
{
  u8          addr;   // i2c address (8bit)
  u16         reg;    // register address 
  u8          *rdata; // read data pointer
  u8          *wdata  // write data pointer
  u32         rlen;   // read data length
  u32         wlen;   // write data length
  i2c_status  status; // FSM i2c status
  u16         error;  // error register value
} i2c_control;

ErrorStatus  i2c_init (void);
void         i2c_func (void);
void         i2c_task (void *param);

ErrorStatus  i2c_write   (u8 addr, u8 *cmd, u16 len, u8 cont);
ErrorStatus  i2c_read    (u8 addr, u8 *data, u16 len);
ErrorStatus  i2c_read16  (u8 addr, u16 *data, u16 len);

u8          FLASH_erase_MLX90640_pages  (void);



#endif  // _I2C_H_
