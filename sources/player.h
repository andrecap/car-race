/*
 * player.h
 *
 *  Created on: 06/dic/2015
 *      Author: root
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "types.h"

void manage_menu(screen_t* my_screen, char* str, BITMAP* bg_buff);
void manage_arrows(screen_t* my_screen, double* steady_vel, double* steady_om);
void manage_enter(screen_t* my_screen, pthread_t** c_idp, task_par_t** c_taskpar);
void delete_threads(screen_t* my_screen);


#endif /* PLAYER_H_ */
