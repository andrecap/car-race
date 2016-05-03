/*
 * utility.h
 *
 *  Created on: 07/nov/2015
 *      Author: root
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include "types.h"

short equals(double a, double b);
coord_t coord_transform(coord_t dist, coord_t offset, double angle);
double lowpass(double y, double x);
void update_despos(car_t* car, int period);
void update_car_geom(car_t* my_car);
void filter_vel(car_t* my_car, double steady_vel, double steady_om);
void car_vertex( coord_t *vertex, coord_t pos, double angle);
void box_vertex(coord_t *vertex, coll_rect_t box);
void compute_bb(car_t* my_car);
coll_rect_t compute_linebb(coord_t A, coord_t B);
void vw_bb_init(virtualworld_t* vw);
int bb_coll_detection(coll_rect_t* bb, int id, virtualworld_t* vw);
int collision_detector(car_t* car, virtualworld_t* vw);
void mod_pos(int kind_of_coll, car_t* car, void* coll_obj);
void mod_vel(double* steady_vel, double* steady_om);
coll_rect_t des_bb(coord_t* vertx);
int box_vs_box(coll_rect_t* a, coll_rect_t* b);
int mud_detector(car_t* car, virtualworld_t* vw);
int point_vs_box(coord_t A,  coll_rect_t box);
int line_collision(double* closeness, coord_t A1, coord_t A2, coord_t B1, coord_t B2);
void new_car(car_t* car, short id, char* img_folder);
int get_status(screen_t* my_screen);



#endif /* UTILITY_H_ */
