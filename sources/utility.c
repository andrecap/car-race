/*
 * utility.c
 *
 *  Created on: 07/nov/2015
 *      Author: root
 */
#include <math.h>
#include <stdio.h>
#include "utility.h"
#include "screen.h"

#define EPSILON 0.005

/*Returns equality between two doubles with tolerance epsilon*/
short equals(double a, double b){
	short eq = 0;
	if(a <= b + EPSILON && a>= b-EPSILON) eq=1;
	return eq;
}

/*Lowpass filter*/
double lowpass(double y, double x){
	double filt = lp_weight*x + (1-lp_weight)*y;
	return filt;
}

/*Returns games status*/
int get_status(screen_t* my_screen){
	int stat;
	pthread_mutex_lock(&(my_screen->sems.stat_mux));
	stat = my_screen->status;
	pthread_mutex_unlock(&(my_screen->sems.stat_mux));
	return stat;
}

/*Updates car's desired position*/
void update_despos(car_t* car, int period){
	car->desired_angle = car->angle + car->omega*period*0.001;
	if(car->desired_angle>=360)
		car->desired_angle=car->desired_angle - 360;
	if(car->desired_angle<=-360)
		car->desired_angle=car->desired_angle + 360;
	car->desired_pos.x = car->pos.x + cos(car->desired_angle * pi / 180)*car->vel*period*0.001;
	car->desired_pos.y = car->pos.y + sin(car->desired_angle * pi / 180)*car->vel*period*0.001; // - sin?

}

/*Update the geometry of the car, given by: vertexes, bounding box*/
void update_car_geom(car_t* my_car){
	car_vertex(my_car->vertx, my_car->pos, my_car->angle);
	compute_bb(my_car);
}

/*Applies lowpass filter to actual and desired speed*/
void filter_vel(car_t* my_car, double steady_vel, double steady_om){
	my_car->vel=lowpass(steady_vel, my_car->vel);
	my_car->omega = lowpass(steady_om, my_car->omega);
}

/*Translation and Rotation Matrix effects*/
coord_t coord_transform(coord_t dist, coord_t offset, double angle){
	coord_t newpos;
	double c = cos(-angle * pi / 180);
	double s = sin(-angle * pi / 180);
	newpos.x = offset.x + (dist.x)*c + (dist.y)*s;
	newpos.y = offset.y - (dist.x)*s + (dist.y)*c;
	return newpos;
}

/*MUST BE GIVEN IN ORDER: FIRST sensor THEN arrive point THEN collision box ones*/
/*Returns a value in [0,1] where 0 means the collision point is in A1(sensor origin), 1 means it is in A2(sensor radius)*/
int line_collision(double *closeness, coord_t A1, coord_t A2, coord_t B1, coord_t B2){
	double denom, ua, ub;
	int coll = 0;
	denom = ((B2.y - B1.y)*(A2.x - A1.x)) - ((B2.x-B1.x)*(A2.y - A1.y));
	if(denom!=0){
		ua = (((B2.x - B1.x) * (A1.y - B1.y)) - ((B2.y - B1.y) * (A1.x - B1.x))) / denom;
		ub = (((A2.x - A1.x) * (A1.y - B1.y)) - ((A2.y - A1.y) * (A1.x - B1.x))) / denom;
		if((ua<0)||(ua>1)||(ub<0)||(ub>1))
				coll= 0;
		else {
			coll = 1;
			*closeness = ua;
		}
	}
	return coll;
}

/*Detects whether a point is included into a box*/
int point_vs_box(coord_t A, coll_rect_t box){
	int collided = 0;
	if(A.x <= box.x+box.w && A.x >= box.x && A.y <= box.y+box.h && A.y>=box.y)
		collided=1;
	return collided;
}

/*Box vs Box collision Detection*/
int box_vs_box(coll_rect_t* a, coll_rect_t* b){
	int coll = 0;
	if((a->x < b->x + b->w) && (a->x+a->w > b->x) && (a->y < b->y + b->h) && (a->y+a->h > b->y)){
		a->collision = b->collision = 1;
		coll = 1;
	}
	return coll;
}

