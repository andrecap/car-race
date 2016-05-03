/*
 * player.c

 *
 *  Created on: 10/ott/2015
 *      Author: andrea
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
#include "player_thread.h"


void *player_thread(void* pp){
	task_par_t*		tp = (task_par_t*)pp;					//Pointer to the default task_par structure
	screen_t* 		my_screen = (screen_t*)tp->arg;			//Pointer to the default screen structure
	car_t*			playercar = &my_screen->vw.player;		//Pointer to the player's car structure
	char			str[256];								//String for loading the right background
	int				i;
	int				enters = 0;								//Threshold for the ENTER key
	short			stat;									//Game status
	double			steady_vel;								//Desired speed
	double			steady_om;								//Desired angular speed
	int				phy_period;								//Period of Physics thread
	BITMAP*			bg_buff;								//Background bitmap
	task_par_t*		c_taskpar[CPUCARS];						//Parameters to allocate a new car thread
	pthread_t*		c_idp[CPUCARS];							//Parameters to allocate a new car thread

	phy_period = my_screen->td.tpars.phy_tp.period;
	printf("player thread born: %lu\n", (unsigned long)pthread_self());
	bg_buff = create_bitmap(640,480);
	strcpy(str, my_screen->images_folder);
	for(i=0;i<CPUCARS; i++){
		c_idp[i] = &(my_screen->td.tids.car_id[i]);
		c_taskpar[i] = &my_screen->td.tpars.c_tp[i];
	}

	set_period(tp);
	while(!key[KEY_ESC]){
		stat = get_status(my_screen);
		switch(stat){
		case MENU:
			manage_menu(my_screen, str, bg_buff);
			break;
		case PLAYING:
			manage_arrows(my_screen, &steady_vel, &steady_om);
			if(key[KEY_ENTER]){
				steady_vel = 0;
				steady_om = 0;
				if(enters >= 15){
					manage_enter(my_screen, c_idp, c_taskpar);
					enters=0;
				}
				else enters++;
			}
			if(!(key[KEY_ENTER] || key[KEY_DOWN] || key[KEY_UP] || key[KEY_LEFT] || key[KEY_RIGHT])){
				enters=0;
				steady_vel = 0;
				steady_om = 0;
			}
			pthread_mutex_lock(&my_screen->sems.pcar_mux);
			update_car_geom(playercar);
			pthread_mutex_lock(&my_screen->sems.vw_mux);
			if(mud_detector(playercar, &my_screen->vw)){
				mod_vel(&steady_vel, &steady_om);
			}
			pthread_mutex_unlock(&my_screen->sems.vw_mux);
			filter_vel(playercar, steady_vel, steady_om);
			update_despos(playercar, phy_period);
			pthread_mutex_unlock(&my_screen->sems.pcar_mux);
			break;
		default:
			break;
		}
		if(deadline_miss(tp))
			printf("____________player dmiss\n");
		wait_for_period(tp);
	}
	printf("player thread esce\n");
	sem_post(&(my_screen->sems.sync[0]));
	delete_threads(my_screen);
	pthread_exit(NULL);
}
