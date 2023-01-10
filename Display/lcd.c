//----------------------------------------------------------------------------
// Файл lcd.c
// вывод информации на дисплей по SPI
// диспей 240х240 контроллер ST7789
//----------------------------------------------------------------------------


#include	"global.h"



__no_init lcd_param	lcd;
static lcd_status	status;

tFont const * const font_list [10] =
{
	&Agency_18,
	&Agency_29,
	&Courier_18,
	&Courier_29,
  &Arial_18,
  &Arial_29,
	&Battery_18,
	&Arial_29,
  &Arial_29,
  &Arial_29
};				// ссылки на установленные шрифты


// ST7789
const lcd_param lcd_default = 
{
	(s16) 	ST7789_OFFSET_X_ALBUM,	// сдвиг по x
	(s16) 	ST7789_OFFSET_Y_ALBUM,	// сдвиг по y
	(s16) 	ST7789_XMAX,	// ширина экрана
	(s16) 	ST7789_YMAX,	// высота экрана
	(lcd_orient)	ALBUM,	// ориентация дисплея
	(s16) 	0,					  // место вывода символа по x (координаты логические)
	(s16) 	0,					  // место вывода символа по y (координаты логические)
	(s16) 	ST7789_XMAX,	// ширина символа
	(s16) 	ST7789_YMAX,	// высота символа
  (win_modes)   LCD_SCREEN,   // режим работы дисплея (полноформатный/окно)
	(s16)         0,            // начало окна по x (координаты логические)
	(s16)         0,            // начало окна по Y (координаты логические)
	(s16)         ST7789_XMAX,	// ширина окна
	(s16)         ST7789_YMAX,	// высота окна
	(s16)		0,					  // включить / выключить курсор
	(u16)		RGB_BLACK,	  // цвет фона RGB 565		
	(u16)		RGB_WHITE,	  // цвет фигур RGB 565
	(u16)		RGB_BLACK,	  // цвет фона текста RGB 565		
	(u16)		RGB_YELLOW,	  // цвет текста RGB 565
	(s16)		0,					  // ширина границы в пикселях
	(const tFont *)	 &Arial_29,			// указатель на структуру текущего шрифта
	(const tImage *) NULL // Arial_29.chars[0].image, // указатель на выводимую картинку	
}; 				// параметры дисплея по умолчанию

static u16 lcd_default_colors [10][2] = 
{
	{RGB_BLACK,   RGB_DARKGRAY},
	{RGB_RED,     RGB_BRIGHT_RED},
	{RGB_GREEN,   RGB_BRIGHT_GREEN},
	{RGB_BROWN,   RGB_YELLOW},
	{RGB_BLUE,    RGB_BRIGHT_BLUE},
	{RGB_MAGENTA, RGB_BRIGHT_MAGENTA},
	{RGB_CYAN,    RGB_BRIGHT_CYAN},
	{RGB_GRAY,    RGB_WHITE},
	{RGB_BLACK,   RGB_WHITE},
	{RGB_BLACK,   RGB_WHITE}
};			
// первое значение - приглушённый цвет, второе - яркий (Bright)
// цвет: 0-Black	1-Red	2-Green	3-Yellow	4-Blue	5-Magenta	6-Cyan	7-White
// 8-пользовательский 9- по умолчанию


typedef struct _lcd_init_data
{
  u8  command;
  u8  data;
} lcd_init_data;

static const lcd_init_data ST7789_init[] = 
{
  {SWRESET,       0    },
  {TFTLCD_DELAY,  150  },
  {SLPOUT,        0    },
  {TFTLCD_DELAY,  150  },
  {COLMOD,        0x05 },   // RGB444(12bit) 0x03, RGB565(16bit) 0x05, RGB666(18bit) 0x06
  {MADCTL,        0x14 },   // 0x08 B-G-R, 0x14 R-G-B
  {INVON,         0    },   // ??? INVON
  {NORON,         0    },
  {DISPON,        0    },
  {TFTLCD_DELAY,  255  },
};




// определения внутренних функций
void          spi_init8         (void);
void          spi_read8         (void);
void          spi_set8          (void);
void          spi_set16         (void);
void          dma_init          (void);
void          lcd_read_param    (void);
void          lcd_read          (u8 command, u8 *result, u8 n) ;
inline void   lcd_reset         (void);
inline void   lcd_send8         (u8 data);
inline void   lcd_send16        (u16 data);
inline void		lcd_command				(u8 command);
inline void		lcd_data  			  (u8 data);
inline void   lcd_data16        (u16 data);
inline void   wait_spi_buffer   (void);
inline void   wait_spi_finish   (void);
inline void 	lcd_start_data	  (void);
inline void 	lcd_stop_data			(void);
tChar *			  lcd_find_char_by_code	(u8 code, const tFont *font);
void		      lcd_parser				(const char *str);
inline u8	    atou8					    (char *str);





void lcd_read_param (void)
{
	lcd_read (RDDID, status.id, 3);
	lcd_read (RDDST, status.st, 4);
	lcd_read (RDDPM, &status.pm, 1);
	lcd_read (RDD_MADCTL, &status.madctr, 1);
	lcd_read (RDD_COLMOD, &status.colmod, 1);
	lcd_read (RDDIM, &status.im, 1);
	lcd_read (RDDSM, &status.sm, 1);
	lcd_read (RDDSDR, &status.sfr, 1);
}


/*******************************************************************************
* init SPI1 to send 8 bit data
*******************************************************************************/
void spi_init8 (void)
{
  // SPI disable
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/2
            | SPI_CR1_SPE      * 0  // SPI Enable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 0  // Data Frame Format 0: 8-bit format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // output mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode disable
  SPI1->I2SCFGR = 0;          // SPI mode selection
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 0  // SPI Enable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 0  // Data Frame Format 0: 8-bit format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // output mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode disable
  SPI1->CR2 = SPI_CR2_RXDMAEN  * 0  // Rx Buffer DMA Enable
            | SPI_CR2_TXDMAEN  * 0  // Tx Buffer DMA Enable
            | SPI_CR2_SSOE     * 0  // SS Output Enable
            | SPI_CR2_ERRIE    * 0  // Error Interrupt Enable
            | SPI_CR2_RXNEIE   * 0  // RX buffer Not Empty Interrupt Enable
            | SPI_CR2_TXEIE    * 0; // Tx buffer Empty Interrupt Enable
  // SPI Enable
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 1  // SPI Enable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 0  // Data Frame Format 0: 8-bit format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // output mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode disable

}


/*******************************************************************************
* init SPI1 to read 8 bit data
*******************************************************************************/
void spi_read8 (void)
{
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 1  // SPI disable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 0  // Data Frame Format 0: 8-bit format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 0  // read
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode disable
}


/*******************************************************************************
* modify SPI1 to send 16 bit data
*******************************************************************************/
void spi_set16 (void)
{
  wait_spi_finish ();
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 0  // SPI disable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 0  // 0: 8-bit data frame format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // write mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode enable
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 0  // SPI disable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 1  // 1: 16-bit data frame format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // write mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode disable
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 1  // SPI Enable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 1  // 1: 16-bit data frame format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // write mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode disable
}



/*******************************************************************************
* modify SPI1 to send 16 bit data
*******************************************************************************/
void spi_set8 (void)
{
  wait_spi_finish ();
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 0  // SPI disable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 1  // 1: 16-bit data frame format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // write mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode enable
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 0  // SPI disable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 0  // 0: 8-bit data frame format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // write mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode disable
  SPI1->CR1 = SPI_CR1_CPHA     * 0  // Clock Phase
            | SPI_CR1_CPOL     * 0  // Clock Polarity
            | SPI_CR1_MSTR     * 1  // Master Selection
            | SPI_CR1_BR_F2         // Baud Rate Control = fPCLK/4
            | SPI_CR1_SPE      * 1  // SPI Enable
            | SPI_CR1_LSBFIRST * 0  // Frame Format
            | SPI_CR1_SSI      * 1  // Internal slave select
            | SPI_CR1_SSM      * 0  // Software slave management
            | SPI_CR1_RXONLY   * 0  // 0: Full duplex
            | SPI_CR1_DFF      * 0  // 0: 8-bit data frame format
            | SPI_CR1_CRCNEXT  * 0  // Transmit CRC next
            | SPI_CR1_CRCEN    * 0  // Hardware CRC calculation enable
            | SPI_CR1_BIDIOE   * 1  // write mode
            | SPI_CR1_BIDIMODE * 1; // Bidirectional data mode disable
}

