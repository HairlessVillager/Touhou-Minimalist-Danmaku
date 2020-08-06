// #define DEBUG
#include <graphics.h>
#include <math.h>
#include <ege/fps.h>    //includeing fps

#define PI 		M_PI
#define REIMUCOLOR	0xFC0A00;
#define SCREEN_WIDTH    480//384
#define SCREEN_HEIGHT   640//448
#define DANMAKU_MAX	1400
#define ENTITY_MAX	200
#define START_LIFE	5
#define FPS      	60

#define wait(N)		if(play(N)==1){return;}
#define rand(N)		(double) random(N)

#define WHITE		0xe9ecec
#define ORANGE		0xf07613
#define MAGENTA		0xbd44b3
#define LIGHT_BLUE	0x3aafd9
#define YELLOW		0xf8c627
#define LIME		0x70b919
#define PINK		0xed8dac
#define GRAY		0x3e4447
#define SILVER		0x8e8e86
#define CYAN		0x158991
#define PURPLE		0x792aac
#define BLUE		0x35399d
#define BROWN		0x724728
#define GREEN		0x546d1b
#define RED		0xa12722
#define BLACK		0x141519

typedef unsigned long long int ull;

typedef struct {
	int exi;
	unsigned long long int timer;

	double rad;
	int color;
	double pos_x;
	double pos_y;
	double vel_ang;
	double vel_mod;
} danmaku;

typedef struct {
	int exi;
	unsigned long long int timer;

	double rad;
	int color;
	double pos_x;
	double pos_y;
	int blood;
	int speNum;
	double vel_x;
	double vel_y;
	int frame_n;
} entity;

typedef struct{
	double pos_x;
	double pos_y;
	double speed;
	int blood;
	color_t color;
	double rad;
} player;

ull missNum;
ull gameTimer;
ull longestTime;

int getkeyEnable = 1;

danmaku danmakus[DANMAKU_MAX];
entity entitys[ENTITY_MAX];
player reimu;

void display();
void mainInit();
void process();
void miss();
void playerMove();
int input();
int play(int);

class enemy{
	public:
		enemy(){
			;
		}

		void init(
				double rad,
				int color,
				double pos_x,
				double pos_y,
				int blood,
				int speNum
				){
			int i;
			for(i = 0; i < ENTITY_MAX; i++){
				if(!entitys[i].exi){
					entitys[i].exi = 1;
					entitys[i].timer = 0;

					entitys[i].rad = rad;
					entitys[i].color = color;
					entitys[i].pos_x = pos_x + SCREEN_WIDTH / 2;
					entitys[i].pos_y = pos_y + SCREEN_HEIGHT / 2;
					entitys[i].blood = blood;
					entitys[i].speNum = speNum;

					entitys[i].vel_x = 0.0f;
					entitys[i].vel_y = 0.0f;
					entitys[i].frame_n = 0;
					break;
				}
			}
			entity_index = i;
		}

		void createSimple(
				double rad,
				int color,
				double pos_x,
				double pos_y,
				double vel_ang,
				double vel_mod
				){
			for(int i = 0; i < DANMAKU_MAX; i++){
				if(!danmakus[i].exi){
					danmakus[i].exi = 1;
					danmakus[i].timer = 0;

					danmakus[i].rad = rad;
					danmakus[i].color = color;
					danmakus[i].pos_x = pos_x + entitys[entity_index].pos_x;
					danmakus[i].pos_y = pos_y + entitys[entity_index].pos_y;
					danmakus[i].vel_ang = vel_ang;
					danmakus[i].vel_mod = vel_mod;

					return;
				}
			}
		}

		void createFacePlayer(
				double rad,
				int color,
				double pos_x,
				double pos_y,
				double vel_ang_delta,
				double vel_mod
				){
			for(int i = 0; i < DANMAKU_MAX; i++){
				if(!danmakus[i].exi){
					danmakus[i].exi = 1;
					danmakus[i].timer = 0;

					danmakus[i].rad = rad;
					danmakus[i].color = color;
					danmakus[i].pos_x = pos_x + entitys[entity_index].pos_x;
					danmakus[i].pos_y = pos_y + entitys[entity_index].pos_y;
					danmakus[i].vel_ang = atan((reimu.pos_y - pos_y - entitys[entity_index].pos_y) / (reimu.pos_x - pos_x - entitys[entity_index].pos_x)) + vel_ang_delta;
					danmakus[i].vel_mod = (reimu.pos_x >= pos_x + entitys[entity_index].pos_x) ? vel_mod : - vel_mod;

					return;
				}
			}
		}

