/*
 * cars.h
 *
 *  Created on: 04/dic/2015
 *      Author: root
 */

#ifndef CARS_H_
#define CARS_H_

#include "types.h"

#define LEFT		0
#define CENTER		1
#define RIGHT		2
#define NEAROBS		3
#define NOOBS		4

#define SENSE_RAD	CAR_W

#define NORMAL		0
#define STALL		1
#define GRASS		2
#define RESTALL		3

void init_car(screen_t* my_screen, car_t* my_car, int my_id);
void update_sensorpos(sens_t* sens, coord_t pos, double angle);
int bb_sense(car_t* car, virtualworld_t *vw);
void compute_closeness(double* buff, double * far, sens_t* sens, coll_rect_t bb);
int sense_mud(car_t* car, virtualworld_t *vw);
void manage_sense(car_t* car, virtualworld_t *vw, double closeness, int dir, double* steady_vel, double* steady_om);
void manage_sense_mud(car_t* car, int dir, double* steady_vel, double *steady_om);
double line_sense(int* direction, car_t* car, virtualworld_t *vw);
void set_dir_and_vel(screen_t* my_screen, car_t* my_car, double* closeness, int* dir, double* steady_vel, double* steady_om, int* into_mud);
void compute_stall(car_t* my_car, int* stall_times, int* restall_times, int* strategy, int new_strategy);
void stall_dir_vel(car_t* my_car, double* steady_vel, double* steady_om);
void restall_dir_vel(car_t* my_car, double* steady_vel, double* steady_om);



#endif /* CARS_H_ */
