/*
 * graphic_thread.c

 *
 *  Created on: 06/dic/2015
 *      Author: root
 */
#include <stdio.h>
#include "graphic_thread.h"
#include "graphics.h"
#include "screen.h"
#include "periodics.h"
#include "graphics.h"
#include "utility.h"


void* graphic_thread(void* pp){
	printf("graphic thread born: %lu\n", (unsigned long)pthread_self());
	task_par_t* tp = (task_par_t*) pp;
	screen_t* my_screen = (screen_t*)tp->arg;
	short initialized = 0;
	short stat = 0;
	short sc = 0;

	set_period(tp);
	while(!key[KEY_ESC]){
		stat = get_status(my_screen);
		switch(stat){
		case MENU:
			pthread_mutex_lock(&my_screen->sems.scene_mux);
			sc = my_screen->scene;
			pthread_mutex_unlock(&my_screen->sems.scene_mux);
			switch(sc){
			case 0:
				rectfill(screen, 336, 250, 500, 300, my_screen->bg_col);
				textout_ex(screen, font, "City Map", 336, 250, makecol(255,255,255), makecol(204,0,0));
				textout_ex(screen, font, "Race Truck", 336, 258, makecol(255,255,255), -1);
				break;
			case 1:
				rectfill(screen, 336, 250, 500, 300, makecol(0,0,0));
				textout_ex(screen, font, "City Map", 336, 250, makecol(255,255,255), -1);
				textout_ex(screen, font, "Race Truck", 336, 258, makecol(255,255,255), makecol(204,0,0));
				break;
			default:
				break;
			}
			break;
		case PLAYING:
			if(!initialized){
				init_ui(my_screen);
				initialized = init_map(my_screen);
			}
			else{
				update_ui(my_screen);
				update_map(my_screen);
				update_td(my_screen);
			}
			break;
		default:
			break;
		}
		if(deadline_miss(tp))
			printf("_________________________________graphic dmiss\n");
		wait_for_period(tp);
	}
	printf("graphic thread esce\n");
	pthread_exit(NULL);
}