void dma_init (void)
{
  DMA1_Channel3->CCR = DMA_CCR1_EN     * 0     // Channel enable
            | DMA_CCR1_TCIE    * 0     // Transfer complete interrupt enable
            | DMA_CCR1_HTIE    * 0     // Half Transfer interrupt enable
            | DMA_CCR1_TEIE    * 0     // Transfer error interrupt enable
            | DMA_CCR1_DIR_FROM_MEMORY // Data transfer direction 
            | DMA_CCR1_CIRC    * 0     // Circular mode
            | DMA_CCR1_PINC    * 0     // Peripheral increment mode
            | DMA_CCR1_MINC    * 1     // Memory increment mode
            | DMA_CCR1_PSIZE_16        // Peripheral size
            | DMA_CCR1_MSIZE_16        // Memory size
            | DMA_CCR1_PL_HIGH         // Channel Priority level
            | DMA_CCR1_MEM2MEM * 0;    // Memory to memory mode 
  DMA1_Channel3->CPAR = (u32)(&SPI1->DR);
}




/******************************************************************************
* Передача данных на дисплей
* Вход - u8 данные
******************************************************************************/
inline void lcd_send8 (u8 data)
{
  wait_spi_buffer ();
  SPI1->DR = data;  
}


/******************************************************************************
* Передача данных на дисплей
* Вход - u16 данные
******************************************************************************/
inline void lcd_send16 (u16 data)
{
  wait_spi_buffer ();
  SPI1->DR = data;  
}


/******************************************************************************
* Ожидание окончания передачи SPI
******************************************************************************/
inline void wait_spi_buffer  (void)
{
  while (!(SPI1->SR & SPI_SR_TXE))
  {
  }
}

inline void wait_spi_finish (void)
{
  while (SPI1->SR & SPI_SR_BSY)
  {
  }
}

/******************************************************************************
* Передача данных на дисплей
* Вход - u8 данные
******************************************************************************/
inline void lcd_data (u8 data) 
{
  wait_spi_finish ();
  LCD_DATA;
  SPI1->DR = data;  
}


/******************************************************************************
* Передача данных на дисплей
* Вход - u16 данные
******************************************************************************/
inline void lcd_data16 (u16 data) 
{
  wait_spi_finish ();
  LCD_DATA;
  SPI1->DR = data;  
}

/******************************************************************************
* Передача команды на дисплей
* Вход - u8 команда
******************************************************************************/
inline void lcd_command (u8 command) 
{
  wait_spi_finish ();
  LCD_COMMAND;
  SPI1->DR = command; 
}


/******************************************************************************
* Сброс дисплея
******************************************************************************/
inline void lcd_reset (void) 
{
  LCD_CS    (0);
  LCD_RST   (0);
  vTaskDelay  (1);
  LCD_RST   (1);
  vTaskDelay  (150);
}




/******************************************************************************
* начало передачи массива данных в дисплей
******************************************************************************/
inline void lcd_start_data (void)
{
  wait_spi_finish ();
	LCD_CS (0);	
	lcd_command (RAMWR);      //lcd_command(RAMWR);
  wait_spi_finish ();
  LCD_DATA;
}

/******************************************************************************
* конец передачи массива данных в дисплей
*******************************************************************************/
inline void lcd_stop_data (void)
{
  wait_spi_finish ();
//	LCD_CS (1);
//	__no_operation ();
//	__no_operation ();
//  LCD_CS (0);
	lcd_command (NOP);	
  wait_spi_finish ();
  LCD_CS (1);
}



/******************************************************************************
* команда чтения из дисплея n байтов
* Вход:  u8 - команда
*        n - количество байтов
* Выход: u8[n] данные
******************************************************************************/
void lcd_read (u8 command, u8 *result, u8 n) 
{
  wait_spi_finish ();
  LCD_CS (0);
  lcd_command (command);
  wait_spi_finish ();
//	SPI1->CR1 &= ~SPI_CR1_SPE;	// запретили SPI
//  LCD_SCK (1);		// дёрнули SCK
//  __no_operation ();
//  __no_operation ();
//	LCD_SCK (0);
//  spi_init8 ();

	while (n)  // приём
	{
    spi_read8 ();
    while (!(SPI1->SR & SPI_SR_RXNE));
    n--;
		result[n] = (u8)SPI1->DR;
	}
  LCD_CS (1);
}




/******************************************************************************
* Установить ориентацию дисплея.
* сканирование пикселей сверху вниз и слева направо
* по установленной ориентации
* вход - указатель на структуру lcd_param
* параметр orient
******************************************************************************/
void lcd_set_orient (lcd_orient	orient)
{
	lcd.orient = orient;
  
  wait_spi_finish ();
  LCD_CS (0);
	lcd_command (MADCTL);
	switch (lcd.orient)
	{
	case PORTRAIT:
		lcd_data (MADCTR_MX	* 0	// Column Address Order
        | MADCTR_MY	* 0	// Row Address Order
				| MADCTR_MV	* 1	// Row/Column Exchange
				| MADCTR_ML	* 0	// Vertical Refresh Order
				| MADCTR_MH	* 0	// Horisontal Refresh Order
				| MADCTR_RGB * 0);
		lcd.x_offset = ST7789_OFFSET_X_PORTRAIT;
		lcd.y_offset = ST7789_OFFSET_Y_PORTRAIT;
		lcd.x_max = ST7789_XMAX;
		lcd.y_max = ST7789_YMAX;
		break;
		
	case ALBUM:
		lcd_data (MADCTR_MX	* 1	// Column Address Order
				| MADCTR_MY	* 0	// Row Address Order
				| MADCTR_MV	* 0	// Row/Column Exchange
				| MADCTR_ML	* 0	// Vertical Refresh Order
				| MADCTR_MH	* 1	// Horisontal Refresh Order
				| MADCTR_RGB * 0);
		lcd.x_offset = ST7789_OFFSET_X_ALBUM;
		lcd.y_offset = ST7789_OFFSET_Y_ALBUM;
		lcd.x_max = ST7789_YMAX;
		lcd.y_max = ST7789_XMAX;
		break;
	
	case REV_PORTRAIT:
		lcd_data (MADCTR_MX	* 1	// Column Address Order
				| MADCTR_MY	* 1	// Row Address Order
				| MADCTR_MV	* 1	// Row/Column Exchange
				| MADCTR_ML	* 1	// Vertical Refresh Order
				| MADCTR_MH	* 0	// Horisontal Refresh Order
				| MADCTR_RGB * 0);
		lcd.x_offset = ST7789_OFFSET_X_REV_PORTRAIT;
		lcd.y_offset = ST7789_OFFSET_Y_REV_PORTRAIT;
		lcd.x_max = ST7789_XMAX;
		lcd.y_max = ST7789_YMAX;
		break;

	case REV_ALBUM:
		lcd_data (MADCTR_MX	* 0	// Column Address Order
				| MADCTR_MY	* 1	// Row Address Order
				| MADCTR_MV	* 0	// Row/Column Exchange
				| MADCTR_ML	* 1	// Vertical Refresh Order
				| MADCTR_MH	* 1	// Horisontal Refresh Order
				| MADCTR_RGB * 0);
		lcd.x_offset = ST7789_OFFSET_X_REV_ALBUM;
		lcd.y_offset = ST7789_OFFSET_Y_REV_ALBUM;
		lcd.x_max = ST7789_YMAX;
		lcd.y_max = ST7789_XMAX;
		break;
	}
  wait_spi_finish ();
  LCD_CS (1);
}


/******************************************************************************
* установка окна вывода на дисплее
* вход - параметры x y width height в структуре lcd_param
* выход - число выводимых пикселей
******************************************************************************/
u32 lcd_set_window (void)
{
	volatile u32	temp;
	
	if ((lcd.x >= lcd.x_max)
		|| (lcd.y >= lcd.y_max))
	{
		return 0;	// За пределами экрана
	}
	if ((lcd.x + lcd.width) > lcd.x_max)	// проверка границ
	{
		lcd.width = (lcd.x_max) - lcd.x;
	}
	if ((lcd.y + lcd.height) > lcd.y_max)
	{
		lcd.height = (lcd.y_max) - lcd.y;
	}

	// Диапазон столбцов
  wait_spi_finish ();
  LCD_CS (0);
	lcd_command (RASET);      //lcd_command(CASET);
	temp = lcd.x_offset + lcd.x;
  spi_set16 ();
	lcd_data16 (temp);
	temp += lcd.width;
	temp--;
	lcd_data16 (temp);
  spi_set8 ();
  LCD_CS (1);
  __no_operation ();
  __no_operation ();
  
	// Диапазон строк
  LCD_CS (0);
	lcd_command (CASET);      //lcd_command(RASET);
	temp = lcd.y_offset + lcd.y;
  spi_set16 ();
	lcd_data16 (temp);
	temp += lcd.height;
	temp--;
	lcd_data16 (temp);	
  spi_set8 ();
  LCD_CS (1);
  __no_operation ();
  __no_operation ();

	// Возвращаем количество видимых пикселей
	return ((u32)lcd.width * (u32)lcd.height);	
}