/*Given a bounding box bb, sets at 1 all the collided field of colliding boxes in the virtual world*/
int bb_coll_detection(coll_rect_t* bb, int id, virtualworld_t* vw){
	int i;
	int coll = 0;
	vw_bb_init(vw);
	for(i=0;i<MAX_OBS;i++){
		if(box_vs_box(bb, &vw->soft_obs[i]) ||	box_vs_box(bb, &vw->hard_obs[i]))
			coll = 1;
	}
	for(i=0;i<CPUCARS;i++){
		if(vw->cpu[i].id != id)
			if(box_vs_box(&vw->cpu[i].bound_box, bb))
				coll = 1;
	}
	if(vw->player.id != id)
		if(box_vs_box(&vw->player.bound_box, bb))
			coll = 1;
	return coll;
}


/*Detects a collision with any 2 objects in the virtual world*/
int collision_detector(car_t* car, virtualworld_t* vw){
	int i,j,k;
	double dummy;
	int coll = 0;
	coord_t des_vertx[4];
	car_vertex(des_vertx, car->desired_pos, car->desired_angle);
	coll_rect_t desired_bb;
	desired_bb = des_bb(des_vertx);

	if(bb_coll_detection(&desired_bb, car->id, vw)){
			for(i=0;i<MAX_OBS;i++){
				if(vw->hard_obs[i].collision)
					for(j = 0; j<4; j++)
						if(point_vs_box(des_vertx[j], vw->hard_obs[i])){
							coll = 1;
						}
			}
			for(i=0;i<CPUCARS;i++){
				if(vw->cpu[i].bound_box.collision && vw->cpu[i].id != car->id && vw->cpu[i].alive){
					for(j=0;j<4;j++){
						for(k=0;k<4;k++){
							if(line_collision(&dummy, vw->cpu[i].vertx[j], vw->cpu[i].vertx[(j+1)%4], des_vertx[k], des_vertx[(k+1)%4])){
								coll = 1;
							}
						}
					}
				}
			}
			if(vw->player.bound_box.collision && vw->player.id != car->id)
				for(j=0;j<4;j++)
					for(k=0;k<4;k++)
						if(line_collision(&dummy, vw->player.vertx[j], vw->player.vertx[(j+1)%4], des_vertx[k], des_vertx[(k+1)%4])){
							coll = 1;
						}
	}
	for(j=0;j<4;j++)
		if(des_vertx[j].x < 0 || des_vertx[j].x > 800/MT_2_PX || des_vertx[j].y < 0 || des_vertx[j].y > 600/MT_2_PX)
			coll = 1;
	return coll;
}

/*Detects whether a car is into accidental field or grass*/
int mud_detector(car_t* car, virtualworld_t* vw){
	int i;
	int into_mud = 0;
	vw_bb_init(vw);
	bb_coll_detection(&car->bound_box, car->id, vw);
	if(car->bound_box.collision){
		for(i=0;i<MAX_OBS;i++){
			if(vw->soft_obs[i].collision)
					if(point_vs_box(car->pos, vw->soft_obs[i])){
						into_mud = 1;
					}
		}
	}
	return into_mud;
}

/*Applies speed modifiers*/
void mod_vel(double* steady_vel, double* steady_om){
	*steady_vel/=2;
	*steady_om/=2;
}

/*Resets the collision field of the obstacles in virtual world to 0*/
void vw_bb_init(virtualworld_t* vw){
	int i;
	for(i=0;i<MAX_OBS; i++){
		vw->hard_obs[i].collision=0;
		vw->soft_obs[i].collision=0;
	}
	for(i=0;i<CPUCARS;i++)
		vw->cpu[i].bound_box.collision =0;
	vw->player.bound_box.collision = 0;
}

