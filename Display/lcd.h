//----------------------------------------------------------------------------
// Файл lcd.h
// вывод информации на дисплей ST7789 по SPI
// определения внешних функций
//----------------------------------------------------------------------------

#ifndef	_LCD_H_
#define	_LCD_H_

#include	"lcd_types.h"


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
* Шрифты:
*     10 - Agency_18
*     11 - Agency_29
*     12 - Courier_18
*     13 - Courier_29
*     14 - Arial_18
*     15 - Arial_29
*     16 - 
*     17 - 
*     18 - 
*     19 - 
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



#define NOP         (0x00)    // No operation
#define SWRESET     (0x01)    // Software reset
#define RDDID       (0x04)    // Read display ID
#define RDDST       (0x09)    // Read display status
#define RDDPM       (0x0A)    // Read display power
#define RDD_MADCTL  (0x0B)    // Read display
#define RDD_COLMOD  (0x0C)    // Read display pixel
#define RDDIM       (0x0D)    // Read display image
#define RDDSM       (0x0E)    // Read display signal
#define RDDSDR      (0x0F)    // Read display self-diagnostic result
#define SLPIN       (0x10)    // Sleep in
#define SLPOUT      (0x11)    // Sleep out
#define PTLON       (0x12)    // Partial mode on
#define NORON       (0x13)    // Partial off (Normal)
#define INVOFF      (0x20)    // Display inversion off
#define INVON       (0x21)    // Display inversion on
#define GAMSET      (0x26)    // 
#define DISPOFF     (0x28)    // Display off
#define DISPON      (0x29)    // Display on
#define CASET       (0x2A)    // Column address set
#define RASET       (0x2B)    // Row address set
#define RAMWR       (0x2C)    // Memory write
#define RAMRD       (0x2E)    // Memory read
#define PTLAR       (0x30)    // Partial sart/end address set
#define VSCRDEF     (0x33)    // Vertical scrolling definition
#define TEOFF       (0x34)    // Tearing effect line off
#define TEON        (0x35)    // Tearing effect line on

#define MADCTL      (0x36)    // Memory data access control
#define	MADCTR_MY	  (0x80)	  // Row Address Order
#define	MADCTR_MX	  (0x40)	  // Column Address Order
#define	MADCTR_MV	  (0x20)	  // Row/Column Exchange
#define	MADCTR_ML	  (0x10)	  // Vertical Refresh Order
#define	MADCTR_RGB	(0x08)	  // RGB-BGR ORDER
#define	MADCTR_MH	  (0x04)	  // Horizontal Refresh LCD refresh Left to Right, when = 0

#define VSCRSADD    (0x37)    // Vertical scrolling start address
#define IDMOFF      (0x38)    // Idle mode off
#define IDMON       (0x39)    // Idle mode on
#define COLMOD      (0x3A)    // Interface pixel format
#define RAMWRC      (0x3C)    // Memory write continue
#define RAMRDC      (0x3E)    // Memory read continue
#define TESCAN      (0x44)    // Set tear scanline
#define RDTESCAN    (0x45)    // Get scanline
#define WRDISBV     (0x51)    // Write display brightness
#define RDDISBV     (0x52)    // Read display brightness value
#define WRCTRLD     (0x53)    // Write CTRL display
#define RDCTRLD     (0x54)    // Read CTRL value dsiplay
#define WRCACE      (0x55)    // Write content adaptive brightness control and Color enhancemnet
#define RDCABC      (0x56)    // Read content adaptive brightness control
#define WRCABCMB    (0x5E)    // Write CABC minimum brightness
#define RDCABCMB    (0x5F)    // Read CABC minimum brightness
#define RDID1       (0xDA)    // Read ID1
#define RDID2       (0xDB)    // Read ID2
#define RDID3       (0xDC)    // Read ID3

#define RAMCTRL     (0xB0)    // RAM Control
#define RGBCTRL     (0xB1)    // RGB Control
#define PORCTRL     (0xB2)    // Porch control
#define FRCTRL1     (0xB3)    // Frame Rate Control 1
#define GCTRL       (0xB7)    // Gate control
#define DGMEN       (0xBA)    // Digital Gamma Enable
#define VCOMS       (0xBB)    // VCOM Setting
#define LCMCTRL     (0xC0)    // LCM Control
#define IDSET       (0xC1)    // ID Setting
#define VDVVRHEN    (0xC2)    // VDV and VRH Command Enable
#define VRHS        (0xC3)    // VRH Set
#define VDVSET      (0xC4)    // VDV Setting
#define VCMOFSET    (0xC5)    // VCOM Offset Set
#define FRCTR2      (0xC6)    // FR Control 2
#define CABCCTRL    (0xC7)    // CABC Control
#define REGSEL1     (0xC8)    // Register value selection1
#define REGSEL2     (0xCA)    // Register value selection2
#define PWCTRL1     (0xD0)    // Power Control 1
#define VAPVANEN    (0xD2)    // Enable VAP/VAN signal output
#define PVGAMCTRL   (0xE0)    // Positive Voltage Gamma Control
#define NVGAMCTRL   (0xE1)    // Negative Voltage Gamma Control
#define DGMLUTR     (0xE2)    // Digital Gamma Look-up Table for Red
#define DGMLUTB     (0xE3)    // Digital Gamma Look-up Table for Blue
#define GATECTRL    (0xE4)    // Gate control
#define PWCTRL2     (0xE8)    // Power Control 2
#define EQCTRL      (0xE9)    // Equalize Time Control
#define PROMCTRL    (0xEC)    // Program Control
#define PROMEN      (0xFA)    // Program Mode Enable
#define NVMSET      (0xFC)    // NVM Setting
#define PROMACT     (0xFE)    // Program Action

#define TFTLCD_DELAY 0xFF

extern lcd_param	lcd;	// структура параметров дисплея 

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
*   цвет: 0-Black	1-Red	2-Green	3-Yellow	4-Blue	5-Magenta	6-Cyan	7-White
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

// Функции библиотеки lcd
void  lcd_set_orient				    (lcd_orient	orient);
u32	  lcd_set_window				    (void);
void  lcd_clear_rect		        (void);
void  lcd_clear					        (void);
void  lcd_create_window         (void);
void  lcd_close_window          (void);
void	lcd_border_window         (void);
void	lcd_clear_window          (void);
void	lcd_clear_row			        (void);

void  lcd_draw_symbol_mono		  (void);
void  lcd_draw_symbol_mono_rle	(void);
void  lcd_pixel					        (void);
void  lcd_rect					        (void);
void  lcd_line					        (s16 x1, s16 y1, s16 x2, s16 y2);
void  lcd_circle					      (s16 x, s16 y, s16 radius);
void  lcd_draw_string				    (const char *str);
void  lcd_draw_number           (u32 num, u8 len);
void  lcd_draw_bcd              (u8 num);

void  lcd_func					        (void);
void  lcd_print					        (const char *str);
void  itoa                      (s32 n, char *s);
void  itoan						          (s32 n, char *s, u8 num);
u8    itos_fix_dig              (s32 val, char *buf, u8 mult, u8 digits);
void  palette_to_str            (palettes palette, char *buf);

void  rgb_2_hsv                 (rgb_t *rgb, hsv_t *hsv);
void  hsv_2_rgb                 (hsv_t *hsv, rgb_t *rgb);
s32   lightness                 (rgb_t *pixel);   // вычисление яркости RGB пикселя
u8    rgb_min                   (rgb_t *rgb);
u8    rgb_max                   (rgb_t *rgb);


void  lcd_task      (void *param);
void  display_test  (void);

#endif