/******************************************************************************
* Вывод закрашенного прямоугольника 
* Вход - параметры x y width height в структуре lcd_param
* и цвет front
******************************************************************************/
void lcd_rect (void)
{
	u32 count = lcd_set_window ();    // Функция возвращает количество видимых пикселей в установленной области вывода.
	lcd_start_data ();
  spi_set16 ();
//	while (count--) 
//	{
//    wait_spi_buffer ();
//    SPI1->DR = lcd.front;  
//	}
  DMA1->IFCR = DMA_IFCR_CTCIF3
              | DMA_IFCR_CHTIF3
              | DMA_IFCR_CGIF3;  // clear flag
  DMA1_Channel3->CMAR = (u32)(&lcd.front);
  DMA1_Channel3->CCR = DMA_CCR1_EN * 0 // Channel enable
            | DMA_CCR1_TCIE    * 0     // Transfer complete interrupt enable
            | DMA_CCR1_HTIE    * 0     // Half Transfer interrupt enable
            | DMA_CCR1_TEIE    * 0     // Transfer error interrupt enable
            | DMA_CCR1_DIR_FROM_MEMORY // Data transfer direction 
            | DMA_CCR1_CIRC    * 0     // Circular mode
            | DMA_CCR1_PINC    * 0     // Peripheral increment mode
            | DMA_CCR1_MINC    * 0     // Memory increment mode
            | DMA_CCR1_PSIZE_16        // Peripheral size
            | DMA_CCR1_MSIZE_16        // Memory size
            | DMA_CCR1_PL_HIGH         // Channel Priority level
            | DMA_CCR1_MEM2MEM * 0;    // Memory to memory mode  
  DMA1_Channel3->CNDTR = count;
  DMA1_Channel3->CCR = DMA_CCR1_EN * 1 // Channel enable
            | DMA_CCR1_TCIE    * 0     // Transfer complete interrupt enable
            | DMA_CCR1_HTIE    * 0     // Half Transfer interrupt enable
            | DMA_CCR1_TEIE    * 0     // Transfer error interrupt enable
            | DMA_CCR1_DIR_FROM_MEMORY // Data transfer direction 
            | DMA_CCR1_CIRC    * 0     // Circular mode
            | DMA_CCR1_PINC    * 0     // Peripheral increment mode
            | DMA_CCR1_MINC    * 0     // Memory increment mode
            | DMA_CCR1_PSIZE_16        // Peripheral size
            | DMA_CCR1_MSIZE_16        // Memory size
            | DMA_CCR1_PL_HIGH         // Channel Priority level
            | DMA_CCR1_MEM2MEM * 0;    // Memory to memory mode  
  SPI1->CR2 = SPI_CR2_TXDMAEN;
  while (!(DMA1->ISR & DMA_ISR_TCIF3));
  DMA1->IFCR = DMA_IFCR_CTCIF3
              | DMA_IFCR_CHTIF3
              | DMA_IFCR_CGIF3;
  SPI1->CR2 = 0;
  
  spi_set8 ();
	lcd_stop_data ();
}

/******************************************************************************
* Создание окна
* Вход - параметры win_x win_y win_width win_height в структуре lcd_param
* и цвет front и back и border - ширина, и размеры окна
******************************************************************************/
void lcd_create_window (void)
{
  lcd.x = lcd.win_x;
  lcd.y = lcd.win_y;
  lcd.width = lcd.win_width;
  lcd.height = lcd.win_height;
  lcd_clear_window ();
  
  lcd.win_mode = LCD_WINDOW;
  lcd.x = lcd.win_x + lcd.border;
  lcd.y = lcd.win_y + lcd.border;
}

/******************************************************************************
* Закрытие окна
* Вход - параметры win_x win_y win_width win_height в структуре lcd_param
* и цвет front и back и border - ширина, и размеры окна
******************************************************************************/
void lcd_close_window (void)
{
  lcd.x = lcd.win_x;
  lcd.y = lcd.win_y;
  lcd.width = lcd.win_width;
  lcd.height = lcd.win_height;
  lcd.border = 0;
  
  lcd_clear_rect ();
  
  lcd.win_mode = LCD_SCREEN;
  lcd.x = 0;
  lcd.y = 0;
}

/******************************************************************************
* Прорисовка окна с рамкой
* Вход - параметры x y width height в структуре lcd_param
* и цвет back и border - ширина, и размеры окна
******************************************************************************/
void lcd_clear_rect (void)
{
	u16 temp = lcd.front;
	lcd.front = lcd.back;
	lcd_rect ();
	lcd.front = temp;
  if (lcd.border)
  {
    lcd_border_window ();
  }     
}

/******************************************************************************
* Очистка дисплея
* вход - параметр back в структуре lcd_param
******************************************************************************/
void lcd_clear (void)
{
	u16 temp = lcd.front;
	lcd.front = lcd.back;
	lcd.x = 0;
	lcd.y = 0;
	lcd.width = lcd.x_max;
	lcd.height = lcd.y_max;
	lcd_rect ();
	lcd.front = temp;
}

/******************************************************************************
* Нарисовать один пиксель по координатам
* Вход - параметры x y в структуре lcd
* и цвет front
******************************************************************************/
void lcd_pixel (void)
{
  lcd.width = lcd.height = 1;
  
	lcd_set_window ();
  lcd_start_data ();
  spi_set16 ();
	lcd_data16 (lcd.front);
  spi_set8 ();
  lcd_stop_data ();
}

/******************************************************************************
* Программа lcd_draw_symbol_mono
* Что делает: выводит на экран один символ из шрифта или монохромную картинку
* Входные данные: lcd_STRUCT *lcd - адрес структуры данных для вывода
* lcd->x - начальная координата по Х
* lcd->y - начальная координата по У
* lcd->image - адрес структуры символа или картинки
* lcd->front - цвет шрифта
* lcd->back - цвет фона
* Возвращаемое значение: нет
******************************************************************************/
void lcd_draw_symbol_mono (void)
{
    u8 x0, y0;
    u8 value = 0;
    u8 counter = 0;
 	const tImage *symbol = lcd.image;
  const u8 *pdata = (const u8 *)symbol->data;	// входные данные из шрифта
	// задаём координаты окна
	lcd_set_window ();
  wait_spi_finish ();
  LCD_CS (0);
	lcd_command (RAMWR);		// start_data
  wait_spi_finish ();
  spi_set16 ();
	// columns
	for (x0 = 0; x0 < symbol->width; x0++)
    {
		// rows
		for (y0 = 0; y0 < symbol->height; y0++)
    {
    // load new data
    if (!counter)
    {
      value = *pdata++;
      counter = 8;
    }
    counter--;
    // set pixel
    if (value & 0x80)
    {
      lcd_data16 (lcd.text_front);
    }
    else
    {
      lcd_data16 (lcd.text_back);
    }
      value = value << 1;
		}
	}
  wait_spi_finish ();
  spi_set8 ();
	lcd_command (NOP);		// stop_data
  wait_spi_finish ();
  LCD_CS (1);
}

/******************************************************************************
* Программа lcd_draw_symbol_mono_rle
* Что делает: выводит на экран один символ из шрифта, упакованного RLE
* Входные данные: lcd_STRUCT *lcd - адрес структуры данных для вывода
* lcd->x - начальная координата по Х
* lcd->y - начальная координата по У
* lcd->image - адрес структуры символа в шрифте
* lcd->color - цвет шрифта
* lcd->back - цвет фона
* Возвращаемое значение: нет
*******************************************************************************/
void lcd_draw_symbol_mono_rle (void)
{
  u16	x0, y0;
  u8	counter = 0;		// счётчик бит в слове данных шрифта
  s8	sequence = 0;		// счётчик повторяющихся символов (знаковый)
  s8	nonsequence = 0;	// счётчик неповторяющихся символов (знаковый)
  u8	value = 0;			// распакованные данные
	
	const u8 width = lcd.image->width;
	const u8 height = lcd.image->height;
  const u8 *pdata = (const u8 *)lcd.image->data;	// входные данные из шрифта
	
	lcd_set_window ();
  wait_spi_finish ();
  LCD_CS (0);
	lcd_command (RAMWR);      // start_data
  wait_spi_finish ();
  spi_set16 ();
	// columns
	for (x0 = 0; x0 < width; x0++)	
  {
  // rows
  for (y0 = 0; y0 < height; y0++)
    {
      // load new data
      if (!counter)		// Если первый бит в байте - загрузить байт из шрифта
      {
        if (!sequence && !nonsequence)
        {
          sequence = *pdata++;	// загрузка первого байта последовательности
          if (sequence < 0)		// если число отрицательное	
          {
            nonsequence = -sequence;
            sequence = 0;		// то это число неповторяющихся байт
          }
        }
        if (sequence)		// если повторения
        {
          value = *pdata;	// загрузить байт для повтроения
          sequence--;		// посчитать количество повторяющихся байт
          if (!sequence)	// когда повтор окончится, сдвинуть указатель на шрифт
          {
            pdata++;
          }
        }
        if (nonsequence)	// если неповторяющаяся последовательность
        {
          value = *pdata++;	// загрузить байт из шрифта
          nonsequence--;	// посчитать количество неповторяющихся байт
        }
        counter = 8;	//symbol->dataSize;	// размер данных в шрифте
      }
      counter--;
      // set pixel
      if (value & 0x80)
      {
      lcd_data16 (lcd.text_front);
      }
      else
      {
      lcd_data16 (lcd.text_back);
      }
      value = value << 1;
    }
  }
  wait_spi_finish ();
  spi_set8 ();
	lcd_command (NOP);		// stop_data
  wait_spi_finish ();
  LCD_CS (1);
}