/*Computes the vertexes of the car pointed*/
void car_vertex( coord_t *vertex, coord_t pos, double angle){
	coord_t dist;
	dist.x = -CAR_W/2;
	dist.y = -CAR_H/2;
	vertex[0] = coord_transform(dist, pos, angle);
	dist.x = -CAR_W/2;
	dist.y = CAR_H/2;
	vertex[1] = coord_transform(dist, pos, angle);
	dist.x = CAR_W/2;
	dist.y = CAR_H/2;
	vertex[2] = coord_transform(dist, pos, angle);
	dist.x = CAR_W/2;
	dist.y = -CAR_H/2;
	vertex[3] = coord_transform(dist, pos, angle);
}

/*Computes the vertexes of the box pointed*/
void box_vertex(coord_t *vertex, coll_rect_t box){
	vertex[0].x = box.x;
	vertex[0].y = box.y;
	vertex[1].x = box.x + box.w;
	vertex[1].y = box.y;
	vertex[2].x = box.x + box.w;
	vertex[2].y = box.y + box.h;
	vertex[3].x = box.x;
	vertex[3].y = box.y + box.h;
}

/*Computes the bounding box of the car*/
void compute_bb(car_t* my_car){
	coord_t buff;
	coll_rect_t* bb = &my_car->bound_box;
	int i;
	bb->x = my_car->vertx[0].x;
	bb->y = my_car->vertx[0].y;
	buff.x = my_car->vertx[0].x;
	buff.y = my_car->vertx[0].y;
	for(i=1;i<4;i++){
		if(my_car->vertx[i].x < bb->x) bb->x = my_car->vertx[i].x;
		if(my_car->vertx[i].y < bb->y) bb->y = my_car->vertx[i].y;
		if(my_car->vertx[i].x > buff.x) buff.x = my_car->vertx[i].x;
		if(my_car->vertx[i].y > buff.y) buff.y = my_car->vertx[i].y;
	}
	bb->w = buff.x - bb->x;
	bb->h = buff.y - bb->y;
}

/*Given a set of vertexes computes the relative bounding box*/
coll_rect_t des_bb(coord_t* vertx){
	coll_rect_t bb;
	coord_t buff;
	int i;
	bb.x = vertx[0].x;
	bb.y = vertx[0].y;
	buff.x = vertx[0].x;
	buff.y = vertx[0].y;
	for(i=1;i<4;i++){
		if(vertx[i].x < bb.x) bb.x = vertx[i].x;
		if(vertx[i].y < bb.y) bb.y = vertx[i].y;
		if(vertx[i].x > buff.x) buff.x = vertx[i].x;
		if(vertx[i].y > buff.y) buff.y = vertx[i].y;
	}
	bb.w = buff.x - bb.x;
	bb.h = buff.y - bb.y;
	return bb;
}

/*Computes the bounding box of a line*/
coll_rect_t compute_linebb(coord_t A, coord_t B){
	coll_rect_t bb;
	if(A.x <= B.x){
		bb.x = A.x;
		bb.w = (B.x - A.x);
	}
	else{
		bb.x = B.x;
		bb.w = (A.x - B.x);
	}
	if(A.y <= B.y){
		bb.y = A.y;
		bb.h = (B.y - A.y);
	}
	else {
		bb.y = B.y;
		bb.h = (A.y - B.y);
	}
	bb.collision = 0;
	return bb;
}

/*Allocates a new car*/
void new_car(car_t* car, short id, char* img_folder){
	char	str[256];
	BITMAP*	buff;
	strcpy(str, img_folder);
	car->id = 	 id;
	car->w =	 CAR_W;
	car->h = 	 CAR_H;
	car->alive = 0;
	if(car->id == PLAYER_ID)
		strcat(str, "redcar.tga");
	else
		strcat(str, "bluecar.tga");
	buff = load_bitmap(str, NULL);
	if(!buff){
		printf("problems loading car bmp!\n");
	}
	car->sprite = create_bitmap(CAR_W * MT_2_PX, CAR_H * MT_2_PX);
	stretch_blit(buff, car->sprite, 0, 0, buff->w, buff->h, 0, 0, car->sprite->w, car->sprite->h);
	destroy_bitmap(buff);
}

