//----------------------------------------------------------------------------
// Файл menu.c
// главная программа работы с меню
//----------------------------------------------------------------------------


#include	"global.h"


#define MENU_CLOSE_TIME     (30000) // время ожидания для закрытия меню

const menu_t NULL_MENU = {(void *)0, (void *)0, (void *)0, NULL_F, NULL_F, NULL_F, NULL_TXT};

// локальные функции меню
void  draw_borders    (void); // прорисовка контуров меню
void  print_titles    (void); // отображение заголовков меню
void  print_palette   (void); // отображение названия палитры
void  print_emission  (void); // отображение эмиссионной способности материала

void  pal_chg     (void);
void  emiss_chg   (void);
void  close       (void);
void  switch_off  (void);
void  print_tmax  (void);
void  tmax_chg    (void);
void  tmax_auto   (void);
void  print_tmin  (void);
void  tmin_chg    (void);
void  tmin_auto   (void);

char  txt_pal[]       = {"Palette       "};
char  txt_emiss[]     = {"Emission      "};
char  txt_tmax[]      = {"Tmax set      "};
char  txt_tmax_man[]  = {"Tmax manual   "};
char  txt_tmax_auto[] = {"Tmax auto     "};
char  txt_tmin[]      = {"Tmin set      "};
char  txt_tmin_man[]  = {"Tmin manual   "};
char  txt_tmin_auto[] = {"Tmin auto     "};
char  txt_close[]     = {"Close menu    "};
char  txt_off[]       = {"Power Off     "};

//             имя меню     ---------- пункты меню -------------  ------------- функции ---------------   - текст меню -
//                          prev        next        ent           enc         enter       title
MAKE_MENU     (m_palette    ,m_off      ,m_emiss    ,sub_palette  ,NULL_F     ,NULL_F     ,print_titles   ,txt_pal        );  // установить палитру отображения
  MAKE_MENU   (sub_palette  ,m_palette  ,NULL_MENU  ,m_palette    ,pal_chg    ,NULL_F     ,print_palette  ,NULL_TXT       );  // подменю палитры отображения
MAKE_MENU     (m_emiss      ,m_palette  ,m_tmax     ,sub_emiss    ,NULL_F     ,NULL_F     ,print_titles   ,txt_emiss      );  // установить отражательную способность
  MAKE_MENU   (sub_emiss    ,NULL_MENU  ,NULL_MENU  ,m_emiss      ,emiss_chg  ,NULL_F     ,print_emission ,NULL_TXT       );  // подменю отражательной способность
MAKE_MENU     (m_tmax       ,m_emiss    ,m_tmin     ,sub_tmax     ,NULL_F     ,NULL_F     ,print_titles   ,txt_tmax       );  // установить максимальную температуру
  MAKE_MENU   (sub_tmax     ,m_tmax     ,auto_tmax  ,set_tmax     ,NULL_F     ,NULL_F     ,print_titles   ,txt_tmax_man   );  // установить максимальную температуру
    MAKE_MENU (set_tmax     ,NULL_MENU  ,NULL_MENU  ,sub_tmax     ,tmax_chg   ,NULL_F     ,print_tmax     ,NULL_TXT       );  // изменить максимальную температуру
  MAKE_MENU   (auto_tmax    ,m_tmax     ,sub_tmax   ,NULL_MENU    ,NULL_F     ,tmax_auto  ,print_titles   ,txt_tmax_auto  );  // установить авто макс температуру
MAKE_MENU     (m_tmin       ,m_tmax     ,m_close    ,sub_tmin     ,NULL_F     ,NULL_F     ,print_titles   ,txt_tmin       );  // установить минимальную температуру
  MAKE_MENU   (sub_tmin     ,m_tmin     ,auto_tmin  ,set_tmin     ,NULL_F     ,NULL_F     ,print_titles   ,txt_tmin_man   );  // установить минимальную температуру
    MAKE_MENU (set_tmin     ,NULL_MENU  ,NULL_MENU  ,sub_tmin     ,tmin_chg   ,NULL_F     ,print_tmin     ,NULL_TXT       );  // изменить минимальную температуру
  MAKE_MENU   (auto_tmin    ,m_tmin     ,sub_tmin   ,NULL_MENU    ,NULL_F     ,tmax_auto  ,print_titles   ,txt_tmin_auto  );  // установить авто мин температуру
MAKE_MENU     (m_close      ,m_tmin     ,m_off      ,NULL_MENU    ,NULL_F     ,close      ,print_titles   ,txt_close      );  // выйти из меню
MAKE_MENU     (m_off        ,m_close    ,m_palette  ,NULL_MENU    ,NULL_F     ,switch_off ,print_titles   ,txt_off        );	// выключить питание вручную