/******************************************************************************
* Программа lcd_find_char_by_code
* Что делает: ищет адрес структуры символа в шрифте по его коду 
* Входные данные: u8 code - код символа
* tFont *font - указатель на структуру шрифта
* Возвращаемое значение: адрес структуры tChar или 0, если не нашёл
******************************************************************************/
tChar *lcd_find_char_by_code(u8 code, const tFont *font)
{
    u8 count = font->length;
    u8 first = 0;
    u8 last = count - 1;
    u8 mid = 0;

    if (count)
    {
        if ((code >= font->chars[0].code) && (code <= font->chars[count - 1].code))
        {
            while (last >= first)
            {
                mid = first + ((last - first) / 2);

                if (font->chars[mid].code < code)
                    first = mid + 1;
                else
                    if (font->chars[mid].code > code)
                        last = mid - 1;
                    else
                        break;
            }

            if (font->chars[mid].code == code)
                return (tChar *)(&font->chars[mid]);
        }
    }
    return (0);
}



/******************************************************************************
 * Имя                   :  LcdLine
 * Описание              :  Рисует линию между двумя точками на дисплее (алгоритм Брезенхэма)
 * Аргумент(ы)           :  x1, y1  -> абсолютные координаты начала линии
 *                          x2, y2  -> абсолютные координаты конца линии
 * Возвращаемое значение :  нет
******************************************************************************/
void lcd_line (s16 x1, s16 y1, s16 x2, s16 y2)
{
    s16 dx, dy, stepx, stepy, fraction;

    // dy   y2 - y1
    // -- = -------
    // dx   x2 - x1

    dy = y2 - y1;
    dx = x2 - x1;
	if ((dx == 1) || (dy == 1))		// если горизонтальная или вертикальная прямая
	{					// то рисовать прямоугольник
		if (x1 < x2)
		{
			lcd.x = x1;
			lcd.width = x2 - x1;
		}
		else
		{
			lcd.x = x2;
			lcd.width = x1 - x2;
		}
		if (y1 < y2)
		{
			lcd.y = y1;
			lcd.height = y2 - y1;
		}
		else
		{
			lcd.y = y2;
			lcd.height = y1 - y2;
		}
		lcd_rect ();
	}
	
	// dy отрицательное
	if ( dy < 0 )
	{
		dy    = -dy;
		stepy = -1;
	}
	else
	{
		stepy = 1;
	}

	// dx отрицательное
	if ( dx < 0 )
	{
		dx    = -dx;
		stepx = -1;
	}
	else
	{
		stepx = 1;
	}

	dx <<= 1;
	dy <<= 1;

	// Рисуем начальную точку
  lcd.x = x1;
  lcd.y = y1; 
	lcd_pixel ();

	// Рисуем следующие точки до конца
	if (dx > dy)
	{
		fraction = dy - (dx >> 1);
		while (lcd.x != x2)
		{
			if (fraction >= 0)
			{
				lcd.y += stepy;
				fraction -= dx;
			}
			lcd.x += stepx;
			fraction += dy;
			lcd_pixel ();
		}
	}
	else
	{
		fraction = dx - ( dy >> 1);
		while (lcd.y != y2 )
		{
			if (fraction >= 0 )
			{
				lcd.x += stepx;
				fraction -= dy;
			}
			lcd.y += stepy;
			fraction += dx;
			lcd_pixel ();
		}
	}
}



/******************************************************************************
* Имя                   :  LcdCircle
* Описание              :  Рисует окружность (алгоритм Брезенхэма)
* Аргумент(ы)           :  x, y   -> абсолютные координаты центра
*                          radius -> радиус окружности
* Возвращаемое значение :  нет
******************************************************************************/
void lcd_circle (s16 x, s16 y, s16 radius)
{
  s16	xc = 0;
  s16	yc = 0;
  s16	p = 0;

  yc = radius;
  p = 3 - (radius << 1);

  while (xc <= yc)  
  {
    lcd.x = x + xc;
    lcd.y = y + yc;
    lcd_pixel ();
    lcd.x = x + xc;
    lcd.y = y - yc;
    lcd_pixel ();
    lcd.x = x - xc;
    lcd.y = y + yc;
    lcd_pixel ();
    lcd.x = x - xc;
    lcd.y = y - yc;
    lcd_pixel ();
    lcd.x = x + yc;
    lcd.y = y + xc;
    lcd_pixel ();
    lcd.x = x + yc;
    lcd.y = y - xc;
    lcd_pixel ();
    lcd.x = x - yc;
    lcd.y = y + xc;
    lcd_pixel ();
    lcd.x = x - yc;
    lcd.y = y - xc;
    lcd_pixel ();
    if (p < 0)
    {
      p += (xc++ << 2) + 6;
    }
        else 
    {
      p += ((xc++ - yc--)<<2) + 10;
    }
  }
}




/******************************************************************************
* Программа lcd_draw_string
* Что делает: выводит на экран строку символов
* Входные данные: char *str - адрес строки символов
* lcd_STRUCT lcd - адрес строки для вывода
* lcd->w_x - начальная позиция окна по X
* lcd->w_y - начальная позиция окна по Y
* lcd->w_width - ширина окна
* lcd->w_height - высота окна
* lcd->x - начальная координата по Х
* lcd->y - начальная координата по У
* lcd->text_front - цвет шрифта
* lcd->text_back - цвет фона
* CURSOR указывает координаты вывода x и у
* Возвращаемое значение: нет
******************************************************************************/
void lcd_draw_string (const char *str)
{
	tChar		*ch;

    while (*str)
    {
		ch = lcd_find_char_by_code (*str, lcd.font);
		if (ch)
		{
			if ((lcd.x + ch->image->width) > lcd.x_max)	// проверка на правое поле
			{
				lcd.y += ch->image->height;				// переход на новую строку
				lcd.x = 0;						            //left_base;
			}
			if ((lcd.y + ch->image->height) > lcd.y_max)	// проверка на нижнее поле
			{
				lcd.x = 0;						            // переход в начало окна
				lcd.y = 0;
			}
			lcd.image	= ch->image;
			lcd.width	= ch->image->width;
			lcd.height	= ch->image->height;
			lcd_draw_symbol_mono_rle ();
			lcd.x += ch->image->width;
		}
		str++;
    }
}

/*******************************************************************************
* Общая функция инициализации и работы с дисплеем. Нет задержек в циклах
* После инициализации передаёт сообщение "M_LCD_READY"
*
*
*******************************************************************************/
void lcd_func (void)
{
	typedef enum _lcd_states
	{
    LCD_STATE_INIT = 0,
		LCD_STATE_READY,
		LCD_STATE_PARSER
	} lcd_states;

	int s = sizeof (ST7789_init) / sizeof (ST7789_init[0]);
  lcd_init_data *dat = (lcd_init_data *)&ST7789_init;
	int i;
	static lcd_states	lcd_state = LCD_STATE_INIT;
	
	switch (lcd_state)
	{
	case LCD_STATE_INIT:
		memcpy (&lcd, &lcd_default, sizeof (lcd_default));	// заполнение данных по умолчанию
		
    // инициализация из массива данных
    spi_init8 ();
    dma_init ();
//    lcd_read_param ();
    
    for(i = 0; i < s; i++, dat++)
    {
      if (dat->command == TFTLCD_DELAY)
      {
        vTaskDelay (dat->data);
      }
      else 
      {
        LCD_CS (0);	
        lcd_command (dat->command);
        lcd_data    (dat->data);
        wait_spi_finish ();
        LCD_CS (1);	
      }
    }
    lcd_set_orient (ALBUM);
    lcd_clear ();
    LCD_LIGHT_ON;		// подсветка	
    xEventGroupSetBits (g.events, M_LCD_READY);
    lcd_state = LCD_STATE_READY;
		break;
		
	case LCD_STATE_READY:
		// приём сообщений и вывод на дисплей
		if (xStreamBufferBytesAvailable (g.lcd_stream))
		{
			lcd_state = LCD_STATE_PARSER;
		}
    if (xEventGroupClearBits (g.events, M_LCD_RESET))
    {
      lcd_state = LCD_STATE_INIT;
    }
		break;
		
	case LCD_STATE_PARSER:
		if (xStreamBufferBytesAvailable (g.lcd_stream))		// пока есть символы в строке
		{
      char    str[STRBUF_SIZE];
      u8 count = xStreamBufferReceive (g.lcd_stream, str, sizeof(str), 0);
			lcd_parser (str);					// разобрать строку и напечатать
		}
		else				// когда строка закончилась
		{
			lcd_state = LCD_STATE_READY;	// вернуться в режим ожидания
		}
    if (xEventGroupClearBits (g.events, M_LCD_RESET))
    {
      lcd_state = LCD_STATE_INIT;
    }
		break;
		
	default:
		lcd_state = LCD_STATE_INIT;
		break;
	}
	
}

