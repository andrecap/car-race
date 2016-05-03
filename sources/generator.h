/*
 * generator_thread.h
 *
 *  Created on: 05/ott/2015
 *      Author: andrea
 */

#ifndef GENERATOR_THREAD_H_
#define GENERATOR_THREAD_H_

#include <pthread.h>
#include "types.h"

void task_par_init(task_par_t *tp, void* arg, int period, int prio, int id);
void gen_threads(screen_t* my_screen);

int glob_id;

#endif /* GENERATOR_THREAD_H_ */
