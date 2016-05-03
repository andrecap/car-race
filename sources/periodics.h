/*
 * periodics.h
 *
 *  Created on: 02/ott/2015
 *      Author: andrea
 */

#ifndef PERIODICS_H_
#define PERIODICS_H_

#include "types.h"

void set_period(task_par_t *tp);
void wait_for_period(task_par_t *tp);
int deadline_miss(task_par_t * tp);


#endif /* PERIODICS_H_ */