static volatile menu_t const   *menu       = &m_palette;  // текущий пункт меню
//static volatile menu_t const   *n_menu     = &m_palette;  // новый пункт меню
static volatile menu_t const   *prev_menu     = &NULL_MENU;;    // предыдущий пункт меню
//__no_init static u32 menu_timer;				  // таймер меню


static bool menu_off = false;


void menu_task (void *param)
{
  (void)param;
  
  while (1)
  {
    vTaskDelay (10);
  }
  
}


/*******************************************************************************
* Отображение меню
* После нажатия кнопки энкодера
*******************************************************************************/
void menu_func (void)
{
  typedef enum _menu_modes
  {
    MENU_IDLE = 0,
    MENU_WORK
  } menu_modes;
  static menu_modes mode = MENU_IDLE;
  static u32 menu_mode_timer;
  
  switch (mode)
  {
  case MENU_IDLE:
    if (xEventGroupClearBits (g.events, M_SHORT_PRESS) == M_SHORT_PRESS)
    {
      menu = &m_palette;
      prev_menu = &NULL_MENU;
      xEventGroupSetBits (g.events, M_DISP_OFF);  // не отображать картинку
      mode = MENU_WORK;
    }
    break;
    
  case MENU_WORK:
    // отображаем меню
    if (prev_menu != menu)
    {
      if (menu->title != NULL_F)
      {
        menu->title ();
      }
      prev_menu = menu;
      timer_reset (&menu_mode_timer);
    }
    if (xEventGroupClearBits (g.events, M_ENC) == M_ENC)  // перемещение по пунктам меню
    {
      if (menu->enc)  // если есть спец ф-я энкодера
      {
        menu->enc ();
      }
      else            // иначе - переход по пунктам меню
      {
        if (g.enc > 0 && (menu->next != &NULL_MENU))
        {
          menu = (menu_t *)menu->next;
        }
        else if (g.enc < 0 && (menu->prev != &NULL_MENU))
        {
          menu = (menu_t *)menu->prev;
        }
        g.enc = 0;
      }
      timer_reset (&menu_mode_timer);
    }
    if (xEventGroupClearBits (g.events, M_SHORT_PRESS) == M_SHORT_PRESS)  // короткое нажатие
    {
      if (menu->ent != &NULL_MENU)
      {
        menu = menu->ent; // вход в подменю
      }
      else if (menu->enter) // если есть ф-я непосредственного управления
      {
        menu->enter ();  // выполняем пункт меню
      }
      timer_reset (&menu_mode_timer);
    }
    if (xEventGroupClearBits (g.events, M_LONG_PRESS) == M_LONG_PRESS
        || timer_end (&menu_mode_timer, MENU_CLOSE_TIME)
        || menu_off)  // выйти из меню
    {
      menu_off = false;
      xEventGroupSetBits (g.events, M_DISP_ON);  // отображать картинку
      lcd.back = RGB_BLACK;
      lcd_clear ();
      mode = MENU_IDLE;     
    }
    break;
    
  default:
    mode = MENU_IDLE;
    break;
  }
}


/*******************************************************************************
* Прорисовка  меню
*******************************************************************************/
// Контуры окон меню
void  draw_borders  (void)
{
  lcd.x = 20;
  lcd.y = 35;
  lcd.width = 200;
  lcd.height = 24;
  lcd.border = 2;
  lcd.back = RGB_BLUE;
  lcd.front = RGB_ORANGE;
  lcd_clear_rect ();
  
  lcd.y = 102;
  lcd_clear_rect ();
  
  lcd.x = 4;
  lcd.y = 63;
  lcd.width = 232;
  lcd.height = 35;
  lcd_clear_rect ();
  lcd.border = 0;
}


/*******************************************************************************
* Печать пунктов меню
*******************************************************************************/
void  print_titles  (void)
{
  static const char prev_menu_prep[] = "~38;28H~10m~33;1m~44;0m";
  static const char current_menu_prep[] = "~66;15H~11m";
  static const char next_menu_prep[] = "~105;28H~10m";
  static const char exit_menu_prep[] = "~37;1m~40;0m";
  
  menu_t const *p_menu;
  
  draw_borders ();
  
  p_menu = (menu_t const *)menu->prev;
  if (p_menu->txt)
  {
    lcd_print (prev_menu_prep);
    lcd_print (p_menu->txt);
  }
  if (menu->txt)
  {
    lcd_print (current_menu_prep);
    lcd_print (menu->txt);
  }
  p_menu = (menu_t const *)menu->next;
  if (p_menu->txt)
  {
    lcd_print (next_menu_prep);
    lcd_print (p_menu->txt);
  }
  lcd_print (exit_menu_prep);
}