		void move(
				double target_pos_x,
				double target_pos_y,
				int frame_n
			      ){
			entitys[entity_index].vel_x = (target_pos_x + SCREEN_WIDTH / 2 - entitys[entity_index].pos_x) / frame_n;
			entitys[entity_index].vel_y = (target_pos_y + SCREEN_HEIGHT / 2 - entitys[entity_index].pos_y) / frame_n;
			entitys[entity_index].timer = frame_n;
		}

		void free(){
			entitys[entity_index].exi = 0;
		}

		~enemy(){
			entitys[entity_index].exi = 0;
		}
	private:
		int entity_index;
};

int play(int n)
{
	for(int j = 0; j < n; delay_fps(FPS), j++){
		if(getkeyEnable){
			if(keystate(key_esc)){
				return 1;
			}
			playerMove();
		}
#ifndef DEBUG
		if(missNum >= START_LIFE){
			return 1;
		}
#endif
		longestTime = (gameTimer > longestTime) ? gameTimer : longestTime;
		display();
		for(int i = 0; i < DANMAKU_MAX; i++){
			if(danmakus[i].exi){
				/* run danmaku*/
				danmakus[i].pos_x += danmakus[i].vel_mod * cos(danmakus[i].vel_ang);
				danmakus[i].pos_y += danmakus[i].vel_mod * sin(danmakus[i].vel_ang);
				danmakus[i].timer++;

				/* clean danmaku */
				if(0 >= danmakus[i].pos_x || danmakus[i].pos_x >= SCREEN_WIDTH || 0 >= danmakus[i].pos_y || danmakus[i].pos_y >= SCREEN_HEIGHT) {
					danmakus[i].exi = 0;
				}

				/* miss decide */
				if((danmakus[i].pos_x - reimu.pos_x) * (danmakus[i].pos_x - reimu.pos_x) + (danmakus[i].pos_y - reimu.pos_y) * (danmakus[i].pos_y - reimu.pos_y) <= (danmakus[i].rad + reimu.rad - 1) * (danmakus[i].rad + reimu.rad - 1)){
					missNum++;
					danmakus[i].exi = 0;
#ifndef DEBUG
					miss();
#endif
				}
			}
		}
		for(int i = 0; i < ENTITY_MAX; i++){
			if(entitys[i].exi){
				/* run entity */
				if(entitys[i].timer > 0){
					entitys[i].pos_x += entitys[i].vel_x;
					entitys[i].pos_y += entitys[i].vel_y;
					entitys[i].timer--;
				}
			}
		}
		gameTimer++;
	}
	return 0;
}

void mainInit()
{
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT, INIT_RENDERMANUAL);
	setrendermode(RENDER_MANUAL);
	setcaption("Easy STG by HairlessVillager");
	//ege_enable_aa(true);
	randomize();
	setbkcolor(EGEACOLOR(0XFF, 0x0f0f0f));
	setfont(16, 0, "ËÎÌו");
	setbkmode(TRANSPARENT);

	missNum = 0;
	gameTimer = 0;
	longestTime = 0;

	for(int i = 0; i < DANMAKU_MAX; i++){
		danmakus[i].exi = 0;
	}
	for(int i = 0; i < ENTITY_MAX; i++){
		entitys[i].exi = 0;
	}

	reimu.pos_x = SCREEN_WIDTH * 0.5f;
	reimu.pos_y = SCREEN_HEIGHT * 0.8f;
	reimu.color = REIMUCOLOR;
	reimu.rad = 2.0f;
}

int input()
{
	setfillcolor(WHITE);
	for(int i = 1; i <= 30; delay_fps(FPS), i++){
		cleardevice();
		xyprintf(0, 0, "Press <Arrow> to move.");
		xyprintf(0, 20, "Press <Shift> to be slow.");
		xyprintf(0, 40, "Press <Esc> to exit.");
		xyprintf(0, 60, "Press <Z> to start the game...");
		bar(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 10 + 200, SCREEN_WIDTH / 2 + 120 - i * 8, SCREEN_HEIGHT / 2 + 10 + 200);
	}
	flushkey();
	while(1){
		if(keystate(90)){
			return 1;
		}
		else if(keystate(key_esc)){
			return 0;
		}
		delay_fps(FPS);
	}
}

