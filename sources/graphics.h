/*
 * graphics.h
 *
 *  Created on: 15/ott/2015
 *      Author: andrea
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "types.h"

void *graphic_thread(void* pp);
int cord2px(double c);
int cary2px(double y);
int carx2px(double x);
short init_map(screen_t* my_screen);
void update_map(screen_t* my_screen);
void init_ui(screen_t* my_screen);
void update_ui(screen_t* my_screen);
void update_td(screen_t* my_screen);

#endif /* GRAPHICS_H_ */
