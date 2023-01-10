/******************************************************************************
* Файл palettes.c
* Функции палитр
******************************************************************************/

#include  "global.h"

u16 get_color_iron      (s16 t);
u16 get_color_rainbow   (s16 t);
u16 get_color_bw        (s16 t);
u16 get_color_bw_inverce  (s16 t);
u16 get_color_br        (s16 t);
u16 get_part_color      (hsv_t start, hsv_t end, u8 part);


u16 get_color (s16 t)
{
  switch (g.disp.palette)
  {
  case PAL_IRON:
    return (get_color_iron (t));
    break;
    
  case PAL_RAINBOW:
    return (get_color_rainbow (t));
    break;
    
  case PAL_BW:
    return (get_color_bw (t));
    break;
    
  case PAL_BW_INVERCE:
    return (get_color_bw_inverce (t));
    break;
    
  case PAL_BR:
    return (get_color_br (t));
    break;
    
  default:
    return (get_color_iron (t));
    break;
  } 
}


u16 get_color_iron (s16 t)
{
  const hsv_t iron_pal[] = 
  {
    HSV_BLACK, 
    //HSV_VIOLET, 
    HSV_BLUE, 
    HSV_YELLOW,
    HSV_ORANGE,
    HSV_RED,
    HSV_GOLD, 
    HSV_WHITE
  };
  if (t > g.disp.max_t_setup) t = g.disp.max_t_setup;
  if (t < g.disp.min_t_setup) t = g.disp.min_t_setup;
  t -= g.disp.min_t_setup;
  const u8 ranges = sizeof(iron_pal) / sizeof (iron_pal[0]) - 1;
  s16 part_size = (g.disp.range / ranges) + 1;
  u8 n = t / part_size;
  u8 part = (u8)(((s32)(t % part_size) * 255) / part_size);
  
  hsv_t start = iron_pal[n];
  hsv_t end = iron_pal[n + 1];
  
  return (get_part_color (start, end, part));
}

u16 get_color_rainbow (s16 t)
{
  const hsv_t rainbow_pal[] = 
  {
    HSV_DARK_VIOLET, 
    //HSV_VIOLET,
    HSV_BLUE,
    HSV_GREEN,
    HSV_YELLOW,
    HSV_RED
  };
  if (t > g.disp.max_t_setup) t = g.disp.max_t_setup;
  if (t < g.disp.min_t_setup) t = g.disp.min_t_setup;
  t -= g.disp.min_t_setup;
  const u8 ranges = sizeof(rainbow_pal) / sizeof (rainbow_pal[0]) - 1;
  s16 part_size = g.disp.range / ranges;
  u8 n = t / part_size;
  u8 part = (u8)(((s32)(t % (part_size + 1)) * 255) / part_size);
  
  hsv_t start = rainbow_pal[n];
  hsv_t end = rainbow_pal[n + 1];
  
  return (get_part_color (start, end, part));
}

u16 get_color_bw (s16 t)
{
  if (t > g.disp.max_t_setup) t = g.disp.max_t_setup;
  if (t < g.disp.min_t_setup) t = g.disp.min_t_setup;
  t -= g.disp.min_t_setup;
  u8 part = (u8)((s32)255 * t / g.disp.range);

  hsv_t start = (hsv_t){HSV_BLACK};
  hsv_t end = (hsv_t){HSV_WHITE};
  
  return (get_part_color (start, end, part));
}

u16 get_color_bw_inverce (s16 t)
{
  if (t > g.disp.max_t_setup) t = g.disp.max_t_setup;
  if (t < g.disp.min_t_setup) t = g.disp.min_t_setup;
  t -= g.disp.min_t_setup;
  u8 part = (u8)((s32)255 * t / g.disp.range);

  hsv_t start = (hsv_t){HSV_WHITE};
  hsv_t end = (hsv_t){HSV_BLACK};
  
  return (get_part_color (start, end, part));
}

u16 get_color_br (s16 t)
{
  if (t > g.disp.max_t_setup) t = g.disp.max_t_setup;
  if (t < g.disp.min_t_setup) t = g.disp.min_t_setup;
  t -= g.disp.min_t_setup;
  u8 part = (u8)((s32)255 * t / g.disp.range);

  hsv_t start = (hsv_t){HSV_BLUE};
  hsv_t end = (hsv_t){HSV_RED};
  
  return (get_part_color (start, end, part));
}

u16 get_part_color (hsv_t start, hsv_t end, u8 part)
{
  uRGB565 color;
  hsv_t hsv;
  rgb_t rgb;
  
  hsv.h = (((s16)end.h - start.h) * part / 255 + start.h);
  hsv.s = (u8)((s16)(end.s - start.s) * part / 255 + start.s);
  hsv.v = (u8)((s16)(end.v - start.v) * part / 255 + start.v);
  hsv_2_rgb (&hsv, &rgb);
  
  color.rgb_color.r = rgb.r >> 3;
  color.rgb_color.g = rgb.g >> 2;
  color.rgb_color.b = rgb.b >> 3;
  
  return (color.value);
}