void playerMove()
{
	if(keystate(key_shift)) {
		reimu.speed = 2.4f;
	} else {
		reimu.speed = 5.0f;
	}

	int directKeys[4] = { key_left, key_up, key_right, key_down };
	double speed_xs[] = {- reimu.speed, 0.0f, reimu.speed, 0.0f};
	double speed_ys[] = {0.0f, - reimu.speed, 0.0f, reimu.speed};

	double p_next_pos_x;
	double p_next_pos_y;
	p_next_pos_x = reimu.pos_x;
	p_next_pos_y = reimu.pos_y;

	for (int i = 0; i < 4; i++) {
		if (keystate(directKeys[i])) {
			p_next_pos_x += speed_xs[i];
			p_next_pos_y += speed_ys[i];
		}
	}
	if (p_next_pos_x != reimu.pos_x || p_next_pos_y != reimu.pos_y) {
		if(reimu.rad <= p_next_pos_x && p_next_pos_x <= (SCREEN_WIDTH - reimu.rad)) {
			reimu.pos_x = p_next_pos_x;
		}
		if(reimu.rad <= p_next_pos_y && p_next_pos_y <= (SCREEN_HEIGHT - reimu.rad)) {
			reimu.pos_y = p_next_pos_y;
		}
	}
}

void miss()
{
	getkeyEnable = 0;

	for(int i = 0; i < 4; i++){
		setfillcolor(EGEACOLOR(0xff, 0x0f0f0f));
		ege_fillellipse(reimu.pos_x - 11, reimu.pos_y - 11, 22, 22);
		delay_fps(FPS);
		delay_fps(FPS);
		delay_fps(FPS);
		delay_fps(FPS);
		delay_fps(FPS);

		setfillcolor(EGEACOLOR(0xff, reimu.color));
		ege_fillellipse(reimu.pos_x - 11, reimu.pos_y - 11, 22, 22);
		delay_fps(FPS);
		delay_fps(FPS);
		delay_fps(FPS);
		delay_fps(FPS);
		delay_fps(FPS);
	}
	for(int i = 0; i < DANMAKU_MAX; i++){
		danmakus[i].exi = 0;
	}

	gameTimer = 0;

	reimu.pos_x = SCREEN_WIDTH * 0.5f;
	reimu.pos_y = SCREEN_HEIGHT * 0.8f;

	getkeyEnable = 1;
}

void display()
{
	cleardevice();

#ifdef DEBUG
	xyprintf(0, SCREEN_HEIGHT - 20, "Debug Version");
#endif

	for(int i = 0; i < ENTITY_MAX; i++){
		if(entitys[i].exi){
			setfillcolor(EGEACOLOR(0xff, entitys[i].color));
			ege_fillellipse(entitys[i].pos_x - entitys[i].rad, entitys[i].pos_y - entitys[i].rad, 2 * entitys[i].rad, 2 * entitys[i].rad);
		}
	}

	setfillcolor(EGEACOLOR(0xff, reimu.color));
	ege_fillellipse(reimu.pos_x - 10, reimu.pos_y - 10, 2 * 10, 2 * 10);
	setfillcolor(EGEACOLOR(0xff, 0xffffff));
	ege_fillellipse(reimu.pos_x - reimu.rad, reimu.pos_y - reimu.rad, 2 * reimu.rad, 2 * reimu.rad);

	for(int i = 0; i < DANMAKU_MAX; i++) {
		if(danmakus[i].exi) {
			setfillcolor(EGEACOLOR(0xff, danmakus[i].color));
			ege_fillellipse(danmakus[i].pos_x - danmakus[i].rad, danmakus[i].pos_y - danmakus[i].rad, 2 * danmakus[i].rad, 2 * danmakus[i].rad);
		}
	}

	xyprintf(0, 0, "time : %.2f s", (double) gameTimer / FPS);
	xyprintf(0, 20, "miss : %lld", missNum);
	xyprintf(0, 40, "longest live time : %.2f s", (double) longestTime / FPS);
	xyprintf(0, 60, "fps %.2f", getfps());

}

#include "process.cpp"
/*
   void process(){
   ...
   }
   */

void winDecide(){
	flushkey();
	for(;;delay_fps(FPS)){
		if(keystate(90)){
			break;
		}
		cleardevice();
		if(missNum < START_LIFE){
			xyprintf(0, 0, "You Win!");
			xyprintf(0, 40, "miss : %d", missNum);
		}
		else{
			xyprintf(0, 0, "You're Covered.");
		}
		xyprintf(0, 20, "longest live time : %.2f s", (float) longestTime / FPS);
		xyprintf(0, 60, "Press <Z> to continue...");
#ifdef DEBUG
		xyprintf(0, SCREEN_HEIGHT - 20, "Debug Version");
#endif
	}
}

int main()
{
	while(1){
		mainInit();
		if(input()){
			process();
		}
		else{
			break;
		}
		winDecide();
	}
	return 0;
}

