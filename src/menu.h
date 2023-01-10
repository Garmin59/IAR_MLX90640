//----------------------------------------------------------------------------
// Файл menu.h
//----------------------------------------------------------------------------

#ifndef	_MENU_H_
#define	_MENU_H_

#define NULL_F      ((void*)0)
#define NULL_TXT    ("")
//#define NULL_MENU   (void*)0;

typedef void (*func)(void);

#define MAKE_MENU(name,prev,next,ent,enc,enter,title,txt) \
  extern const menu_t prev;\
  extern const menu_t next;\
  extern const menu_t ent;\
  const menu_t name = {(void*)&prev, (void*)&next, (void*)&ent,\
                      (func)enc, (func)enter, (func)title, (char *)txt}



typedef struct
{ 
  void    *prev;  // предыдущий пункт меню при повороте влево
  void    *next;  // следующий пункт меню при повороте вправо
  void    *ent;   // пункт меню при нажатии энкодера
  func    enc;    // ф-я при повороте энкодера
  func    enter;  // ф-я при нажатии
  func    title;  // ф-я строки заголовка
  char    *txt;   // текст строки заголовка
} menu_t;

void menu_task (void *param);
void menu_func (void);

#endif  // _MENU_H_
