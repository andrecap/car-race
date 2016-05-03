/*
 * types.h
 *
 *  Created on: 29/set/2015
 *      Author: andrea
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <allegro.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define pi			3.1415926535

#define lp_weight	0.95

#define MT_2_PX		12

#define MENU 		0
#define PLAYING		1
#define GAMEOVER	2

#define NUMSCENES 	2
#define CPUCARS		5
#define MAX_OBS		64

#define PLAYER_ID	10

#define MAX_VEL 20				//expressed in m/s


typedef struct coord{
	double x;
	double y;
}coord_t;

typedef struct collision_rect{
	float x;
	float y;
	float w;
	float h;
	short collision;
}coll_rect_t;

typedef struct carsens{
	coord_t sensor;
	coord_t left;
	coord_t center;
	coord_t right;
}sens_t;

typedef struct car{
	coord_t	pos;
	coord_t	old_pos;
	coord_t desired_pos;
	coord_t vertx[4];
	sens_t	carsensor;
	coll_rect_t bound_box;
	double	vel;
	double	angle;
	double	desired_angle;
	double	omega;
	float	w;			//in meters
	float	h;			//in meters
	short	id;
	short	alive;
	BITMAP*	sprite;
}car_t;

typedef struct map{
	int		x;
	int		y;
	int 	w;
	int		h;
	BITMAP* background;
}map_t;

typedef struct semaphores{
	pthread_mutex_t	stat_mux;
	//pthread_mutex_t	scr_mux;
	pthread_mutex_t	scene_mux;
	pthread_mutex_t	vw_mux;
	pthread_mutex_t pcar_mux;
	pthread_mutex_t cpucar_mux[CPUCARS];
	sem_t 			sync[2];
}semaphores_t;

typedef struct interface{
	int		x;
	int		y;
	int		w;
	int		h;
	int		bg_col;
	BITMAP*	if_bmp;
	BITMAP* speedmeter;
	BITMAP* redspeedmeter;
	BITMAP* needle;
	BITMAP* little_needle;
}interface_t;

typedef struct thread_ids{
	pthread_t 	p_id;
	pthread_t 	g_id;
	pthread_t 	car_id[CPUCARS];
	pthread_t	phy_id;
}thread_ids_t;

typedef struct task_par{
	void*	arg;
	int		id;
	long 	wcet;
	int		period;
	int		deadline;
	int		priority;
	int		dmiss;
	struct	timespec at;
	struct	timespec dl;
}task_par_t;

typedef struct task_params{
	task_par_t p_tp;
	task_par_t g_tp;
	task_par_t c_tp[CPUCARS];
	task_par_t phy_tp;
}task_params_t;

typedef struct thread_data{
	int				x;
	int				y;
	int				w;
	int				h;
	int				bg_col;
	//int				active_threads;
	thread_ids_t	tids;
	task_params_t	tpars;
	BITMAP*			td_bmp;
	pthread_mutex_t	td_mux;
}thread_data_t;

typedef struct virtualworld{
	float 		w;
	float		h;
	car_t		player;
	car_t		cpu[CPUCARS];
	coll_rect_t	hard_obs[MAX_OBS];	// to be avoided, stops movement
	coll_rect_t	soft_obs[MAX_OBS];	// to be avoided, but only half speed
}virtualworld_t;

typedef struct screen{
	int 			w;
	int 			h;
	char*			images_folder;
	int				bg_col;
	int				status;
	int				scene;
	virtualworld_t	vw;
	semaphores_t	sems;
	interface_t 	ui;
	map_t			map;
	thread_data_t	td;
}screen_t;

#endif /* TYPES_H_ */
