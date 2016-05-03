/*
 * generator_thread.c
 *
 *  Created on: 05/ott/2015
 *      Author: andrea
 */

#include <stdio.h>
#include "player_thread.h"
#include "graphic_thread.h"
#include "car_thread.h"
#include "physic_thread.h"
#include "generator.h"
#include "utility.h"

pthread_attr_t		c_attr;

/*Adds an obstacle to the virtual world*/
void add_obstacle(coll_rect_t* obs, float x, float y, float w, float h){
	obs->x = x / MT_2_PX;
	obs->y = y / MT_2_PX;
	obs->w = w / MT_2_PX;
	obs->h = h / MT_2_PX;
	obs->collision = 0;
}

/*Fills virtualworld with soft and hard obstacles*/
void fill_virt_world(virtualworld_t* vw, short scene){
	int i;
	for(i=0; i<MAX_OBS; i++){
		vw->hard_obs[i].h = vw->hard_obs[i].w = 0;
		vw->hard_obs[i].x = vw->hard_obs[i].y = 0;
		vw->soft_obs[i].h = vw->soft_obs[i].w = 0;
		vw->soft_obs[i].x = vw->soft_obs[i].y = 0;
	}
	switch(scene){
	case 0:
		add_obstacle(&vw->hard_obs[0], 0, 0, 101, 86);
		add_obstacle(&vw->hard_obs[1], 159, 109, 101, 85);
		add_obstacle(&vw->hard_obs[2], 363, 259, 78, 85);
		add_obstacle(&vw->hard_obs[3], 0, 455, 164, 144);
		add_obstacle(&vw->hard_obs[4], 350, 270, 13, 63);
		add_obstacle(&vw->hard_obs[5], 440, 269, 11, 66);
		add_obstacle(&vw->hard_obs[6], 375, 344, 51, 9);
		add_obstacle(&vw->hard_obs[7], 373, 250, 58, 9);
		add_obstacle(&vw->hard_obs[8], 184, 273, 17, 19);
		add_obstacle(&vw->hard_obs[9], 404, 428, 16, 20);
		add_obstacle(&vw->hard_obs[10], 595, 279, 16, 20);
		add_obstacle(&vw->hard_obs[11], 455, 64, 18, 20);
		add_obstacle(&vw->hard_obs[12], 711, 148, 18, 21);
		add_obstacle(&vw->hard_obs[15], 0, 0, 101, 86);
		add_obstacle(&vw->hard_obs[16], 0, 597, 799, 2);
		add_obstacle(&vw->hard_obs[17], 0, 0, 799, 2);
		add_obstacle(&vw->hard_obs[18], 0, 0, 2, 599);
		add_obstacle(&vw->hard_obs[19], 797, 0, 2, 599);
		add_obstacle(&vw->hard_obs[20], 0, 597, 799, 2);
		add_obstacle(&vw->soft_obs[0], 713, 426, 83, 41);
		add_obstacle(&vw->soft_obs[1], 710, 500, 89, 99);
		add_obstacle(&vw->soft_obs[2], 0, 0, 284, 196);
		add_obstacle(&vw->soft_obs[3], 520, 0, 280, 194);
		add_obstacle(&vw->soft_obs[4], 0, 407, 286, 192);
		add_obstacle(&vw->soft_obs[5], 514, 404, 286, 195);
		break;
	case 1:
		add_obstacle(&vw->hard_obs[0], 0, 577, 799, 22);
		add_obstacle(&vw->hard_obs[1], 0, 0, 799, 23);
		add_obstacle(&vw->hard_obs[2], 0, 0, 18, 599);
		add_obstacle(&vw->hard_obs[3], 777, 0, 22, 599);
		add_obstacle(&vw->hard_obs[4], 0, 577, 799, 22);
		add_obstacle(&vw->soft_obs[0], 18, 539, 760, 37);
		add_obstacle(&vw->soft_obs[1], 729, 520, 49, 19);
		add_obstacle(&vw->soft_obs[2], 760, 486, 17, 34);
		add_obstacle(&vw->soft_obs[3], 18, 524, 54, 15);
		add_obstacle(&vw->soft_obs[4], 18, 488, 14, 36);
		add_obstacle(&vw->soft_obs[5], 123, 415, 303, 46);
		add_obstacle(&vw->soft_obs[6], 101, 430, 22, 19);
		add_obstacle(&vw->soft_obs[7], 417, 440, 237, 23);
		add_obstacle(&vw->soft_obs[8], 654, 357, 28, 95);
		add_obstacle(&vw->soft_obs[9], 613, 372, 41, 67);
		add_obstacle(&vw->soft_obs[10], 570, 409, 43, 30);
		add_obstacle(&vw->soft_obs[11], 329, 387, 81, 29);
		add_obstacle(&vw->soft_obs[12], 372, 130, 32, 256);
		add_obstacle(&vw->soft_obs[13], 112, 129, 480, 28);
		add_obstacle(&vw->soft_obs[14], 337, 158, 197, 31);
		add_obstacle(&vw->soft_obs[15], 369, 190, 103, 60);
		add_obstacle(&vw->soft_obs[16], 404, 249, 27, 46);
		add_obstacle(&vw->soft_obs[17], 18, 235, 246, 101);
		add_obstacle(&vw->soft_obs[18], 19, 24, 761, 28);
		add_obstacle(&vw->soft_obs[19], 729, 53, 47, 240);
		add_obstacle(&vw->soft_obs[20], 646, 221, 83, 52);
		add_obstacle(&vw->soft_obs[21], 594, 239, 52, 37);
		add_obstacle(&vw->soft_obs[22], 564, 267, 30, 23);
		add_obstacle(&vw->soft_obs[23], 539, 288, 25, 24);
		add_obstacle(&vw->soft_obs[24], 516, 312, 23, 22);
		add_obstacle(&vw->soft_obs[25], 496, 334, 23, 17);
		add_obstacle(&vw->soft_obs[26], 697, 52, 33, 31);
		add_obstacle(&vw->soft_obs[27], 18, 52, 25, 30);
		add_obstacle(&vw->soft_obs[28], 18, 336, 34, 28);
		add_obstacle(&vw->soft_obs[29], 18, 202, 20, 32);
		add_obstacle(&vw->soft_obs[30], 264, 249, 20, 75);
		add_obstacle(&vw->soft_obs[31], 757, 293, 20, 29);
		add_obstacle(&vw->soft_obs[32], 430, 425, 14, 16);
		add_obstacle(&vw->soft_obs[33], 404, 295, 12, 29);
		add_obstacle(&vw->soft_obs[34], 431, 249, 20, 25);
		add_obstacle(&vw->soft_obs[35], 472, 189, 33, 28);
		add_obstacle(&vw->soft_obs[36], 534, 157, 24, 17);
		add_obstacle(&vw->soft_obs[37], 687, 207, 42, 14);
		add_obstacle(&vw->soft_obs[38], 717, 180, 12, 27);
		add_obstacle(&vw->soft_obs[39], 592, 132, 32, 16);
		add_obstacle(&vw->soft_obs[40], 716, 83, 14, 24);
		add_obstacle(&vw->soft_obs[41], 657, 52, 40, 11);
		add_obstacle(&vw->soft_obs[42], 42, 52, 19, 14);
		add_obstacle(&vw->soft_obs[43], 98, 134, 14, 18);
		add_obstacle(&vw->soft_obs[44], 38, 218, 20, 17);
		add_obstacle(&vw->soft_obs[45], 312, 157, 25, 15);
		add_obstacle(&vw->soft_obs[46], 357, 189, 12, 29);
		add_obstacle(&vw->soft_obs[47], 284, 258, 6, 55);
		add_obstacle(&vw->soft_obs[48], 357, 352, 15, 35);
		add_obstacle(&vw->soft_obs[49], 102, 425, 24, 25);
		add_obstacle(&vw->soft_obs[50], 18, 363, 13, 31);
		add_obstacle(&vw->soft_obs[51], 41, 519, 24, 18);
		add_obstacle(&vw->soft_obs[52], 539, 426, 31, 13);
		add_obstacle(&vw->soft_obs[53], 563, 290, 14, 11);
		add_obstacle(&vw->soft_obs[54], 539, 312, 12, 11);
		add_obstacle(&vw->soft_obs[55], 594, 387, 19, 22);
		add_obstacle(&vw->soft_obs[56], 519, 334, 10, 11);
		add_obstacle(&vw->soft_obs[57], 442, 433, 18, 7);
		break;
	default:
		break;
	}
}

