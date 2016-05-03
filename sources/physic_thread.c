/*
 * physic_thread.c
 *
 *  Created on: 06/dic/2015
 *      Author: root
 */

#include <stdio.h>
#include <math.h>
#include "periodics.h"
#include "physics.h"
#include "utility.h"
#include "physic_thread.h"

void* physics_thread(void* pp){
	task_par_t*	tp = (task_par_t*)pp;						//Pointer to the default task_par structure
	screen_t* 	my_screen = (screen_t*)tp->arg;				//Pointer to the default screen structure
	car_t*		playercar = &my_screen->vw.player;			//Pointer to the player car structure
	car_t*		cpucar[CPUCARS];							//Pointers to the cpu cars structures
	int 		stat;										//State of the game
	int 		i;


	for(i = 0; i<CPUCARS; i++)
		cpucar[i] = &my_screen->vw.cpu[i];
	printf("physics thread born: %lu\n", (unsigned long)pthread_self());
	set_period(tp);
	while(1){
		stat = get_status(my_screen);
		switch(stat){
		case PLAYING:
			pthread_mutex_lock(&(my_screen->sems.pcar_mux));
			for(i = 0; i<CPUCARS; i++)
				if(cpucar[i]->alive)
					pthread_mutex_lock(&(my_screen->sems.cpucar_mux[i]));
			pthread_mutex_lock(&my_screen->sems.vw_mux);
			update_car_geom(playercar);
			if(!collision_detector(playercar, &my_screen->vw))
				update_pos(playercar);
			else
				playercar->old_pos = playercar->pos;
			for(i=0;i<CPUCARS;i++){
				if(cpucar[i]->alive){
					update_car_geom(cpucar[i]);
					if(!collision_detector(cpucar[i], &my_screen->vw))
						update_pos(cpucar[i]);
					else
						cpucar[i]->old_pos = cpucar[i]->pos;
				}
			}
			pthread_mutex_unlock(&my_screen->sems.vw_mux);
			for(i = CPUCARS-1; i>=0; i--)
				if(cpucar[i]->alive){
					pthread_mutex_unlock(&(my_screen->sems.cpucar_mux[i]));
				}
			pthread_mutex_unlock(&(my_screen->sems.pcar_mux));
			break;
		default:
			break;
			}

		if(deadline_miss(tp))
			printf("________physics dmiss\n");
		wait_for_period(tp);
	}
	printf("physics_thread exits!\n");
	pthread_exit(NULL);
}

