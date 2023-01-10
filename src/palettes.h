/******************************************************************************
* Файл palettes.h
* Определения функций палитр
******************************************************************************/

#ifndef PALETTES_H
#define PALETTES_H



typedef enum _palettes
{
  PAL_IRON = 0,
  PAL_RAINBOW,
  PAL_BW,
  PAL_BW_INVERCE,
  PAL_BR
} palettes;


u16 get_color (s16 t);


#endif