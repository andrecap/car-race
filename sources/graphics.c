/*
 * graphics.c
 *
 *  Created on: 17/ott/2015
 *      Author: andrea
 */

#include <stdio.h>
#include <math.h>
#include "screen.h"
#include "periodics.h"
#include "graphics.h"
#include "utility.h"

/*Virtual world coordinates to pixel conversion*/
int cord2px(double c){
	int pix_c;

	pix_c = (int) (c * MT_2_PX);
	return pix_c;
}

/*Conversion for car y: takes top left corner*/
int cary2px(double y){
	int pix_y;

	pix_y = (int) ((y-CAR_H/2) * MT_2_PX);
	return pix_y;
}

/*Conversion for car x: takes top left corner*/
int carx2px(double x){
	int pix_x;

	pix_x = (int) ((x - CAR_W/2) * MT_2_PX);
	return pix_x;
}

/*Initializes map portion of screen*/
short init_map(screen_t* my_screen){
	blit(my_screen->map.background, screen, 0, 0, 0, 0, MAP_W-1, MAP_H-1);
	return 1;
}

/*Updates map portion of screen*/
void update_map(screen_t* my_screen){
	car_t* player = &my_screen->vw.player;
	car_t* cpu = my_screen->vw.cpu;
	int i;
	int xblit = cord2px(player->bound_box.x);
	int yblit = cord2px(player->bound_box.y);

	pthread_mutex_lock(&my_screen->sems.pcar_mux);
	blit(my_screen->map.background, screen, xblit -10, yblit-10, xblit-10, yblit-10, cord2px(player->bound_box.w)+20, cord2px(player->bound_box.h)+20);
	for(i = 0; i<CPUCARS; i++){
			if(cpu[i].alive){
				pthread_mutex_lock(&my_screen->sems.cpucar_mux[i]);
				xblit = cord2px(cpu[i].bound_box.x);
				yblit = cord2px(cpu[i].bound_box.y);
				blit(my_screen->map.background, screen, xblit-10, yblit-10, xblit-10, yblit-10, cord2px(cpu[i].bound_box.w)+20, cord2px(cpu[i].bound_box.h)+20);
			}
		}
	for(i=0;i<CPUCARS; i++)
		if(cpu[i].alive){
			rotate_sprite(screen, cpu[i].sprite, carx2px(cpu[i].pos.x), cary2px(cpu[i].pos.y), ftofix(cpu[i].angle*256/360));
			pthread_mutex_unlock(&my_screen->sems.cpucar_mux[i]);
		}
	rotate_sprite(screen, player->sprite, carx2px(player->pos.x), cary2px(player->pos.y), ftofix(player->angle*256/360));
	pthread_mutex_unlock(&my_screen->sems.pcar_mux);
}

/*Initializes User Interface portion of screen*/
void init_ui(screen_t* my_screen){
	blit(my_screen->ui.if_bmp, screen, 0, 0, 0, MAP_H, UI_W, UI_H);
	masked_blit(my_screen->ui.speedmeter, screen, 0, 0, UI_W -my_screen->ui.speedmeter->w, MAP_H, my_screen->ui.speedmeter->w, my_screen->ui.speedmeter->h);
	draw_sprite(screen, my_screen->ui.needle, UI_W -my_screen->ui.speedmeter->w + 66, MAP_H);
}

/*Updates User Interface portion of screen*/
void update_ui(screen_t* my_screen){
	car_t *player = &my_screen->vw.player;
	double angle;
	int i;

	masked_blit(my_screen->ui.redspeedmeter, screen, 0, 0, 0, MAP_H, my_screen->ui.redspeedmeter->w, my_screen->ui.redspeedmeter->h);
	pthread_mutex_lock(&my_screen->sems.pcar_mux);
	angle = - 135 + player->vel * 3.6;
	rotate_sprite(screen, my_screen->ui.needle, 66, MAP_H, ftofix(angle*256/360));
	pthread_mutex_unlock(&my_screen->sems.pcar_mux);
	for(i=0;i<CPUCARS; i++){
		if(my_screen->vw.cpu[i].alive){
			masked_blit(my_screen->ui.speedmeter, screen, 0, 0, UI_W -my_screen->ui.speedmeter->w *(1+i), MAP_H, my_screen->ui.speedmeter->w, my_screen->ui.speedmeter->h);
			pthread_mutex_lock(&my_screen->sems.cpucar_mux[i]);
			angle = - 135 + my_screen->vw.cpu[i].vel * 3.6;
			rotate_sprite(screen, my_screen->ui.little_needle, UI_W -my_screen->ui.speedmeter->w *(1+i) + 49, MAP_H, ftofix(angle*256/360));
			pthread_mutex_unlock(&my_screen->sems.cpucar_mux[i]);
		}
	}
}

/*Updates Thread Data portion of screen*/
void update_td(screen_t* my_screen){
	char string[32][32];
	int i;
	thread_data_t* td = &my_screen->td;

	sprintf(string[0],"PLAYER:");
	sprintf(string[1],"       period = %d",td->tpars.p_tp.period);
	sprintf(string[2],"     priority = %d",td->tpars.p_tp.priority);
	sprintf(string[3],"        dmiss = %d",td->tpars.p_tp.dmiss);
	sprintf(string[4],"GRAPHICS:");
	sprintf(string[5],"       period = %d",td->tpars.g_tp.period);
	sprintf(string[6],"     priority = %d",td->tpars.g_tp.priority);
	sprintf(string[7],"        dmiss = %d",td->tpars.g_tp.dmiss);
	sprintf(string[8],"PHYSICS:");
	sprintf(string[9],"       period = %d",td->tpars.phy_tp.period);
	sprintf(string[10],"     priority = %d",td->tpars.phy_tp.priority);
	sprintf(string[11],"        dmiss = %d",td->tpars.phy_tp.dmiss);
	for(i = 0; i< CPUCARS; i++){
		if(my_screen->vw.cpu[i].alive){
			sprintf(string[4*i+12],"CAR %d:", i);
			sprintf(string[4*i+13],"       period = %d",td->tpars.c_tp[i].period);
			sprintf(string[4*i+14],"     priority = %d",td->tpars.c_tp[i].priority);
			sprintf(string[4*i+15],"        dmiss = %d",td->tpars.c_tp[i].dmiss);
		}
	}
	rectfill(screen, MAP_W + 20, MAP_H/2, MAP_W + TD_W, MAP_H, makecol(0,0,0));
	for(i = 0; i<12;i++)
		textout_ex(screen, font, string[i], MAP_W + 20, MAP_H/2 +8*i, makecol(255,255,255), -1);

	for(i=0;i<CPUCARS;i++){
		if(my_screen->vw.cpu[i].alive){
			textout_ex(screen, font, string[4*i +12], MAP_W + 20, MAP_H/2 + 96 + 32*i, makecol(255,255,255), -1);
			textout_ex(screen, font, string[4*i +13], MAP_W + 20, MAP_H/2 + 104 + 32*i, makecol(255,255,255), -1);
			textout_ex(screen, font, string[4*i +14], MAP_W + 20, MAP_H/2 + 110 + 32*i, makecol(255,255,255), -1);
			textout_ex(screen, font, string[4*i +15], MAP_W + 20, MAP_H/2 + 118 + 32*i, makecol(255,255,255), -1);
		}
	}
}

