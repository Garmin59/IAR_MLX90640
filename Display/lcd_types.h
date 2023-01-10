//-----------------------------------------------------------------------------
// файл lcd_types.h
// определения типов данных
//-----------------------------------------------------------------------------

#ifndef _LCD_TYPES_H_
#define _LCD_TYPES_H_


#include	"global.h"


/* Подключение дисплея к процессору:*/


typedef struct
{
  u8 r; // 0..255
  u8 g; // 0..255
  u8 b; // 0..255
} rgb_t;

typedef struct
{
  s16 h;  // -360..360
  u8  s;  // 0..255
  u8  v;  // 0.255
} hsv_t;



/**** В этом блоке настриваются константы *************************************/

// HSV defined            h     s     v
#define HSV_WHITE         0,    0,    255
#define HSV_LIGHT_GREY    0,    0,    200

#define HSV_RED           0,    255,  255
#define HSV_ROSE          0,    200,  255
#define HSV_GOLD          20,   180,  255
#define HSV_ORANGE        30,   255,  255
#define HSV_YELLOW        40,   255,  255
#define HSV_LIME          90,   255,  255
#define HSV_GREEN         120,  255,  255
#define HSV_CIAN          150,  255,  255
#define HSV_OCEAN         180,  255,  255
#define HSV_BLUE          240,  255,  255
#define HSV_VIOLET        300,  255,  255
#define HSV_VIOLET_RED    359,  255,  255

#define HSV_GREY          0,    0,    63 

#define HSV_DARK_RED      0  ,  255,  63 
#define HSV_DARK_ROSE     20 ,  255,  63 
#define HSV_DARK_ORANGE   40 ,  255,  63 
#define HSV_DARK_YELLOW   60 ,  255,  63 
#define HSV_DARK_LIME     90 ,  255,  63 
#define HSV_DARK_GREEN    120,  255,  63 
#define HSV_DARK_CIAN     150,  255,  63 
#define HSV_DARK_OCEAN    180,  255,  63 
#define HSV_DARK_BLUE     240,  255,  63 
#define HSV_DARK_VIOLET   300,  255,  63 
#define HSV_DARK_GREY     0  ,  0  ,  31 

#define HSV_BLACK         0  ,  0  ,  0  

// Определения цветов
#define	RGB(r,g,b)	(u16)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))

#define	RGB_WHITE		        RGB (255, 255 , 255 )
#define	RGB_LIGHT_GRAY	    RGB (192, 192 , 192 )
#define	RGB_GRAY		        RGB (100, 100 , 100 )
#define	RGB_DARKGRAY	      RGB (64 , 64  , 64  )
#define	RGB_BLACK		        RGB (0  , 0   , 0   )

#define	RGB_RED			        RGB (170, 0   , 0   )
#define	RGB_GREEN		        RGB (0  , 170 , 0   )
#define	RGB_BROWN		        RGB (170, 85  , 0   )
#define	RGB_BLUE		        RGB (0  , 0   , 170 )
#define	RGB_DARKBLUE	      RGB (0  , 0   , 120 )
#define	RGB_MAGENTA	        RGB (170, 0   , 170 )
#define	RGB_CYAN		        RGB (0  , 170 , 170 )

#define	RGB_BRIGHT_RED		  RGB (255, 0   , 0   )
#define	RGB_BRIGHT_GREEN	  RGB (0  , 255 , 0   )
#define	RGB_YELLOW			    RGB (255, 255 , 0   )
#define	RGB_BRIGHT_BLUE		  RGB (0  , 0   , 255 )
#define	RGB_BRIGHT_MAGENTA	RGB (255, 0   , 255 )
#define	RGB_BRIGHT_CYAN		  RGB (0  , 255 , 255 )