/*******************************************************************************
*
*******************************************************************************/
void lcd_task (void *param)
{
  (void)param;
  
  while (1)
  {
    lcd_func ();
    vTaskDelay (5);
  }
}

/*******************************************************************************
* Препроцессор, разбирающий управляющие последовательности
* Вход - строка с текстом и управляющими ESC последовательностями 
* Строка ограничивается нулём. Не длиннее STRBUF_SIZE
* Выход - программа lcd_draw_string и графические программы
* Управляющие последовательности
* ESC = '~'
* Печать "~"				      "~~"
* Сброс экрана				    "~m"
* Установить границы окна	"~Y1;Y2;X1;X2L" Y1-верх, Y2-низ X1-лево, X2-право в пикселях
* Позиция курсора			    "~Y;XH" Y-строка X-столбец в пикселях
* Очистка экрана			    "~NJ" N=0 до конца экрана N=1 до начала экрана N=2 весь экран N=3 окно
* Очистка строки			    "~NK"	N=0 до конца строки N=1 до начала N=2 вся строка
* Курсор вверх				    "~YA" Y-количество строк
* Курсор вниз				      "~YB" Y-количество строк
* Курсор вправо				    "~XC" X-количество символов
* Курсор влево				    "~XD" X-количество символов
* Новая строка				    "~YE" Y-количество новых строк
* Курсор скрыть/отобразить	"~NO" N=0 скрыть N=1 отобразить
* Инверсия текста вкл		  "~7m"
* Установить шрифт			  "~Nm"   N=10-19 10-шрифт основной 11-19 дополнительные
* Инверсия текста выкл		"~27m"
* Цвет текста				      "~N;Mm" N=30-39(цвет) M=0-1(яркость)
* Цвет фона					      "~N;Mm" N=40-49(цвет) M=0-1(яркость)
* Управление границей окна	"~Nm"   N=51 граница 1 пиксель, N=52 2 пикселя, N=54 нет границы
* Цвет фигур				      "~N;Mm" N=50-59(цвет) M=0-1(яркость)
* Цвет фона	фигур			    "~N;Mm" N=60-69(цвет) M=0-1(яркость)
* Сдвинуть окно вверх		  "~NS" N строк
* Сдвинуть окно вниз		  "~NT" N строк
*
* Графические фигуры:
* Линия						        "~X1;Y1;X2;Y2l"	X1,Y1 - старт X2,Y2 - финиш
* круг						        "~X;Y;R;Fc"		X,Y - центр окружности R - радиус F - признак круга
* прямоугольник	незакр.		"~X1;Y1;W;Hs"	X1,Y1 - угол 1 W – ширина H - высота
* прямоугольник	закр.		  "~X1;Y1;W;Hq"	X1,Y1 - угол 1 W – ширина H - высота
* пиксель					        "~X;Yp"			X,Y - координаты
*
* Управляющие коды:
* \b	0x08	BS, забой
* \t	0x09	Горизонтальная табуляция
* \n	0x0A	Новая строка, перевод строки
* \v	0x0B	Вертикальная табуляция
* \f	0x0C 	Новая страница, перевод страницы
* \r	0x0D	Возврат каретки
* цвет: 0-Black	1-Red	2-Green	3-Yellow	4-Blue	5-Magenta	6-Cyan	7-White
* 8-пользовательский 9- по умолчанию
*******************************************************************************/
void lcd_parser (const char *str)
{
	#define L_ESC		    ('~')	//(0x1B)
	#define	L_COMM1		  ('m')
	#define	L_WINDOW	  ('L')
	#define	L_CLEAR_WIN	('J')
	#define	L_CLEAR_ROW	('K')
	#define	L_CUR_MOV	  ('H')
	#define	L_UP		    ('A')
	#define	L_DOWN		  ('B')
	#define	L_RIGHT		  ('C')
	#define	L_LEFT		  ('D')
	#define	L_NEW_ROW	  ('E')
	#define	L_CURSOR	  ('O')
	#define	L_WIN_UP	  ('S')
	#define	L_WIN_DOWN	('T')
	#define	L_LINE		  ('l')
	#define	L_CIRCLE	  ('c')
	#define	L_SQUARE1	  ('s')
	#define	L_SQUARE2	  ('q')
	#define	L_PIXEL		  ('p')

	
	static u8		num_arg;		    // число аргументов в управляющей последовательности
	static u8		curr_pos;		    // текущая позиция в строке
	static char	arg_str[4][4];	// аргументы - строки в реверсивном порядке " 123" = "321 "
	static u8		arg[4];			    // числовые значения аргументов в управляющей последовательности
	
	typedef enum _lcd_parser_states
	{
		PARSER_READY = 0,
		PARSER_PRINT,
		PARSER_CONTROL,
		PARSER_ESC1,
		PARSER_DIGIT,
		PARSER_ARG,
		PARSER_ADD_ARG,
		PARSER_DIGIT_IN,
		PARSER_NEXT_ARG,
		PARSER_COMM
	} lcd_parser_states;
	
	static lcd_parser_states	lcd_parser_state	= PARSER_READY;
	
	u8 	n;
  u8  count;
	u16	color;
	tChar		*ch;
	tChar		*tch;
  char    symbol;
  count = strlen (str);
  u32 timer;
  
  timer_reset (&timer);

  for (u8 k = 0; k < count; k++)
  {
    if (timer_end (&timer, 5)) // для работы остальных задач
    {
      timer_reset (&timer);
      vTaskDelay (1);
    }
    symbol = str[k];
    switch (lcd_parser_state)
    {
    case PARSER_READY:	// начало разборки строки
      if (symbol == L_ESC)
      {
        lcd_parser_state = PARSER_ESC1;
      }
      else if ((symbol < 0x0E) && (symbol > 0x07))
      {
        lcd_parser_state = PARSER_CONTROL;
      }
      else
      {
        lcd_parser_state = PARSER_PRINT;
      }		
      break;
      
    case PARSER_PRINT:	// просто печать символа
      ch = lcd_find_char_by_code (symbol, lcd.font);
      if (ch)
      {
        if ((lcd.x + ch->image->width) > lcd.x_max)	// проверка на правое поле
        {
          lcd.y += ch->image->height;				// переход на новую строку
          lcd.x = 0;	//left_base;
        }
        if ((lcd.y + ch->image->height) > lcd.y_max)	// проверка на нижнее поле
        {
          lcd.y = 0;
        }
        lcd.image	= ch->image;
        lcd.width	= ch->image->width;
        lcd.height	= ch->image->height;
        lcd_draw_symbol_mono_rle ();
        lcd.x	+= ch->image->width;
      }
      lcd_parser_state = PARSER_READY;
      break;
    
    case PARSER_CONTROL:
      switch (symbol)
      {
      case 0x08:	// удалить последний символ
        
        break;
        
      case 0x09:	// горизонтальная табуляция
        
        break;
        
      case 0x0A:	// новая строка
      case 0x0B:	// вертикальная табуляция
        tch = lcd_find_char_by_code ('0', lcd.font);
        
        lcd.y += tch->image->height;
        if ((lcd.y + tch->image->height) >= lcd.y_max)
        {
          lcd.y = lcd.y_max - tch->image->height;
        }
        break;
        
      case 0x0C:	// новая страница
        lcd.x = 0;
        lcd.y = 0;
        break;
        
      case 0x0D:	// возврат каретки
        lcd.x = 0;			
        break;
        
      default:
        
        break;
      }
      lcd_parser_state = PARSER_READY;	// разбираем дальше	
      break;
      
    case PARSER_ESC1:	
      if (symbol == L_ESC)	// печать '~'
      {
        lcd_parser_state = PARSER_PRINT;
      }
      else 
      {
        // начало разборки управляющей последовательности
        // установка агрументов по умолчанию
        num_arg = 0;
        curr_pos = 0;
        for (u8 i = 0; i <= 3; i++)
        {
          for (u8 j = 0; j <= 3; j++)
          {
            arg_str[i][j] = 0;
          }
          arg[i] = 1;
        }
        lcd_parser_state = PARSER_DIGIT;
      }
      break;
      
    case PARSER_DIGIT:
      if ((symbol >= '0') && (symbol <= '9'))	// это цифра?
      {
        lcd_parser_state = PARSER_ADD_ARG;	// да - добавим аргумент в список
      }
      else 
      {
        lcd_parser_state = PARSER_NEXT_ARG; // нет - проверяем на следующий аргумент
      }
      break;
      
    case PARSER_ADD_ARG:
      num_arg++;							// добавили число аргументов,
      if (num_arg < 5)					// число аргументов меньше максимального?
      {
        lcd_parser_state = PARSER_ARG;		// да - заносим аргументы в память
      }
      else
      {
        lcd_parser_state = PARSER_READY;	// нет - сброс парсера
      }
      break;
      
    case PARSER_ARG:
      arg_str[num_arg - 1][curr_pos] = symbol;	// копируем символ аргумента в массив
      lcd_parser_state = PARSER_DIGIT_IN;			// смотрим на следующий символ
      break;
      
    case PARSER_DIGIT_IN:
      if ((symbol >= '0') && (symbol <= '9'))	// это цифра?
      {
        if (curr_pos < 3)					// число цифр меньше 4?
        {
          curr_pos++;
          lcd_parser_state = PARSER_ARG;	// если да, записываем следующую цифру
        }
        else
        {
          lcd_parser_state = PARSER_READY;	// нет - сброс парсера
        }
      }
      else
      {
        arg_str[num_arg - 1][curr_pos + 1] = 0;
        lcd_parser_state = PARSER_NEXT_ARG;
      }
      break;
      
    case PARSER_NEXT_ARG:
      if (symbol == ';')				// есть разделитель аргументов?
      {
        curr_pos = 0;
        lcd_parser_state = PARSER_ADD_ARG;	
      }
      else
      {
        lcd_parser_state = PARSER_COMM;	// нет - разбор команды
      }
           
      break;
      
    case PARSER_COMM:
      // преобразуем аргументы в числа
      for (u8 i = 0; i < num_arg; i++)
      {
        arg[i] = atou8 (arg_str[i]);
      }
      
      // Определение команды
      switch (symbol)
      {
      case L_COMM1:
        if (arg[0] == 0)
        {
        // Сброс экрана				"~0m"
          xEventGroupSetBits (g.events, M_LCD_RESET);
        }
        else if ((arg[0] == 7) || (arg[0] == 27))
        {
        // Инверсия текста вкл		"~7m"
        // Инверсия текста выкл		"~27m"
          color = lcd.text_front;
          lcd.text_front = lcd.text_back;				
          lcd.text_back = color;
        }
        else if ((arg[0] >= 10) && (arg[0] <= 19))
        {
        // Установить шрифт			"~Nm"  N=10-19 10-шрифт основной 11-19 дополнтельные
          n = arg[0] - 10;
          if (font_list[n])
          {
            lcd.font = font_list[n];
          }
        }
        else if ((arg[0] >= 30) && (arg[0] <= 39))
        {
        // Цвет текста				"~N;Mm" N=30-39(цвет) M=0-1
          lcd.text_front = lcd_default_colors[arg[0] - 30][arg[1]];
        }
        else if ((arg[0] >= 40) && (arg[0] <= 49))
        {
        // Цвет фона				"~N;Mm" N=40-49(цвет) M=0-1(яркость)
          lcd.text_back = lcd_default_colors[arg[0] - 40][arg[1]];
        }
        else if (arg[0] == 51)
        {
        // Управление границей окна	"~Nm" N=51 граница 1 пиксель
          lcd.border = 1;
        }
        else if (arg[0] == 52)
        {
        // Управление границей окна	"~Nm" N=52 2 пикселя
          lcd.border = 2;
        }
        else if (arg[0] == 54)
        {
        // Управление границей окна	"~Nm" N=54 нет границы
          lcd.border = 0;
        }
        else if ((arg[0] >= 60) && (arg[0] <= 69))
        {
        // Цвет фигур				"~N;Mm" N=60-69(цвет) M=0-1
          lcd.front = lcd_default_colors[arg[0] - 60][arg[1]];
        }
        else if ((arg[0] >= 70) && (arg[0] <= 79))
        {
        // Цвет фона фигур			"~N;Mm" N=70-79(цвет) M=0-1(яркость)
          lcd.back = lcd_default_colors[arg[0] - 70][arg[1]];
        }
        break;

      case L_WINDOW:
        // Установить границы окна	"~Y1;Y2;X1;X2L" Y1-начало окна по y, Y2-высота, X1-начало окна по x, X2-ширина в пикселях
        if (arg[0] < lcd.x_max)	// проверка корректности координат
        {
          lcd.y = arg[0];
        }
        else
        {
          lcd.y = lcd.x_max - 1;
        }
        if ((lcd.y + arg[1]) <= lcd.x_max)
        {
          lcd.height = arg[1];
        }
        else
        {
          lcd.height = lcd.x_max - lcd.y;
        }
        if (arg[2] < lcd.y_max)
        {
          lcd.x = arg[2];
        }
        else
        {
          lcd.x = lcd.y_max - 1;
        }
        if ((lcd.x + arg[3]) <= lcd.x_max)
        {
          lcd.width = arg[3];
        }
        else
        {
          lcd.width = lcd.y_max - lcd.x;
        }
        break;

      case L_CLEAR_WIN:
        if (arg[0] == 0)	// Очистка экрана N=0 до конца экрана
        {
  // не разработано					
        }
        else if (arg[0] == 1)	// Очистка экрана N=1 до начала экрана
        {
  // не разработано										
        }
        else if (arg[0] == 2)	// Очистка экрана N=2 весь экран
        {
          lcd_clear ();

        }
        else if (arg[0] == 3)	// Очистить экрана N=3 окно
        {
          lcd_clear_window ();
          lcd_border_window ();
        }
        break;

      case L_CLEAR_ROW:
        if (arg[0] == 0)	// Очистка строки N=0 до конца строки в пределах окна
        {
  // не разработано					
        }
        else if (arg[0] == 1)	// Очистка строки N=1 до начала
        {
  // не разработано					
        }
        else if (arg[0] == 2)	// Очистка строки N=2 вся строка
        {
          lcd_clear_row ();	
        }
        break;

      case L_CUR_MOV:
        // Позиция курсора			"~Y;XH" Y-строка X-столбец (в пикселях)
        if (arg[0] < lcd.y_max)
        {
          lcd.y = arg[0];
        }
        else
        {
          lcd.y = 0;
        }
        if (arg[1] < lcd.x_max)
        {
          lcd.x = arg[1];
        }
        else
        {
          lcd.x = 0;
        }
        break;

      case L_UP:
        // Курсор вверх				"~YA" Y-количество строк
        break;
        
      case L_DOWN:
        // Курсор вниз				"~YB" Y-количество строк
        break;
        
      case L_RIGHT:
        // Курсор вправо			"~XC" X-количество символов
        break;
        
      case L_LEFT:
        // Курсор влево				"~XD" X-количество символов
        break;
        
      case L_NEW_ROW:
        // Новая строка				"~YE" Y-количество новых строк
        lcd.x = 0;
        tch = lcd_find_char_by_code ('0', lcd.font);
        
        lcd.y += tch->image->height;
        if ((lcd.y + tch->image->height) >= lcd.y_max)
        {
          lcd.y = lcd.y_max - tch->image->height;
        }
        break;
        
      case L_CURSOR:
        // Курсор скрыть/отобразить	"~NO" N=0 скрыть N=1 отобразить
        lcd.cursor_on = arg[0];
        break;
        
      case L_WIN_UP:
        // Сдвинуть окно вверх		"~NS" N строк
        break;
        
      case L_WIN_DOWN:
        // Сдвинуть окно вниз		"~NT" N строк
        break;

      case L_LINE:
        lcd_line (arg[0], arg[1], arg[2], arg[3]);
        break;

      case L_CIRCLE:
        break;

      case L_SQUARE1:
        lcd.x = arg[0];
        lcd.y = arg[1];
        lcd.width = arg[2];
        lcd.height = arg[3];
        lcd_clear_rect ();
        break;

      case L_SQUARE2:
        lcd.x = arg[0];
        lcd.y = arg[1];
        lcd.width = arg[2];
        lcd.height = arg[3];
        lcd_rect ();
        break;

      case L_PIXEL:
        lcd.x = arg[0];
        lcd.y = arg[1];
        lcd_pixel ();
        break;
        
      default:
        break;
      }
  //		ring_buffer_remove (&lcd.buffer);
      lcd_parser_state = PARSER_READY;	// разбираем дальше		
      break;
      
    default:
      lcd_parser_state = PARSER_READY;
      break;
    }
  }
}

