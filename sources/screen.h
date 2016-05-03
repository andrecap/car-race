/*
 * screen.h
 *
 *  Created on: 01/ott/2015
 *      Author: andrea
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include "types.h"

#define 	CAR_W 	4
#define 	CAR_H 	2
#define 	MAP_W 	800
#define 	MAP_H	600
#define 	UI_W	800
#define		UI_H	168
#define		TD_W	224
#define		TD_H	768

int init_screen(screen_t* scr, int width, int height, char* images_folder);
void select_scene(screen_t* my_screen, char* str, BITMAP* bg_buff);
void move_cursor_down(screen_t* my_screen);
void move_cursor_up(screen_t* my_screen);


#endif /* SCREEN_H_ */
