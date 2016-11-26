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


#define RGB(r,g,b) ((r)|((g)<<8)|((b)<<16))
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define RGB_BLACK (0x00000000)
#define RGB_RED (0x000000FC)
#define RGB_WHITE (0x00FFFFFF)
#define RGB_YELLOW (0x0009CDCB)
#define YES = 1
#define NO = 0
#define ON 1
#define OFF 0
#define MAX_NUM_ENEMIES 100
#define MAX_NUM_BULLETS 100
#define LEFT 0
#define RIGHT 1

PSP_MODULE_INFO("Title Screen", 0, 1, 1);

int done = 0;
int bullets = 0;
char highScore[5];
char playerScore[5];
int points = 0;
int hScore = 1000;

typedef struct{
	int x;
	int y;
	int imgX;
	int imgY;
	//int dir;
	Image *img;
	int isalive;	
	int pctr;
}obj;
obj prs_circle;
obj spl_background;
obj background;
obj starfield;
obj player;
obj enemy[MAX_NUM_ENEMIES];
obj chain[MAX_NUM_BULLETS];

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

void initSplash();
void initMusic();
void blitObj(obj object);
void blitBg(obj object);
void blitEnemies();
void initBackground();
void initStarfield();
void scrollBackground();
void scrollStarfield();
void loadCharacterData();
void loadChain();
void loadPlayer();
void loadEnemies();
void control(SceCtrlData p1);
void shootChain();
void printScore();
int checkCollision(obj blt);

int seq=0;

int main(){
    int exit=0;
    int timer=150;
	//scePowerSetClockFrequency(333, 333, 166);
	
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
	
	while(exit==0){
		sceCtrlReadBufferPositive(&pad, 1);
		blitObj(spl_background);
		
		if(timer<=0){
            blitObj(prs_circle); 
            if(pad.Buttons & PSP_CTRL_CIRCLE){
                exit=1;
            }
        }
        flipScreen();    
        timer=timer-1;        
		if(MP3_EndOfStream() == 1)
			MP3_Stop();
			
	}
	MP3_Stop(1);
   	MP3_FreeTune(1);
   	initBackground();
   	initStarfield();
   	loadCharacterData();
   	unsigned int q = 0;
	int p = 0;
	//sceDisplayWaitVblankStart(); <--was unnecessary
   	while(!done){
		clearScreen(0);  
		scrollBackground();
		scrollStarfield();
        blitEnemies();		
		sceCtrlReadBufferPositive(&pad, 1);	
		/* Counter, for Frameskip */
        for(q = 0; q < 50000; q++){} //was 150000        
        control(pad);
		for(p = 0; p < MAX_NUM_BULLETS; p++){
			chain[p].isalive = checkCollision(chain[p]);
			if(chain[p].isalive == 1){
       			chain[p].x = chain[p].x + 10  * 1.5F;      			
				blitObj(chain[p]);					
			}else{
				chain[p].isalive = 0;
				chain[p].pctr = 0;
			}			
			if(chain[p].x > 485){
					chain[p].isalive = 0;
					chain[p].pctr = 0;
			}
		}
			
        blitObj(player);
        printScore();
        sceDisplayWaitVblankStart();
        flipScreen();
   	}   	
   	//sceKernelSleepThread();
   	
   	sceKernelExitGame();
   	return 0;
}

void blitObj(obj object){
	blitAlphaImageToScreen(0, 0, object.imgX, object.imgY, object.img, object.x, object.y);
}

void blitBg(obj object){
	object.x = object.x + 480;
	blitAlphaImageToScreen(0, 0, object.imgX, object.imgY, object.img, object.x, object.y);
}


void initBackground(){
	
	background.x = 0;
	background.y = 0;
	background.imgX = 480;
	background.imgY = 272;
	background.img = loadImage("space_bg.png");
	if(!background.img){
		printf("Background image failed to load...");
	}
	background.isalive = 1;
	//blitObj(background);
	//return background;
}

void initStarfield(){
	starfield.x = 0;
	starfield.y = 0;
	starfield.imgX = 500;
	starfield.imgY = 300;
	starfield.img = loadImage("starfield_one.png");
	if(!starfield.img){
		printf("Starfield image failed to load...");
	}
	starfield.isalive = 1;
}

void scrollBackground(){

	background.x = background.x-5; 
    blitObj(background); 
 
    if(background.x < 0){
         	blitBg(background);
    	if(background.x<-480){
      		background.x = 0;
       	} 
	}
	if(background.x > 1){
    	background.x = -480 ;
	   	blitBg(background);
	   	if(background.x > 480){
	   	background.x = 0;
	    }	     	
	}
}

void scrollStarfield(){
	starfield.x = starfield.x-10; 
    blitObj(starfield); 
 
    if(starfield.x < 0){
         	blitBg(starfield);
    	if(starfield.x<-480){
      		starfield.x = 0;
       	} 
	}
	if(starfield.x > 1){
    	starfield.x = -480 ;
	   	blitBg(starfield);
	   	if(starfield.x > 480){
	   		starfield.x = 0;
	    }	     	
	}
}