/*Initializes task parameter structure*/
void task_par_init(task_par_t *tp, void* arg, int period, int prio, int id){
	tp->arg = arg;
	tp->period = tp->deadline = period;
	tp->priority = prio;
	tp->dmiss = 0;
	tp->id = id;
}

/*Initializes the scene after selection*/
void prepare_scene(screen_t* my_screen){
	car_t* playercar = &my_screen->vw.player;
	car_t* cpucar = my_screen->vw.cpu;
	playercar->alive =	1;
	playercar->vel = 	0;
	playercar->omega = 	0;
	int i = 0;

	switch(my_screen->scene){
	case 0:
		playercar->pos.x = 	playercar->old_pos.x = playercar->desired_pos.x	= 400 / MT_2_PX;
		playercar->pos.y = 	playercar->old_pos.y = playercar->desired_pos.y = 50 / MT_2_PX;
		playercar->angle = playercar->desired_angle = 90;
		car_vertex(playercar->vertx, playercar->pos, playercar->angle);
		for(i=0;i<2;i++){
			cpucar[0+3*i].pos.x = cpucar[0+3*i].old_pos.x = cpucar[0+3*i].desired_pos.x = 50 / MT_2_PX;
			cpucar[0+3*i].pos.y = cpucar[0+3*i].old_pos.y = cpucar[0+3*i].desired_pos.y = 300 / MT_2_PX;
			cpucar[1+3*i].pos.x = cpucar[1+3*i].old_pos.x = cpucar[1+3*i].desired_pos.x = 750 / MT_2_PX;
			cpucar[1+3*i].pos.y = cpucar[1+3*i].old_pos.y = cpucar[1+3*i].desired_pos.y = 300 / MT_2_PX;
			cpucar[2+3*i].pos.x = cpucar[2+3*i].old_pos.x = cpucar[2+3*i].desired_pos.x = 400 / MT_2_PX;
			cpucar[2+3*i].pos.y = cpucar[2+3*i].old_pos.y = cpucar[2+3*i].desired_pos.y = 550 / MT_2_PX;
			cpucar[0+3*i].angle = cpucar[0+3*i].desired_angle = 	3;
			cpucar[1+3*i].angle = cpucar[1+3*i].desired_angle =	183;
			cpucar[2+3*i].angle = cpucar[2+3*i].desired_angle =	-87;
			car_vertex(cpucar[0+3*i].vertx, cpucar[0+3*i].pos, cpucar[0+3*i].angle);
			car_vertex(cpucar[1+3*i].vertx, cpucar[1+3*i].pos, cpucar[1+3*i].angle);
			car_vertex(cpucar[2+3*i].vertx, cpucar[2+3*i].pos, cpucar[2+3*i].angle);
		}
		break;
	case 1:
		playercar->pos.x = 	playercar->old_pos.x = playercar->desired_pos.x = 215 / MT_2_PX;
		playercar->pos.y = 	playercar->old_pos.y = playercar->desired_pos.y = 480/ MT_2_PX;
		playercar->angle = playercar->desired_angle  =	180;
		compute_bb(playercar);
		car_vertex(playercar->vertx, playercar->pos, playercar->angle);

		for(i=0;i<CPUCARS; i++){
			cpucar[i].pos.x = 	cpucar[i].old_pos.x = cpucar[i].desired_pos.x =	215 / MT_2_PX;
			cpucar[i].pos.y = 	cpucar[i].old_pos.y = cpucar[i].desired_pos.y =	520/ MT_2_PX;
			cpucar[i].angle  = cpucar[i].desired_angle =	180;
			car_vertex(cpucar[i].vertx, cpucar[i].pos, cpucar[i].angle);
			compute_bb(&cpucar[i]);
		}
		break;
	default:
		break;
	}
}

