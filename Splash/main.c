#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspaudio.h>
#include <pspaudiolib.h>
#include <psppower.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "graphics.h"
#include "framebuffer.h"
#include "mp3player.h"



PSP_MODULE_INFO("Title Screen", 0, 1, 1);
int done = 0;
typedef struct{
	int x;
	int y;
	int imgX;
	int imgY;
	//int dir;
	Image *img;
	int isalive;	
	//int pctr;
}obj;
obj prs_circle;
obj spl_background;

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common){
   done = 1;
   return 0;
}
/* Callback thread */
int CallbackThread(SceSize args, void *argp){
   int cbid;
   
   cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
   sceKernelRegisterExitCallback(cbid);
   
   sceKernelSleepThreadCB();
   
   return 0;
}
/* Sets up the callback thread and returns its threat id */
int SetupCallbacks(void){
   int thid;
   
   thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
   if(thid >= 0){
      sceKernelStartThread(thid, 0, 0);
   }
   
   return thid;
}


void titleScreen();
void initSplash();
void blitObj(obj object);
void initMusic();

int seq=0;

int main(){
	scePowerSetClockFrequency(333, 333, 166);
	
	SetupCallbacks();
	pspDebugScreenInit();
   	pspAudioInit();
   	initGraphics();
   	initSplash();
   	initMusic();
	
	
	
	
	SceCtrlData pad;
	
	sceDisplayWaitVblankStart();
	MP3_Play();
	clearScreen(0);			
	
	while(!done){
		titleScreen();
		sceCtrlReadBufferPositive(&pad, 1);	
		if(pad.Buttons & PSP_CTRL_CIRCLE){
			done = 1;		
		}	
		else{
			blitObj(spl_background);
			int ctr = 0;
			for(ctr = 0; ctr < 20000; ctr++);
			if(prs_circle.isalive == 1){
				blitObj(prs_circle);
				int ctr2 = 0;
				for(ctr2 = 0; ctr2 < 2000; ctr2++);
				prs_circle.isalive = 0;
			}
			else{
				prs_circle.isalive = 1;
			}
			
			flipScreen();
		}
		if(MP3_EndOfStream() == 1)
			MP3_Stop();
	}
	MP3_Stop(1);
   	MP3_FreeTune(1);
   	
   	//sceKernelSleepThread();
   	
   	sceKernelExitGame();
   	return 0;
}

void titleScreen(){
	

}

void blitObj(obj object){
	blitAlphaImageToScreen(0, 0, object.imgX, object.imgY, object.img, object.x, object.y);
}

void initSplash(){
	
	spl_background.x = 0;
	spl_background.y = 0;
	spl_background.imgX = 480;
	spl_background.imgY = 272;
	spl_background.img = loadImage("bsg_title.png");
	if(!spl_background.img){
		printf("Background image failed to load...");
	}
	spl_background.isalive = 1;
	
	prs_circle.x = 50;
	prs_circle.y = 200;
	prs_circle.imgX = 379;
	prs_circle.imgY = 48;
	prs_circle.img = loadImage("press_circle.png");
	if(!prs_circle.img){
		printf("Press Circle image failed to load...");
	}
	prs_circle.isalive = 0;
	//blitObj(background);
	//return background;
}

void initMusic(){
	MP3_Init(1);
	MP3_Load("theme_battlestar_galactica.mp3");
}
