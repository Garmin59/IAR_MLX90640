/******************************************************************************
* Файл display.c
* функции отображения дисплея
* Работает с данными в структуре g
******************************************************************************/

#include  "global.h"



void  draw_ir_data        (u16 row);
void  draw_ir_data_no_int (u16 row);
void  draw_ir_data_int_1  (u16 row);
void  draw_ir_data_int_2  (u16 row);
void  find_ir_parameters  (void);
void  draw_texts          (void);
void  draw_cursors        (void);
u16   select_min_contrast (void);
u16   select_max_contrast (void);
void  draw_battery        (void);
u8    batt_level          (void);


typedef enum _display_stat
{
	DISP_WORK = 0,
	DISP_FREEZE
} display_stat;

  
/*******************************************************************************
*
*******************************************************************************/  
void disp_task (void *param)
{
  (void) param;
  
  static display_stat stat = DISP_WORK;
  u16 row = 0;
  EventBits_t bits;

  g.disp.chess = 1;
  g.disp.refresh_rate = 8;
  g.disp.interpolation = INTR_OFF;
  g.disp.palette = PAL_BW_INVERCE;
  g.disp.abs_t = true;
  g.disp.min_t_auto = true;
  g.disp.min_t_pointer = true;
  g.disp.max_t_auto = true;
  g.disp.max_t_pointer = true;
    
  //dma_init ();
  
  while (1)
  {
    
    bits = xEventGroupWaitBits (g.events, 
                               (M_IR_READY | M_DISP_ON | M_DISP_OFF), 
                               pdTRUE,        // xClearOnExit
                               pdFALSE,       // xWaitForAllBits
                               portMAX_DELAY);
      
    if ((bits & M_DISP_ON) == M_DISP_ON)
    {
      stat = DISP_WORK;
    }
    else if ((bits & M_DISP_OFF) == M_DISP_OFF)
    {
      stat = DISP_FREEZE;
    }
    
    switch (stat)
    {
    case DISP_WORK:
      {
        if ((bits & M_IR_READY) == M_IR_READY)
        {
          find_ir_parameters ();
          row = 0;
          while (row < 24)
          {
            draw_ir_data (row++);
            vTaskDelay (1);
          }
          draw_cursors ();
          vTaskDelay (1);
          
          draw_texts ();
          vTaskDelay (1);
          
          draw_battery ();
          vTaskDelay (1);
        }
      }
      break;
      
    case DISP_FREEZE:
      break;
      
    default:
      stat = DISP_WORK;
      break;
    }
  }
}
    





/*******************************************************************************
* найти мин, макс, центр температуру, 
* напряжение питания, заряд батареи
*******************************************************************************/
void find_ir_parameters (void)
{
  g.disp.min_t = g.to[0];
  g.disp.max_t = g.to[0];
  g.disp.center_t = (g.to[367] + g.to[368] + g.to[399] + g.to[400]) / 4;
  for (int i = 0; i < PIXEL_COUNT; i++)
  {
    if (g.disp.min_t > g.to[i])
    {
      g.disp.min_t = g.to[i];
      g.disp.min_t_x = i % 32;
      g.disp.min_t_y = 23 - (i / 32);
    }
    if (g.disp.max_t < g.to[i])
    {
      g.disp.max_t = g.to[i];
      g.disp.max_t_x = i % 32;
      g.disp.max_t_y = 23 - (i / 32);
    }
  }
  if (g.disp.min_t_auto)
  {
    g.disp.min_t_setup = g.disp.min_t;
  }
  if (g.disp.max_t_auto)
  {
    g.disp.max_t_setup = g.disp.max_t;
  }
  g.disp.range = g.disp.max_t_setup - g.disp.min_t_setup;
}

void draw_ir_data (u16 row)
{
  switch (g.disp.interpolation)
  {
  case INTR_OFF:
  default:
    draw_ir_data_no_int (row);
    break;
    
  case INTR_1:
    draw_ir_data_int_1 (row);
    break;
    
  case INTR_2:
    draw_ir_data_int_2 (row);
    break;
  }
}

/*******************************************************************************
* без интерполяции
*******************************************************************************/
void draw_ir_data_no_int (u16 row)
{
  u16 index;
  
  lcd.width = 7;
  lcd.height = 6;
  for (u16 col = 0; col < 32; col++)
  {
    index = (23 - row) * 32 + col;
    
    lcd.front = get_color (g.to[index]);
    lcd.x = 8 + col * 7;
    lcd.y = 8 + row * 6;
    lcd_rect ();
  }   
}


/*******************************************************************************
* линейная интерполяция
*******************************************************************************/
void draw_ir_data_int_1 (u16 row)  
{
  
}

/*******************************************************************************
* нелинейная интерполяция
*******************************************************************************/
void draw_ir_data_int_2 (u16 row)
{
  
}


