/*
 * screen.c
 *
 *  Created on: 01/ott/2015
 *      Author: andrea
 */

#include <stdio.h>
#include <allegro.h>
#include "utility.h"
#include "screen.h"


void move_cursor_down(screen_t* my_screen){
	pthread_mutex_lock(&(my_screen->sems.scene_mux));
	if (my_screen->scene < NUMSCENES -1){
		my_screen->scene++;
	}
	printf("scene = %d\n", my_screen->scene);
	pthread_mutex_unlock(&(my_screen->sems.scene_mux));
}

void move_cursor_up(screen_t* my_screen){
	pthread_mutex_lock(&(my_screen->sems.scene_mux));
	if (my_screen->scene > 0){
		my_screen->scene--;
	}
	printf("scene = %d\n", my_screen->scene);
	pthread_mutex_unlock(&(my_screen->sems.scene_mux));
}

void select_scene(screen_t* my_screen, char* str, BITMAP* bg_buff){
	pthread_mutex_lock(&(my_screen->sems.scene_mux));
	switch(my_screen->scene){
	case 0:
		strcat(str, "citymap1.tga");
		break;
	case 1:
		strcat(str, "track_bg.tga");
		break;
	default:
		break;
	}
	pthread_mutex_unlock(&(my_screen->sems.scene_mux));
	bg_buff = load_bitmap(str, NULL);
	if(!bg_buff){
		printf("problems in loading background!\n");
		exit(1); //ABORT
	}else{
		printf("background loaded\n");
		stretch_blit(bg_buff, my_screen->map.background, 0, 0, bg_buff->w - 1, bg_buff->h-1, 0, 0, MAP_W -1, MAP_H -1);
	}
	destroy_bitmap(bg_buff);
}

void init_screen_map(screen_t* my_screen){
	my_screen->map.x = 0;
	my_screen->map.y = 0;
	my_screen->map.w = MAP_W;
	my_screen->map.h = MAP_H;
	my_screen->map.background = create_bitmap(my_screen->map.w, my_screen->map.h);
	clear_to_color(my_screen->map.background, makecol(0,0,0));
	textout_centre_ex(my_screen->map.background, font, "--- CAR RACE ----", 400, 200, makecol(204, 0 , 0), makecol(255, 255, 0));
	textout_centre_ex(my_screen->map.background, font, "select your map and go!", 400, 208, makecol(204, 0 , 0), makecol(255, 255, 0));
	textout_ex(my_screen->map.background, font, "City Map", 336, 250, makecol(255, 255, 255), makecol(204, 0 , 0));
	textout_ex(my_screen->map.background, font, "Race Truck", 336, 258, makecol(255, 255, 255), -1);
}

void init_screen_ui(screen_t* my_screen){
	my_screen->ui.x = MAP_W;
	my_screen->ui.y = 0;
	my_screen->ui.w = UI_W - 1;
	my_screen->ui.h = UI_H - 1;
	my_screen->ui.bg_col = makecol(0, 0, 0);
	my_screen->ui.if_bmp = create_bitmap(my_screen->ui.w, my_screen->ui.h);
	my_screen->ui.speedmeter = create_bitmap(my_screen->ui.h * 3/4, my_screen->ui.h * 3/4);
	my_screen->ui.redspeedmeter = create_bitmap(my_screen->ui.h, my_screen->ui.h);
	my_screen->ui.needle = create_bitmap(my_screen->ui.h / 5, my_screen->ui.h);
	my_screen->ui.little_needle = create_bitmap(25, 126);
}

void init_screen_td(screen_t* my_screen){
	my_screen->td.x = 0;
	my_screen->td.y = MAP_H;
	my_screen->td.w = TD_W;
	my_screen->td.h = TD_H;
	my_screen->td.bg_col = makecol(0,0,0);
	my_screen->td.td_bmp = create_bitmap(my_screen->td.w, my_screen->td.h);
	clear_to_color(my_screen->td.td_bmp, my_screen->td.bg_col);
}

int init_screen(screen_t* my_screen, int width, int height, char* images_folder){
	int i;
	char str[5][256];
	BITMAP	*buff0;
	BITMAP	*buff1;
	BITMAP	*buff2;
	BITMAP	*buff3;
	BITMAP	*buff4;

	my_screen-> w = width;
	my_screen-> h = height;
	my_screen-> images_folder= images_folder;
	my_screen-> bg_col = makecol(0,0,0);
	my_screen-> status = MENU;
	my_screen->scene = 0 ;
	allegro_init();
	printf("allegro initialized\n");
	set_window_title("Car Race");
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, my_screen->w, my_screen->h, 0, 0);
	install_keyboard();
	strcpy(str[0], images_folder);
	strcpy(str[1], images_folder);
	strcpy(str[2], images_folder);
	strcpy(str[3], images_folder);
	strcpy(str[4], images_folder);
	strcat(str[0], "dash_texture.tga");
	strcat(str[1], "speedmeter.tga");
	strcat(str[2], "needle.tga");
	strcat(str[3], "reddash.tga");
	strcat(str[4], "needle.tga");

	/*init map */
	init_screen_map(my_screen);

	/*init user interface*/
	init_screen_ui(my_screen);
	buff0 = load_bitmap(str[0], NULL);
	if(!buff0)
		printf("problems loading texture bmp!\n");
	stretch_blit(buff0, my_screen->ui.if_bmp, 0, 0, buff0->w - 1, buff0->h - 1, 0, 0, UI_W - 1, UI_H -1);
	destroy_bitmap(buff0);
	buff1 = load_bitmap(str[1], NULL);
	if(!buff1)
		printf("problems loading speedmeter bmp!\n");
	stretch_blit(buff1, my_screen->ui.speedmeter, 0, 0, buff1->w, buff1->h , 0, 0, my_screen->ui.speedmeter->w , my_screen->ui.speedmeter->h);
	destroy_bitmap(buff1);
	buff2 = load_bitmap(str[2], NULL);
	if(!buff2)
		printf("problems loading needle bmp!\n");
	stretch_blit(buff2, my_screen->ui.needle, 0, 0, buff2->w, buff2->h, 0, 0, my_screen->ui.needle->w, my_screen->ui.needle->h);
	destroy_bitmap(buff2);
	buff3 = load_bitmap(str[3], NULL);
	if(!buff3)
		printf("problems loading speedmeter bmp!\n");
	stretch_blit(buff3, my_screen->ui.redspeedmeter, 0, 0, buff3->w, buff3->h , 0, 0, my_screen->ui.redspeedmeter->w , my_screen->ui.redspeedmeter->h);
	destroy_bitmap(buff3);
	buff4 = load_bitmap(str[4], NULL);
	if(!buff4)
		printf("problems loading little needle bmp!\n");
	stretch_blit(buff4, my_screen->ui.little_needle, 0, 0, buff4->w, buff4->h, 0, 0, my_screen->ui.little_needle->w, my_screen->ui.little_needle->h);
	destroy_bitmap(buff4);

	/*init thread data*/
	init_screen_td(my_screen);

	/*Generate Cars*/
	new_car(&my_screen->vw.player, PLAYER_ID, my_screen->images_folder);
	for(i=0; i<CPUCARS; i++)
		new_car(&my_screen->vw.cpu[i], i, my_screen->images_folder);

	blit(my_screen->map.background, screen, 0, 0, 0, 0, my_screen->map.w - 1, my_screen->map.h - 1);
	blit(my_screen->td.td_bmp, screen, 0, 0, MAP_W, 0, my_screen->td.w - 1, my_screen->td.h - 1);
	printf("screen initialized\n");
	return 0;
}