/*
Commands of display VT100:

Esc[20h 	Set new line mode 	LMN
Esc[?1h 	Set cursor key to application 	DECCKM
none 	Set ANSI (versus VT52) 	DECANM
Esc[?3h 	Set number of columns to 132 	DECCOLM
Esc[?4h 	Set smooth scrolling 	DECSCLM
Esc[?5h 	Set reverse video on screen 	DECSCNM
Esc[?6h 	Set origin to relative 	DECOM
Esc[?7h 	Set auto-wrap mode 	DECAWM
Esc[?8h 	Set auto-repeat mode 	DECARM
Esc[?9h 	Set interlacing mode 	DECINLM
 
Esc[20l 	Set line feed mode 	LMN
Esc[?1l 	Set cursor key to cursor 	DECCKM
Esc[?2l 	Set VT52 (versus ANSI) 	DECANM
Esc[?3l 	Set number of columns to 80 	DECCOLM
Esc[?4l 	Set jump scrolling 	DECSCLM
Esc[?5l 	Set normal video on screen 	DECSCNM
Esc[?6l 	Set origin to absolute 	DECOM
Esc[?7l 	Reset auto-wrap mode 	DECAWM
Esc[?8l 	Reset auto-repeat mode 	DECARM
Esc[?9l 	Reset interlacing mode 	DECINLM
 
Esc= 	Set alternate keypad mode 	DECKPAM
Esc> 	Set numeric keypad mode 	DECKPNM
 
Esc(A 	Set United Kingdom G0 character set 	setukg0
Esc)A 	Set United Kingdom G1 character set 	setukg1
Esc(B 	Set United States G0 character set 	setusg0
Esc)B 	Set United States G1 character set 	setusg1
Esc(0 	Set G0 special chars. & line set 	setspecg0
Esc)0 	Set G1 special chars. & line set 	setspecg1
Esc(1 	Set G0 alternate character ROM 	setaltg0
Esc)1 	Set G1 alternate character ROM 	setaltg1
Esc(2 	Set G0 alt char ROM and spec. graphics 	setaltspecg0
Esc)2 	Set G1 alt char ROM and spec. graphics 	setaltspecg1
 
EscN 	Set single shift 2 	SS2
EscO 	Set single shift 3 	SS3
 
Esc[m 	Turn off character attributes 	SGR0
Esc[0m 	Turn off character attributes 	SGR0
Esc[1m 	Turn bold mode on 	SGR1
Esc[2m 	Turn low intensity mode on 	SGR2
Esc[4m 	Turn underline mode on 	SGR4
Esc[5m 	Turn blinking mode on 	SGR5
Esc[7m 	Turn reverse video on 	SGR7
Esc[8m 	Turn invisible text mode on 	SGR8
 
Esc[Line;Liner 	Set top and bottom lines of a window 	DECSTBM
 
Esc[ValueA 	Move cursor up n lines 	CUU
Esc[ValueB 	Move cursor down n lines 	CUD
Esc[ValueC 	Move cursor right n lines 	CUF
Esc[ValueD 	Move cursor left n lines 	CUB
Esc[H 	Move cursor to upper left corner 	cursorhome
Esc[;H 	Move cursor to upper left corner 	cursorhome
Esc[Line;ColumnH 	Move cursor to screen location v,h 	CUP
Esc[f 	Move cursor to upper left corner 	hvhome
Esc[;f 	Move cursor to upper left corner 	hvhome
Esc[Line;Columnf 	Move cursor to screen location v,h 	CUP
EscD 	Move/scroll window up one line 	IND
EscM 	Move/scroll window down one line 	RI
EscE 	Move to next line 	NEL
Esc7 	Save cursor position and attributes 	DECSC
Esc8 	Restore cursor position and attributes 	DECSC
 
EscH 	Set a tab at the current column 	HTS
Esc[g 	Clear a tab at the current column 	TBC
Esc[0g 	Clear a tab at the current column 	TBC
Esc[3g 	Clear all tabs 	TBC
 
Esc#3 	Double-height letters, top half 	DECDHL
Esc#4 	Double-height letters, bottom half 	DECDHL
Esc#5 	Single width, single height letters 	DECSWL
Esc#6 	Double width, single height letters 	DECDWL
 
Esc[K 	Clear line from cursor right 	EL0
Esc[0K 	Clear line from cursor right 	EL0
Esc[1K 	Clear line from cursor left 	EL1
Esc[2K 	Clear entire line 	EL2
 
Esc[J 	Clear screen from cursor down 	ED0
Esc[0J 	Clear screen from cursor down 	ED0
Esc[1J 	Clear screen from cursor up 	ED1
Esc[2J 	Clear entire screen 	ED2
 
Esc5n 	Device status report 	DSR
Esc0n 	Response: terminal is OK 	DSR
Esc3n 	Response: terminal is not OK 	DSR
 
Esc6n 	Get cursor position 	DSR
EscLine;ColumnR 	Response: cursor is at v,h 	CPR
 
Esc[c 	Identify what terminal type 	DA
Esc[0c 	Identify what terminal type (another) 	DA
Esc[?1;Value0c 	Response: terminal type code n 	DA
 
Escc 	Reset terminal to initial state 	RIS
 
Esc#8 	Screen alignment display 	DECALN
Esc[2;1y 	Confidence power up test 	DECTST
Esc[2;2y 	Confidence loopback test 	DECTST
Esc[2;9y 	Repeat power up test 	DECTST
Esc[2;10y 	Repeat loopback test 	DECTST
 
Esc[0q 	Turn off all four leds 	DECLL0
Esc[1q 	Turn on LED #1 	DECLL1
Esc[2q 	Turn on LED #2 	DECLL2
Esc[3q 	Turn on LED #3 	DECLL3
Esc[4q 	Turn on LED #4 	DECLL4
 
 
Codes for use in VT52 compatibility mode
Esc< 	Enter/exit ANSI mode (VT52) 	setansi
 
Esc= 	Enter alternate keypad mode 	altkeypad
Esc> 	Exit alternate keypad mode 	numkeypad
 
EscF 	Use special graphics character set 	setgr
EscG 	Use normal US/UK character set 	resetgr
 
EscA 	Move cursor up one line 	cursorup
EscB 	Move cursor down one line 	cursordn
EscC 	Move cursor right one char 	cursorrt
EscD 	Move cursor left one char 	cursorlf
EscH 	Move cursor to upper left corner 	cursorhome
EscLineColumn		Move cursor to v,h location 	cursorpos(v,h)
EscI 	Generate a reverse line-feed 	revindex
 
EscK 	Erase to end of current line 	cleareol
EscJ 	Erase to end of screen 	cleareos
 
EscZ 	Identify what the terminal is 	ident
Esc/Z 	Correct response to ident 	identresp

 

 
VT100 Special Key Codes

These are sent from the terminal back to the computer when the particular key is pressed. Note that the numeric keypad keys send different codes in numeric mode than in alternate mode. See escape codes above to change keypad mode.

 

Function Keys:
EscOP	PF1
EscOQ	PF2
EscOR	PF3
EscOS	PF4

 

Arrow Keys:
  	Reset 	Set
up 	EscA	EscOA
down 	EscB	EscOB
right 	EscC	EscOC
left 	EscD	EscOD

 

Numeric Keypad Keys:
EscOp	0
EscOq	1
EscOr	2
EscOs	3
EscOt	4
EscOu	5
EscOv	6
EscOw	7
EscOx	8
EscOy	9
EscOm	-(minus)
EscOl	,(comma)
EscOn	.(period)
EscOM	^M

 

Printing:
Esc[i 	Print Screen 	Print the current screen
Esc[1i	Print Line 	Print the current line
Esc[4i	Stop Print Log 	Disable log
Esc[5i	Start Print Log	Start log; all received text is echoed to a printer
*/