void pal_chg (void)
{
  if (g.enc > 0)
  {
    if (g.disp.palette == PAL_BR)
    {
      g.disp.palette = PAL_IRON;
    }    
    else
    {
      g.disp.palette++;
    }
  }
  else if (g.enc < 0)
  {
    if (g.disp.palette == PAL_IRON)
    {
      g.disp.palette = PAL_BR;
    }    
    else
    {
      g.disp.palette--;
    }
  }
  prev_menu = &NULL_MENU; // для печати нового состояния
  g.enc = 0;
}

/*
* Печать названия палитры
*/
void print_palette (void)
{
  lcd_print ("~66;15H~11m~44;0m");
  palette_to_str (g.disp.palette, g.txt);
  lcd_print (g.txt);
}

void  print_tmax  (void)
{
  s16 temperature;
  
  lcd_print ("~66;15H~11m~44;0mTmax = ");
  if (g.disp.max_t_auto)
  {
    temperature = g.disp.max_t;
  }
  else
  {
    temperature = g.disp.max_t_setup;
  }
  if (temperature >= 1000)
  {
    itos_fix_dig (temperature, g.txt, 1, 4);
  }
  else
  {
    itos_fix_dig (temperature, g.txt, 1, 3);
  }
  lcd_print (g.txt);
  lcd_print ("C   ");
}

void  tmax_chg    (void)
{
  if (g.disp.max_t_auto)
  {
    g.disp.max_t_auto = false;
  }
  if (g.enc > 0)
  {
    g.disp.max_t_setup += 10;
    if (g.disp.max_t_setup > g.disp.max_t)
    {
      g.disp.max_t_setup = g.disp.max_t;
    }    
  }
  else if (g.enc < 0)
  {
    g.disp.max_t_setup -= 10;
    if (g.disp.max_t_setup < g.disp.min_t_setup)
    {
      g.disp.max_t_setup = g.disp.min_t_setup + 10;
    }    
  }
  prev_menu = &NULL_MENU; // для печати нового состояния
  g.enc = 0;
}

void  tmax_auto   (void)
{
  g.disp.max_t_auto = true;
}

void  print_tmin  (void)
{
  s16 temperature;
  
  lcd_print ("~66;15H~11m~44;0mTmin = ");
  if (g.disp.min_t_auto)
  {
    temperature = g.disp.min_t;
  }
  else
  {
    temperature = g.disp.min_t_setup;
  }
  if (temperature >= 1000)
  {
    itos_fix_dig (temperature, g.txt, 1, 4);
  }
  else
  {
    itos_fix_dig (temperature, g.txt, 1, 3);
  }
  lcd_print (g.txt);
  lcd_print ("C   ");
}

void  tmin_chg    (void)
{
  if (g.disp.min_t_auto)
  {
    g.disp.min_t_auto = false;
  }
  if (g.enc > 0)
  {
    g.disp.min_t_setup += 10;
    if (g.disp.min_t_setup > g.disp.max_t_setup)
    {
      g.disp.min_t_setup = g.disp.max_t_setup - 10;
    }    
  }
  else if (g.enc < 0)
  {
    g.disp.min_t_setup -= 10;
    if (g.disp.min_t_setup < g.disp.min_t)
    {
      g.disp.min_t_setup = g.disp.min_t;
    }    
  }
  prev_menu = &NULL_MENU; // для печати нового состояния
  g.enc = 0;
}

void  tmin_auto   (void)
{
  g.disp.min_t_auto = true;
}

void emiss_chg (void)
{
   if (g.enc > 0)
  {
    g.par.emissivity += 0.05;
    if (g.par.emissivity > 1.0f)
    {
      g.par.emissivity = 1.0f;
    }    
  }
  else if (g.enc < 0)
  {
    g.par.emissivity -= 0.05;
    if (g.par.emissivity < 0.1)
    {
      g.par.emissivity = 0.1f;
    }    
  }
  prev_menu = &NULL_MENU; // для печати нового состояния
  g.enc = 0;
}

void print_emission (void)
{
  s16 em = (s16)(g.par.emissivity * 100.0 + 0.005);
  lcd_print ("~66;15H~11m~44;0mEmission = ");
  itos_fix_dig (em, g.txt, 2, 3);
  lcd_print (g.txt);
  lcd_print ("   ");
}

void close (void)
{
  menu_off = true;
}


void switch_off (void)
{
  xEventGroupSetBits (g.events, M_POWER_OFF);
}




