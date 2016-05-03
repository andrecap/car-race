/*
 * player.c
 *
 *  Created on: 06/dic/2015
 *      Author: root
 */

#include <allegro.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include "periodics.h"
#include "utility.h"
#include "generator.h"
#include "car_thread.h"
#include "player.h"
#include "screen.h"

extern pthread_attr_t		c_attr;

/*Handles Menu for player*/
void manage_menu(screen_t* my_screen, char* str, BITMAP* bg_buff){
	if(key[KEY_UP]){
		move_cursor_up(my_screen);
	}
	if(key[KEY_DOWN]){
		move_cursor_down(my_screen);
	}
	if(key[KEY_ENTER]){
		select_scene(my_screen, str, bg_buff);
		sem_post(&my_screen->sems.sync[0]);
		sem_wait(&my_screen->sems.sync[1]);
	}
}

/*Sets speed based on keys pressed*/
void manage_arrows(screen_t* my_screen, double* steady_vel, double* steady_om){
	if(key[KEY_UP]){
		*steady_vel = MAX_VEL;
		*steady_om = 0;
	}
	if(key[KEY_LEFT]){
		*steady_vel = MAX_VEL/2;
		*steady_om = -150;
	}
	if(key[KEY_RIGHT]){
		*steady_vel = MAX_VEL/2;
		*steady_om = 150;
	}
	if(key[KEY_DOWN]){
		*steady_vel = -MAX_VEL/2;
		*steady_om = 0;
	}
}

/*Creates new car thread if enter key pressed*/
void manage_enter(screen_t* my_screen, pthread_t** c_idp, task_par_t** c_taskpar){
	int loop = 1;
	int i;

	for(i = 0; i<CPUCARS; i++){
		if(!my_screen->vw.cpu[i].alive && loop){
			pthread_mutex_lock(&my_screen->sems.cpucar_mux[i]);
			pthread_create(c_idp[i], &c_attr, car_thread, c_taskpar[i]);
			pthread_mutex_unlock(&my_screen->sems.cpucar_mux[i]);
			loop = 0;
		}
	}
}

/*Deletes threads to close the program*/
void delete_threads(screen_t* my_screen){
	int i;
	pthread_cancel(my_screen->td.tids.g_id);
	pthread_cancel(my_screen->td.tids.phy_id);
	for(i = 0; i < CPUCARS; i++)
		if(my_screen->vw.cpu[i].alive)
			pthread_cancel(my_screen->td.tids.car_id[i]);
}