/*******************************************************************************
* Функция печати из любого места программы
* Вход - нуль-терминированная строка с управляющими последовательностями
* макс. длина строки STRBUF_SIZE
*******************************************************************************/
void lcd_print (const char *str)
{
  xStreamBufferSend (g.lcd_stream, str, strlen(str), 0);
}

/*******************************************************************************
* itoa:  конвертируем сторку n в символы в s 
* выход: нуль терминированная строка
*******************************************************************************/
void itoa (s32 n, char *s)
{
  u8  j;
  u8  i = 0;
  u8  c = 0;

  if (n < 0)
  {
    n = -n;                 // делаем n положительным числом 
    c = 1;
  }
  do                        // генерируем цифры в обратном порядке 
  {      
    s[i++] = n % 10 + '0';  // берем следующую цифру 
    n /= 10;                // удаляем 
  } 
  while (n > 0);    
  if (c)
  {
     s[i++] = '-';
  }
  s[i--] = '\0';
  for (j = 0; i > j; j++, i--) 	// переворачиваем строку s на месте
	{
		c = s[j];
		s[j] = s[i];
		s[i] = c;
	}
}

/*******************************************************************************
* itoan:  конвертируем n в символы в s с заданной точностью num
* выход: нуль терминированная строка
*******************************************************************************/
void itoan (s32 n, char *s, u8 num)
{
	u8  j;
 	u8  i = 0, c = 0;
	if (n < 0)  	
	{
		n = -n;			// делаем n положительным числом
		c = 1;			// записываем знак
	}
	do 
	{       						      /* генерируем цифры в обратном порядке */
		s[i++] = n % 10 + '0';  /* берем следующую цифру */
		n /= 10;
	} 
	while (i < num);     	    //делаем столько раз, сколько цифр нужно
	if (c)
  {
		s[i++] = '-';           // отображаем знак
  }
	s[i--] = '\0';					  // предпоследний символ
	for (j = 0; i > j; j++, i--) 	// переворачиваем строку s на месте
	{
		c = s[j];
		s[j] = s[i];
		s[i] = c;
	}
}


