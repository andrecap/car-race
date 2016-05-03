#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utility.h"
#include "screen.h"
#include "periodics.h"
#include "cars.h"

/*Initializes car with given parameters*/
void init_car(screen_t* my_screen, car_t* my_car, int my_id){
	my_car->id = 	my_id;
	my_car->alive = 1;
	my_car->vel =  	0;
	my_car->omega = 0;
}

/*Updates the origin and ray of the three sensors*/
void update_sensorpos(sens_t* sens, coord_t pos, double angle){
	coord_t sensor, left, right, center;

	sensor.x = CAR_W/2;
	sensor.y = 0;
	left.x = CAR_W/2 + SENSE_RAD;
	left.y = -SENSE_RAD/2;
	right.x = CAR_W/2 + SENSE_RAD;
	right.y = SENSE_RAD/2;
	center.x = CAR_W/2 + SENSE_RAD;
	center.y = 0;
	sens->sensor = coord_transform(sensor, pos, angle);
	sens->left =  coord_transform(left, pos, angle);
	sens->right =  coord_transform(right, pos, angle);
	sens->center =  coord_transform(center, pos, angle);
}


/*Sensors' bounding box collision detection*/
int bb_sense(car_t* car, virtualworld_t *vw){
	int coll = 0;
	sens_t s = car->carsensor;
	coll_rect_t bb_left = compute_linebb(s.sensor, s.left);
	coll_rect_t bb_right = compute_linebb(s.sensor, s.right);
	coll_rect_t bb_center = compute_linebb(s.sensor, s.center);

	if(bb_coll_detection(&bb_left, car->id, vw) || bb_coll_detection(&bb_right, car->id, vw) || bb_coll_detection(&bb_center, car->id, vw))
		coll = 1;
	return coll;
}

/*Calls linear collision detection between each sensor and each line of the given object*/
void compute_closeness(double* buff, double * far, sens_t* sens, coll_rect_t bb){
	coord_t		buff_vert[4];
	int j = 0;

	if(bb.collision){
		box_vertex(buff_vert, bb);
		for(j = 0; j<4; j++){
			if(line_collision(buff, sens->sensor, sens->left, buff_vert[j], buff_vert[(j+1)%4] ) && *buff<far[0])
				far[0] = *buff;
			if(line_collision(buff, sens->sensor, sens->center, buff_vert[j], buff_vert[(j+1)%4] ) && *buff<far[1])
				far[1] = *buff;
			if(line_collision(buff, sens->sensor, sens->right, buff_vert[j], buff_vert[(j+1)%4] ) && *buff<far[2])
				far[2] = *buff;
		}
	}
}

/*Collision Detection with all the objects in virtualworld. The closest one makes the car decide on the direction*/
double line_sense(int* direction, car_t* car, virtualworld_t *vw){
	double min = 1;
	int i;
	double far[3];
	double buff;

	*direction=0;
	far[0] = far[1] = far[2] = 1;
	for(i=0; i<MAX_OBS; i++){
		compute_closeness(&buff, far, &car->carsensor, vw->soft_obs[i]);
		compute_closeness(&buff, far, &car->carsensor, vw->hard_obs[i]);
	}
	for(i = 0; i<CPUCARS; i++)
		if(car->id != vw->cpu[i].id)
			compute_closeness(&buff, far, &car->carsensor, vw->cpu[i].bound_box);
	if(car->id != vw->player.id)
		compute_closeness(&buff, far, &car->carsensor, vw->player.bound_box);
	for(i=0;i<3;i++){
		if(far[i] < min){
			min = far[i];
			*direction= i;
		}
	}
	if(equals(far[0],far[1]) && equals(far[0], far[2]) && min!= 1)
		*direction = 3;
	if(min == 1)
		*direction = 4;
	return min;
}

/*Same as line_sense, but used when into accidental field so avoids collision detection with soft obstacles*/
int sense_mud(car_t* car, virtualworld_t *vw){
	double min = 1;
	int i;
	double far[3];
	double buff;
	int direction = 0;

	far[0] = far[1] = far[2] = 1;
	for(i=0; i<MAX_OBS; i++){
		compute_closeness(&buff, far, &car->carsensor, vw->hard_obs[i]);
	}
	for(i = 0; i<CPUCARS; i++)
		if(car->id != vw->cpu[i].id)
			compute_closeness(&buff, far, &car->carsensor, vw->cpu[i].bound_box);
	if(car->id != vw->player.id)
		compute_closeness(&buff, far, &car->carsensor, vw->player.bound_box);
	for(i=0;i<3;i++){
		if(far[i] < min){
			min = far[i];
			direction= i;
		}
	}
	return min;
}