/*Initializes semaphores structures*/
void init_sems(screen_t* my_screen){
	int i;
	pthread_mutex_init(&(my_screen->sems.stat_mux), NULL);
	pthread_mutex_init(&(my_screen->sems.scene_mux), NULL);
	pthread_mutex_init(&(my_screen->sems.pcar_mux), NULL);
	pthread_mutex_init(&(my_screen->sems.vw_mux), NULL);
	for(i = 0; i < CPUCARS; i++)
		pthread_mutex_init(&(my_screen->sems.cpucar_mux[i]), NULL);
	sem_init(&(my_screen->sems.sync[0]), 0, 0);
	sem_init(&(my_screen->sems.sync[1]), 0, 0);
}

/*Initializes all the pthread parameters*/
void init_pthread_parameters(screen_t* my_screen, pthread_attr_t* attr, struct sched_param* par, task_par_t* taskpar, int period, int prio, int id){
	par->__sched_priority = prio;
	pthread_attr_init(attr);
	pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(attr, SCHED_FIFO);
	pthread_attr_setschedparam(attr, par);
	task_par_init(taskpar, my_screen,	period, par->__sched_priority, id);
}

/*Generate the threads*/
void gen_threads(screen_t* my_screen){
	int err = 	0;
	int i;
	task_par_t*			p_taskpar= &my_screen->td.tpars.p_tp;
	pthread_t*			p_idp = &(my_screen->td.tids.p_id);
	pthread_attr_t		p_attr;
	struct sched_param	p_par;
	task_par_t*			g_taskpar = &my_screen->td.tpars.g_tp;
	pthread_t*			g_idp = &(my_screen->td.tids.g_id);
	pthread_attr_t		g_attr;
	struct sched_param	g_par;
	task_par_t*			phy_taskpar= &my_screen->td.tpars.phy_tp;
	pthread_t*			phy_idp= &(my_screen->td.tids.phy_id);
	pthread_attr_t		phy_attr;
	struct sched_param	phy_par;
	pthread_t*			c_idp[CPUCARS];
	struct sched_param	c_par;
	task_par_t*			c_taskpar[CPUCARS];

	printf("thread generator\n");
	init_sems(my_screen);
	init_pthread_parameters(my_screen, &p_attr, &p_par, p_taskpar, 15, 5, PLAYER_ID);
	init_pthread_parameters(my_screen, &g_attr, &g_par, g_taskpar, 20, 2, 11);
	init_pthread_parameters(my_screen, &phy_attr, &phy_par, phy_taskpar, 18, 4, 12);
	for(i=0; i<CPUCARS; i++){
		c_idp[i] = &(my_screen->td.tids.car_id[i]);
		c_taskpar[i] = &my_screen->td.tpars.c_tp[i];
		init_pthread_parameters(my_screen, &c_attr, &c_par, c_taskpar[i], 10, 7, i);
	}

	/*THREAD CREATION-------------*/
	err = pthread_create(p_idp, &p_attr, player_thread, p_taskpar);
	if(err)
		printf("error %d in creating player thread!\n", err);
	else
		printf("player thread created\n");
	err = pthread_create(g_idp, &g_attr, graphic_thread, g_taskpar);
	if(err)
		printf("error %d in creating graphic thread!\n", err);
	else
		printf("graphic thread created\n");
	printf("wait scene selection\n");

	/*WAIT FOR SCENE SELECTION------------*/
	sem_wait(&(my_screen->sems.sync[0]));
	pthread_mutex_lock(&(my_screen->sems.scene_mux));
	prepare_scene(my_screen);
	fill_virt_world(&my_screen->vw, my_screen->scene);
	pthread_mutex_unlock(&(my_screen->sems.scene_mux));

	err = pthread_create(phy_idp, &phy_attr, physics_thread, phy_taskpar);
	if(err)
		printf("error %d in creating physics thread!\n", err);
	else
		printf("physics thread created\n");

	if(my_screen->scene == 0){
		for(i=0; i<3; i++)
			err = pthread_create(c_idp[i], &c_attr, car_thread, c_taskpar[i]);
	}
	else if(my_screen->scene == 1){
		err = pthread_create(c_idp[0], &c_attr, car_thread, c_taskpar[0]);
	}

	/*-------PLAYING STARTS HERE--------------*/
	pthread_mutex_lock(&(my_screen->sems.stat_mux));
	my_screen->status = PLAYING;
	pthread_mutex_unlock(&(my_screen->sems.stat_mux));
	sem_post(&my_screen->sems.sync[1]);
	pthread_join(*p_idp, NULL);
	printf ("generator finished \n");
}