/*******************************************************************************
* Преобразование стоки чисел в число
* Вход - строка до 3-х символов
* Выход - число 0-255. 
* При переполнении возвращает 0
*******************************************************************************/
inline u8 atou8 (char *str)
{
	u16 summ = 0;
  u8 count = 1;
  if (*str)
  {
    summ = *str - '0';
    str++;
  }
	while (*str && (count < 3))
	{
		summ = summ * 10 + (*str - '0');
		str++;
    count++;
	}
	if (summ > 255)
	{
		summ = 0;
	}
	return (u8)summ;
}


/*******************************************************************************
* очистить окно
*
*******************************************************************************/
void lcd_clear_window (void)
{
	u16 temp = lcd.front;
	lcd.front = lcd.back;
	lcd_rect ();
	lcd.front = temp;
}



/*******************************************************************************
* нарисовать рамку по границам окна
* lcd.border = 0, 1, 2
*******************************************************************************/
void lcd_border_window (void)
{	
  s16 x = lcd.x;
  s16 y = lcd.y;
  s16 w = lcd.width;
  s16 h = lcd.height;
  s16 b = lcd.border;
  
  if (lcd.border)
  {
    lcd.height = b;// верхняя граница
    lcd_rect ();
    
    lcd.y = y + h - b;  // нижняя граница
    lcd_rect ();
    
    lcd.y = y;  // левая граница
    lcd.width = b;
    lcd.height = h;
    lcd_rect ();
    
    lcd.x = x + w - b;  // правая граница
    lcd_rect ();

    lcd.x = x;  // возврат значений
    lcd.y = y;
    lcd.width = w;
    lcd.height = h;
    lcd.border = b;
  }
}

/*******************************************************************************
* очистить строку в текущем окне
*******************************************************************************/
void lcd_clear_row (void)
{
	// высота строки из текущего шрифта
	tChar		*tch;
	
	tch = lcd_find_char_by_code ('0', lcd.font);
  lcd.x = 0;
	lcd.height = tch->image->height;
	lcd.width = lcd.x_max;
	lcd_clear_window ();
}

void lcd_draw_number (u32 num, u8 len)
{
  char n [9];
  char *s = n + len;
  *s = 0;           // EOL
  do 
  {
    *(--s) = (char)(num % 10 + '0');
    num = num / 10;
  } 
  while (num > 0);
  lcd_draw_string (s);
}

void lcd_draw_bcd (u8 num) 
{
  char n[3];
  char *s = n + 2;
  *s = 0;           // EOL
  *(--s) = (num & 0x0F) + '0';
  *(--s) = (num >> 4) + '0';
  lcd_draw_string (s);
}



/*******************************************************************************
* 
*******************************************************************************/
void rgb_2_hsv   (rgb_t *rgb, hsv_t *hsv)
{
  u8 min = rgb_min (rgb);
  u8 max = rgb_max (rgb);
  s16 delta = max - min;
  s16 r = rgb->r;
  s16 g = rgb->g;
  s16 b = rgb->b;
  hsv->v = max;
  if (max)
  {
    hsv->s = (delta * 255) / max;
  }
  else  // r=g=b=0
  {
    hsv->s = 0;
    hsv->h = 0;
  }
  if (rgb->r == max)
  {
    hsv->h = (60 * (g - b)) / delta;
  }
  else if (rgb->g == max)
  {
    hsv->h = 120 + (60 * (b - r)) / delta;
  }
  else
  {
    hsv->h = 240 + (60 * (r - g)) / delta;
  }
  if (hsv->h < 0)
  {
    hsv->h += 360;
  }
}

/*******************************************************************************
*
*******************************************************************************/
void hsv_2_rgb (hsv_t *hsv, rgb_t *rgb)
{
  s32 h = hsv->h;
  u32 s = hsv->s;
  u32 v = hsv->v;
  u8 i;
  u8 a, b, c;
  u32 dh;
  
  if (s == 0)
  {
    rgb->r = v;
    rgb->g = v;
    rgb->b = v;
    return;
  }
  if (h < 0)
  {
    h = h + 360 + (360 * (-h % 360));
  }
  if (h >= 360)
  {
    h = h % 360;
  }
  i = h / 60;
  dh = (255 * (h % 60)) / 60;  // 0..255
  a = (v * (dh + (((255 - dh) * (255 - s)) / 255))) / 255;
  b = (v * (255 - s)) / 255;
  c = (v * ((255 - dh) + (dh * (255 - s) / 255))) / 255;
  
  switch (i)
  {
  case 0:
    rgb->r = v;
    rgb->g = a;
    rgb->b = b;
    break;
  case 1:
    rgb->r = c;
    rgb->g = v;
    rgb->b = b;
    break;
  case 2:
    rgb->r = b;
    rgb->g = v;
    rgb->b = a;
    break;
  case 3:
    rgb->r = b;
    rgb->g = c;
    rgb->b = v;
    break;
  case 4:
    rgb->r = a;
    rgb->g = b;
    rgb->b = v;
    break;
  case 5:
  default:
    rgb->r = v;
    rgb->g = b;
    rgb->b = c;
    break;  
  }
}

/*******************************************************************************
* вычисление яркости пикселя
* вход - адрес пикселя
* выход - яркость y = 0.3R + 0.59G + 0.11B (max 24 bit 0xFFFFFF = 16777215)
*******************************************************************************/
s32 lightness (rgb_t *pixel)
{
  const s32 r_weight = 5033164;
  const s32 g_weight = 9898557;
  const s32 b_weight = 1845494;
  s32 l = r_weight * pixel->r + g_weight * pixel->g + b_weight * pixel->b;
  return l;
}


       
u8 rgb_min (rgb_t *rgb)
{
  if (rgb->r < rgb->g)
  {
    if (rgb->r < rgb->b)
    {
      return (rgb->r);
    }
    return (rgb->b);
  }
  else
  {
    if (rgb->g < rgb->b)
    {
      return (rgb->g);
    }
    return (rgb->b);
  }
}

u8 rgb_max (rgb_t *rgb)
{
  if (rgb->r >= rgb->g)
  {
    if (rgb->r >= rgb->b)
    {
      return (rgb->r);
    }
    return (rgb->b);
  }
  else
  {
    if (rgb->g >= rgb->b)
    {
      return (rgb->g);
    }
    return (rgb->b);
  }
}


/*******************************************************************************
* печать числа с фиксированной точкой в десятичном формате
* buf - выводимая строка
* val - число
* mult - число разрядов после запятой
* digits - количество выводимых символов 
* возвращает число добавленных символов
*******************************************************************************/
u8 itos_fix_dig (s32 val, char *buf, u8 mult, u8 digits)
{
  //char temp[16];
  s16 len;
  
  itoa (val, buf);
  len = strlen (buf);
  if ((len - digits) < mult)
  {
    digits++; // если точка попадает в вывод, увеличиваем длину копируемого сегмента
    buf[len + 1] = 0;
    while (mult--)
    {
      buf[len] = buf[len - 1];
      len--;
    }
    buf[len] = '.';
  }
  return digits;
}

void palette_to_str (palettes palette, char *buf)
{
  static const char palette_names[5][14] =
  {
    "Hot Iron     \0",
    "Rainbow      \0",
    "Black-White  \0",
    "White-Black  \0",
    "Blue-Red     \0"
  };
  u8 palette_index = (u8)palette;
  char const *palette_str = palette_names[palette_index];
  strcpy (buf, palette_str);
}


///*******************************************************************************
//* добавление строки row2 в конец row1
//*******************************************************************************/
//char *add_row (char *row1, char *row2, u8 len = 0)
//{
//  
//}


