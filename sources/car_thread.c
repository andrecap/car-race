/*
 * car_thread.c
 *
 *  Created on: 29/ott/2015
 *      Author: root
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utility.h"
#include "screen.h"
#include "periodics.h"
#include "cars.h"
#include "car_thread.h"


void *car_thread(void *pp){
	task_par_t*	tp = (task_par_t*)pp;					//Pointer to the default task_par structure
	screen_t* 	my_screen = (screen_t*)tp->arg;			//Pointer to the default screen structure
	car_t* 		my_car;									//Pointer to the car associated with this thread
	double		closeness;								//Expresses the closeness of an object when a sensor hits one of them
	double 		steady_vel = 0;							//Desired speed value
	double 		steady_om = 0;							//Desired angular speed value
	int 		stat;									//Game status
	int			into_mud;								//Boolean value: true if the car is into the grass or an accidented road
	int 		phy_period, period;						//Periods of physics and car thread
	int			my_id;									//Id of the car and associated thread
	int			dir;									//Direction of the sensor that senses the closest obstacle
	int 		stall = 0;								//Stall counter
	int			restall = 0;							//Restall counter
	int			strategy = NORMAL;						//Strategy chosen

	my_id = 		tp->id;
	my_car = 		&my_screen->vw.cpu[my_id];
	init_car(my_screen, my_car, my_id);
	period = 		my_screen->td.tpars.c_tp[my_id].period;
	phy_period = 	my_screen->td.tpars.phy_tp.period;
	set_period(tp);
	while(1){
		stat = get_status(my_screen);
		switch(stat){
		case PLAYING:
			pthread_mutex_lock(&(my_screen->sems.cpucar_mux[my_id]));
			switch(strategy){
			case NORMAL:
				closeness = 1;
				update_car_geom(my_car);
				update_sensorpos(&(my_car->carsensor), my_car->pos, my_car->angle);
				set_dir_and_vel(my_screen, my_car, &closeness, &dir, &steady_vel, &steady_om, &into_mud);
				filter_vel(my_car, steady_vel, steady_om);
				update_despos(my_car, phy_period);
				compute_stall(my_car, &stall, &restall, &strategy, STALL);
				if(into_mud) strategy = GRASS;
				break;
			case STALL:
				if(stall==0){
					stall_dir_vel(my_car, &steady_vel, &steady_om);
				}
				if(stall<=100){
					filter_vel(my_car, steady_vel, steady_om);
					update_despos(my_car, period);
					(stall)+=1;
					update_car_geom(my_car);
					compute_stall(my_car, &restall, &stall, &strategy, RESTALL);
				}else{
					strategy = NORMAL;
					stall = 0;
				}
				break;
			case GRASS:
				update_car_geom(my_car);
				update_sensorpos(&(my_car->carsensor), my_car->pos, my_car->angle);
				pthread_mutex_lock(&my_screen->sems.vw_mux);
				if(bb_sense(my_car, &my_screen->vw)){
					dir = sense_mud(my_car, &my_screen->vw);
					manage_sense_mud(my_car, dir, &steady_vel, &steady_om);
				}else{
					steady_vel = MAX_VEL*2/3;
					steady_om = 0;
				}
				if(mud_detector(my_car, &my_screen->vw)){
					mod_vel(&steady_vel, &steady_om);
					into_mud = 1;
				} else
					into_mud=0;
				pthread_mutex_unlock(&my_screen->sems.vw_mux);
				filter_vel(my_car, steady_vel, steady_om);
				update_despos(my_car, phy_period);
				compute_stall(my_car, &stall, &restall, &strategy, STALL);
				if(!(into_mud))
					strategy = NORMAL;
				break;
			case RESTALL:
				restall_dir_vel(my_car, &steady_vel, &steady_om);
				if(restall<100){
					filter_vel(my_car, steady_vel, steady_om);
					update_despos(my_car, period);
					update_car_geom(my_car);
					(restall)+=1;
					compute_stall(my_car, &stall, &restall, &strategy, STALL);
				}else{
					strategy = NORMAL;
					restall=0;
				}
				break;
			default:
				break;
			}
			pthread_mutex_unlock(&(my_screen->sems.cpucar_mux[my_id]));
			break;
		default:
			break;
		}
		if(deadline_miss(tp))
			printf("__________CAR %d: dmiss\n", my_id);
		wait_for_period(tp);
	}
	//printf("car_thread exits!\n");
	pthread_exit(NULL);
}