/*Actuates the decision given by line_sense by modifying the steady_vel and steady_om parameters accordingly*/
void manage_sense(car_t* car, virtualworld_t *vw, double closeness, int dir, double* steady_vel, double* steady_om){
	if(closeness == 1){
		*steady_vel = MAX_VEL*2/3;
		*steady_om = 0;
	}
	else{
		switch(dir){
		case LEFT:
			*steady_vel = MAX_VEL / 3 ;
			*steady_om = 180;
			break;
		case CENTER:
			*steady_vel= 0;
			*steady_om = 0;
			break;
		case RIGHT:
			*steady_vel = MAX_VEL / 3;
			*steady_om = -180;
			break;
		case NEAROBS:
			*steady_vel= 0;
			*steady_om = 0;
			break;
		case NOOBS:
			*steady_vel= MAX_VEL*2/3;
			*steady_om = 0;
			break;
		default:
			break;
		}
	}

}

/*Same as manage_sense, but referred to sense_mud*/
void manage_sense_mud(car_t* car, int dir, double* steady_vel, double *steady_om){
	switch(dir){
	case LEFT:
		*steady_vel = MAX_VEL / 3;
		*steady_om = 120;
		break;
	case CENTER:
		*steady_vel = MAX_VEL * 2/ 3;
		*steady_om = 0;
		break;
	case RIGHT:
		*steady_vel = MAX_VEL * 2/ 3;
		*steady_om = -120;
		break;
	default:
		break;
	}
}

/*Computes the linear and angular speed to be set by acting collision detection*/
void set_dir_and_vel(screen_t* my_screen, car_t* my_car, double* closeness, int* dir, double* steady_vel, double* steady_om, int* into_mud){
	pthread_mutex_lock(&my_screen->sems.vw_mux);
	if(bb_sense(my_car, &my_screen->vw)){
		(*closeness) = line_sense(dir, my_car, &my_screen->vw);
		manage_sense(my_car, &(my_screen->vw), *closeness, *dir, steady_vel, steady_om);
	}else{
		*steady_vel = MAX_VEL*2/3;
		*steady_om = 0;
	}
	if(mud_detector(my_car, &my_screen->vw)){
		mod_vel(steady_vel, steady_om);
		(*into_mud) = 1;
	} else
		(*into_mud)=0;
	pthread_mutex_unlock(&my_screen->sems.vw_mux);
}


/*Enters a new strategy if the car is stalled*/
void compute_stall(car_t* my_car, int* stall_times, int* restall_times, int* strategy, int new_strategy){
	if(equals(my_car->pos.x,my_car->old_pos.x) && equals(my_car->pos.y,my_car->old_pos.y))
		*stall_times+=1;
	else *stall_times = 0;
	if((*stall_times)>=50){
		*strategy= new_strategy;
		*stall_times=0;
		*restall_times = 0;
	}
}

/*Sets direction and speed while in the STALL state*/
void stall_dir_vel(car_t* my_car, double* steady_vel, double* steady_om){
	if((my_car->angle>= 0 && my_car->angle < 45) || (my_car->angle>=90 && my_car->angle<135) ||
	(my_car->angle>= 180 && my_car->angle < 225) || (my_car->angle>= 270 && my_car->angle< 315) ||
	(my_car->angle>= -90 && my_car->angle < -45) || (my_car->angle>= -180 && my_car->angle<-135) ||
	(my_car->angle>= -270 && my_car->angle < -225) || (my_car->angle>= -360 && my_car->angle< -315))
		*steady_om = 90;
	else
		*steady_om = -90;
	*steady_vel = - MAX_VEL/4;
}

/*Sets direction and speed while in the RESTALL state*/
void restall_dir_vel(car_t* my_car, double* steady_vel, double* steady_om){
	if((my_car->angle>= 0 && my_car->angle < 45) || (my_car->angle>=90 && my_car->angle<135) ||
	(my_car->angle>= 180 && my_car->angle < 225) || (my_car->angle>= 270 && my_car->angle< 315) ||
	(my_car->angle>= -90 && my_car->angle < -45) || (my_car->angle>= -180 && my_car->angle<-135) ||
	(my_car->angle>= -270 && my_car->angle < -225) || (my_car->angle>= -360 && my_car->angle< -315))
		*steady_om = -120;
	else
		*steady_om = 120;
	*steady_vel = MAX_VEL/4;
}