/*******************************************************************************
* вывод текстовых полей на экран
*******************************************************************************/
void draw_texts (void)
{
  static char txt[127] = {0};
  
  lcd_print ("~10m~153;0H~37;1m~0KT max=");
  
  if (g.disp.max_t >= 1000)
  {
    itos_fix_dig (g.disp.max_t, txt, 1, 4);
  }
  else
  {
    itos_fix_dig (g.disp.max_t, txt, 1, 3);
  }
  lcd_print (txt);
  lcd_print ("C   ~153;120HBattery=");

  itos_fix_dig (g.v_battery, txt, 3, 2);
  lcd_print (txt);
  lcd_print ("V  ~182;0H~0KT min=");
  
  if (g.disp.min_t >= 1000)
  {
    itos_fix_dig (g.disp.min_t, txt, 1, 4);
    
  }
  else
  {
    itos_fix_dig (g.disp.min_t, txt, 1, 3);
  }
  lcd_print (txt);
  lcd_print ("C    ~182;120H");
  palette_to_str (g.disp.palette, txt);
  lcd_print (txt);
  lcd_print ("~211;0H~0KT center=");
  
  if (g.disp.center_t >= 1000)
  {
    itos_fix_dig (g.disp.center_t, txt, 1, 4);
    
  }
  else
  {
    itos_fix_dig (g.disp.center_t, txt, 1, 3);
  }
  lcd_print (txt);
  lcd_print ("C  ");
  
  // wait to print finish
}

void  draw_cursors (void)
{
  // курсор центра
  typedef struct 
  {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    u16 color;
    u16 stop;
  } lines_t;
  
  static const lines_t lines [] =
  {
    {117, 73, 1, 5, RGB_BLACK, 1},
    {113, 77, 4, 1, RGB_BLACK, 1},
    {122, 73, 1, 5, RGB_BLACK, 1},
    {123, 77, 4, 1, RGB_BLACK, 1},
    {117, 82, 1, 5, RGB_BLACK, 1},
    {113, 82, 4, 1, RGB_BLACK, 1},
    {122, 82, 1, 5, RGB_BLACK, 1},
    {123, 82, 4, 1, RGB_BLACK, 1},
    {116, 72, 1, 5, RGB_WHITE, 1},
    {112, 76, 4, 1, RGB_WHITE, 1},
    {123, 72, 1, 5, RGB_WHITE, 1},
    {124, 76, 4, 1, RGB_WHITE, 1},
    {116, 83, 1, 5, RGB_WHITE, 1},
    {112, 83, 4, 1, RGB_WHITE, 1},
    {123, 83, 1, 5, RGB_WHITE, 1},
    {124, 83, 4, 1, RGB_WHITE, 1},
    {124, 83, 4, 1, RGB_WHITE, 0},
  }; 
  
  lines_t const *p_lines = lines;
  while (p_lines->stop)
  {
    lcd.x = p_lines->x;
    lcd.y = p_lines->y;
    lcd.width = p_lines->w;
    lcd.height = p_lines->h;
    lcd.front = p_lines->color;
    lcd_rect ();
    p_lines++;
  }
  
  // курсор минимума
  s16 x1 = 8 + g.disp.min_t_x * 7;
  s16 y1 = 8 + g.disp.min_t_y * 6;
  s16 x2 = x1 + 5;
  s16 y2 = y1 + 5;
  lcd.front = select_min_contrast ();
  lcd_line (x1,   y1, x2,   y2); 
  lcd_line (x2,   y1, x1,   y2); 
  lcd_line (++x1, y1, ++x2, y2); 
  lcd_line (x2,   y1, x1,   y2);
  
  // курсор максимума
  x1 = 8 + g.disp.max_t_x * 7;
  y1 = 8 + g.disp.max_t_y * 6;
  x2 = x1 + 5;
  y2 = y1 + 5;
  lcd.front = select_max_contrast ();
  lcd_line (x1,   y1, x2,   y2); 
  lcd_line (x2,   y1, x1,   y2); 
  lcd_line (++x1, y1, ++x2, y2); 
  lcd_line (x2,   y1, x1,   y2);

}

u16 select_min_contrast (void)
{
  switch (g.disp.palette)
  {
  case PAL_IRON:
    return RGB_WHITE;
    break;
  case PAL_RAINBOW:
    return RGB_WHITE;
    break;
  case PAL_BW:
    return RGB_WHITE;
    break;
  case PAL_BW_INVERCE:
    return RGB_BLACK;
    break;
  case PAL_BR:
  default:
    return RGB_RED;
    break;
  }
}

u16 select_max_contrast (void)
{
  switch (g.disp.palette)
  {
  case PAL_IRON:
    return RGB_BLACK;
    break;
  case PAL_RAINBOW:
    return RGB_BLACK;
    break;
  case PAL_BW:
    return RGB_BLACK;
    break;
  case PAL_BW_INVERCE:
    return RGB_WHITE;
    break;
  case PAL_BR:
  default:
    return RGB_BLUE;
    break;
  }
}


void draw_battery (void)
{
  char batt[] = "0";
  // определение режима зарядки
  if (IS_CHARGE)
  {
    batt[0] += 5;
    lcd_print ("~16m~211;220H~31;1m"); // шрифт Battery красный
  }
  else
  {
    lcd_print ("~16m~211;220H~34;1m"); // шрифт Battery синий
  }
  batt[0] += batt_level ();
  lcd_print (batt);
  if (g.v_battery < 3000) // защита от переразряда
  {
    xEventGroupSetBits (g.events, M_POWER_OFF);
  }
}

u8 batt_level (void)
{
  if (g.v_battery > 4000)
  {
   return 4;
  }
  else if (g.v_battery > 3800)
  {
   return 3;
  }
  else if (g.v_battery > 3600)
  {
   return 2;
  }
  else if (g.v_battery > 3400)
  {
   return 1;
  }
  return 0;
}