#define	RGB_ORANGE	        RGB (255, 200 , 85  )
#define	RGB_L_GREEN	        RGB (170, 255 , 85  )
#define	RGB_VIOLET	        RGB (170, 0   , 255 )
#define	RGB_PINK		        RGB (255, 150 , 150 )
#define	RGB_AQUA		        RGB (51 , 153 , 255 )



// ST7789 параметры
#define ST7789_OFFSET_X_PORTRAIT	0
#define ST7789_OFFSET_Y_PORTRAIT	0
#define ST7789_OFFSET_X_ALBUM	0
#define ST7789_OFFSET_Y_ALBUM	0
#define ST7789_OFFSET_X_REV_PORTRAIT	0
#define ST7789_OFFSET_Y_REV_PORTRAIT	80
#define ST7789_OFFSET_X_REV_ALBUM	80
#define ST7789_OFFSET_Y_REV_ALBUM	0

#define ST7789_XMAX		240
#define ST7789_YMAX	  240

#define STRBUF_SIZE   256


#define ESC		0x1B
#define CSI		0x1B, '['


typedef enum _lcd_orient
{
	PORTRAIT = 0,	// экран разъёмом внизу
	ALBUM,			// разъём слева
	REV_PORTRAIT,	// разъём снизу
	REV_ALBUM		// разъём справа
} lcd_orient;

typedef enum _tft_command
{
	C_LIGHT = 0, 
	C_COLOR, 
	C_BACK, 
	C_FONT, 
	C_ORIENT, 
	C_RECT, 
	C_CURSOR, 
	C_LINE, 
	C_CIRCLE
} tft_command; // команды дисплея




typedef struct _tImage
{
  const u8 	*data;
  const u8 	width;
  const u8 	height;
} tImage;


typedef struct _tChar
{
  const u8 		  code;
  const tImage * const image;
} tChar;

typedef struct
{
  const u8 		  length;
  const tChar   *chars;
} tFont;


typedef struct _rgb
{
	u8 	r;
	u8 	g;
	u8 	b;
} rgb;

typedef union _uRGB565
{
	u16 value;
	struct
	{
		u16 b	:5;
		u16 g	:6;
		u16 r	:5;
	} rgb_color;
} uRGB565;

typedef enum _win_modes
{
  LCD_SCREEN = 0,
  LCD_WINDOW
} win_modes;

	

typedef struct _lcd_param
{
	s16           x_offset;	// сдвиг по x
	s16           y_offset;	// сдвиг по y
	s16           x_max;		// ширина экрана
	s16           y_max;		// высота экрана
	lcd_orient		orient;		// ориентация дисплея
	s16           x;			  // текущая координата по x (координаты логические)
	s16           y;			  // текущая координата по Y (координаты логические)
	s16           width;		// ширина символа
	s16           height;		// высота символа
  win_modes     win_mode;   // режим работы дисплея (полноформатный/окно)
	s16           win_x;      // начало окна по x (координаты логические)
	s16           win_y;      // начало окна по Y (координаты логические)
	s16           win_width;	// ширина окна
	s16           win_height;	// высота окна
	s16           cursor_on;	// включить / выключить курсор
	u16           back;		    // цвет фона RGB 565		
	u16           front;		  // цвет фигур RGB 565
	u16           text_back;	// цвет фона текста RGB 565		
	u16           text_front;	// цвет текста RGB 565
	s16           border;		  // ширина границы в пикселях
	const tFont   *font;		// указатель на структуру текущего шрифта
	const tImage  *image;		// указатель на выводимую картинку
//	char 	        *p_str;		// указатель на строку для вывода на экран
//  ring_buffer_t buffer;
//  char          buffer_data[STRBUF_SIZE];  // внутренний буфер строки

} lcd_param;


typedef struct _lcd_status
{
  u8	id[3];  // display ID
  u8	st[4];  // display status
  u8	pm;
  u8	madctr;
  u8	colmod;
  u8	im;
  u8	sm;
  u8	sfr;
} lcd_status;








#endif	// _LCD_TYPES_H_

