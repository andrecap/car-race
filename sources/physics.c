/*
 * physics.c
 *
 *  Created on: 18/nov/2015
 *      Author: root
 */

#include <stdio.h>
#include <math.h>
#include "periodics.h"
#include "physics.h"
#include "utility.h"

void update_pos(car_t *car){
	car->old_pos = car->pos;
	car->angle = car->desired_angle;
	car->pos = car->desired_pos;
}

