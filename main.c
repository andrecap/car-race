/*
 * main.c
 *
 *  Created on: 29/set/2015
 *      Author: andrea
 */

#include <stdio.h>
#include "sources/screen.h"
#include "sources/generator.h"

#define WIDTH	1024
#define HEIGHT	768

int main(){
	screen_t	my_screen;							//screen structure, contains all that is needed by the program
	char*		images_folder = "/home/andrea/workspace/car_race_v5/images/";	//images folder path

	printf("main started!\n");
	init_screen(&my_screen, WIDTH, HEIGHT, images_folder);
	gen_threads(&my_screen);
	printf("main finished!\n");
	allegro_exit();
	return 0;
}
//END_OF_MAIN()