void loadCharacterData(){
	loadPlayer();
	loadEnemies();
	loadChain();
}
void loadChain(){
	int i;
	Image *temp = loadImage("chain.png");
	for(i = 0; i < MAX_NUM_BULLETS; i++){
		chain[i].x = 0;
		chain[i].y = 0;
		chain[i].imgX = 8;
		chain[i].imgY = 6;
		chain[i].img = temp;
		if(!chain[i].img){
			printf("chain image failed to load...");
		}
		chain[i].isalive = 0;
		chain[i].pctr = 0;
	}
}
void loadPlayer(){
	
	player.x = 10;
	player.y = 80;
	player.imgX = 46;
	player.imgY = 24 ;
	player.img = loadImage("vipersm.png");
	if(!player.img){
		printf("Player image failed to load...");
	}
	player.isalive = 1;
	//blitObj(player);
	//return player;
}

void loadEnemies(){

	int i;
	Image *temp = loadImage("raidersm.png");
	for(i = 0; i < MAX_NUM_ENEMIES; i++){
		enemy[i].x = 480 + ((int)rand()%10000);
		enemy[i].y = 1 + ((int)rand()%272 - 34);
		enemy[i].imgX = 42;
		enemy[i].imgY = 20;
		enemy[i].img = temp;
		if(!enemy[i].img){
			printf("Enemy image failed to load...");
		}
		enemy[i].isalive = 1;
		enemy[i].pctr = 0;
		//blitObj(enemy[i]);
	}
	//return enemy[i];
}

void blitEnemies(){
	int enemy_ctr;		
	for(enemy_ctr = 0; enemy_ctr < MAX_NUM_ENEMIES; enemy_ctr++)
		{
			if(enemy[enemy_ctr].isalive == 1){
				float mtpl = ((int)rand()%7) * 2.1;
				
	     		enemy[enemy_ctr].x = enemy[enemy_ctr].x - mtpl;
				if(enemy[enemy_ctr].x < 0){
					enemy[enemy_ctr].isalive = 0;
				}
	     		blitObj(enemy[enemy_ctr]);
	     	} else {
	     		//free up memory.. do not blit to screen
	     		enemy[enemy_ctr].x = 500;
	     	}
		}
}

void control(SceCtrlData p1){	
	if(p1.Buttons & PSP_CTRL_LEFT){
    	//dir = backwards;
     	if(player.x > 60){player.x = player.x - 10;} 
       	//else{bg_x =  bg_x + 10;}        
    } 

    if(p1.Buttons & PSP_CTRL_RIGHT){
     	//dir = forwards;
       	if (player.x <= 150){player.x = player.x+10;}
       	//else{bg_x = bg_x - 10;}         
    }
         
    if((p1.Buttons & PSP_CTRL_UP)&&player.y > -10){player.y = player.y - 10;}  
         
    if((p1.Buttons & PSP_CTRL_DOWN)&&player.y < 262){player.y = player.y + 10;}

    /*   if(pad.Buttons & PSP_CTRL_START) done = 1; */
              
    /*   if(pad.Buttons & PSP_CTRL_SQUARE){} */

    /*   if(pad.Buttons & PSP_CTRL_CIRCLE){} */
         
    /*   if(pad.Buttons & PSP_CTRL_TRIANGLE){} */
         
   	if(p1.Buttons & PSP_CTRL_CROSS){
       	shootChain();
       	//MP3_Play();      	
       	/*fire_x = player_x + 32;
    	fire_y = player_y+8;
       	blitAlphaImageToScreen(0, 0, 32, 16, fire, fire_x, fire_y);*/
    }
    
    //return playerShip;
}

void shootChain(){
	if(bullets < MAX_NUM_BULLETS && chain[bullets].isalive == 0){
		chain[bullets].isalive = 1;
		chain[bullets].x = player.x + 42;
		chain[bullets].y = player.y + (player.imgY / 2);
	} 
	
	bullets++;
	if(bullets > MAX_NUM_BULLETS){
		bullets = 0;
	}
}

void printScore(){
	/* player score */
	printTextScreen(340,0,"Score:",RGB_WHITE);
	sprintf(playerScore,"%d",points);
	printTextScreen(440,0,playerScore,RGB_WHITE);
	/* High Score */
	printTextScreen(200,0,"High Score:",RGB_WHITE);
	if(hScore > points){
		/* print HighScore */
		sprintf(highScore,"%d",hScore);
		printTextScreen(300,0,highScore,RGB_RED);
	}else{
		/* player score is the high score! */
		printTextScreen(300,0,playerScore,RGB_YELLOW);
	}
}

int checkCollision(obj blt){
	int c_ctr;
	if(blt.isalive == 1){
		//blt.x = blt.x + (8 * 1.5F);
		for(c_ctr = 0; c_ctr < MAX_NUM_ENEMIES; c_ctr++){	
			if((blt.x >= enemy[c_ctr].x && blt.y < enemy[c_ctr].y + 20) && (blt.x < enemy[c_ctr].x + 42 && blt.y >= enemy[c_ctr].y)){
				enemy[c_ctr].isalive = 0;
				points = points + 10;
				blt.isalive = 0;
			}
		}
	}
	return(blt.isalive);
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
