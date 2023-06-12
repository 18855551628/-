/*
坤坤快跑开发
1.创建项目
2.导入素材
3.开发流程
	a.创建游戏界面
	b.加载背景资源(tools.h是修复图片工具头文件，借了大佬的）
	c.角色奔跑
	d.玩家跳跃
	e:玩家下蹲
	f.添加障碍物
	g.碰撞检测
	e.记录分数
*/

#include<stdio.h>
#include<easyx.h> //图形库函数
#include"tools.h"
#include<conio.h>
#include<vector> //c++提供长度可变的数组
using namespace std;

#define win_width 1012
#define win_height 396
#define barrier_count 10



//背景图片
IMAGE img[3];

//角色图片
IMAGE imghero[12];
int herox; //人物x坐标
int heroy; //人物y坐标
int heroindex;//玩家奔跑的序号

//IMAGE imgwugui;
//int wuguix; //乌龟的x坐标
//int wuguiy; //乌龟的y坐标
//bool wuguiexist; //当前是否有小乌龟 可删去，增加难度
//int wuguiindex; //乌龟序号


int blood;//玩家血量
int score; //分数

typedef enum {   //枚举   优化结构体封装优化障碍物
	wugui, //0
	lion,  //1
	hook1,
	hook2,
	hook3,
	hook4,
	barrier_typeCount,
}barrier_type;


vector<vector<IMAGE>> barrierImgs; //存放障碍物图片    二维可变数组

typedef struct barrier {
	int type;//障碍物的类型
	int imgindex; //当前显示的图片序号
	int x, y; //障碍物的坐标
	int speed;
	int power; //伤害
	bool exist;
	bool hited; //判断是否碰撞了
	bool passed; //判断是否通过障碍物
}barrier_t;

barrier_t barrierNum[barrier_count];

IMAGE imgDown[2];
bool herodown;//表示玩家正在蹲

bool herojump; //表示玩家正在跳跃

int jumpheightmax;
int herojumpoff; //跳跃偏移量
int update; //表示是否要马上刷新画面

int bgx[3];//背景图片x坐标

//int a = 2;
//int b = 3;
//int c = 8;
int bgspeed[3] = { 2,3,8 };


IMAGE imgsz[10];

//游戏初始化
void origin()
{
	initgraph(win_width, win_height,EW_SHOWCONSOLE);

	//加载背景资源
	char name[64];
	
	for (int i = 0; i < 3; i++) {
		sprintf(name, "res/bg%03d.png", i + 1);
		loadimage(&img[i], name);
		bgx[i] = 0;
	}

	//加载角色资源
	for (int i = 0; i < 12; i++) {
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&imghero[i], name);
	}

	//设置玩家的初始位置,人物居中算法
	herox = win_width * 0.5 - imghero[0].getwidth() * 0.5;
	heroy = 345 - imghero[0].getheight();
	heroindex = 0;
	herojump = false;
	jumpheightmax = 345 - imghero[0].getheight() - 120; //为啥是减120，因为坐标朝下是正
	herojumpoff = -11;

	update = true;

	//加载小乌龟
	//loadimage(&imgwugui,"res/t1.png");
	//wuguiexist = false;
	//wuguiy = 345 - imgwugui.getheight()+5;
	IMAGE imgWugui;
	loadimage(&imgWugui, "res/t1.png");
	vector<IMAGE>imgWuguiArray;
	imgWuguiArray.push_back(imgWugui); //把图片存放到imgWuguiArray数组中去
	barrierImgs.push_back(imgWuguiArray);

	IMAGE imgLion;
	vector<IMAGE>imgLionArray;
	for (int i = 0; i < 6; i++) {
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	barrierImgs.push_back(imgLionArray);

	//初始化障碍物池
	for (int i = 0; i < barrier_count; i++) {
		barrierNum[i].exist = false;
	}

	//加载下蹲
	loadimage(&imgDown[0], "res/d1.png");
	loadimage(&imgDown[1], "res/d2.png");
	herodown = false;

	IMAGE imgH;
	for (int i = 0; i < 4; i++) {
		vector<IMAGE>imgHookArray;
		sprintf(name, "res/h%d.png", i + 1);
		loadimage(&imgH, name, 63, 260,true);
		imgHookArray.push_back(imgH);
		barrierImgs.push_back(imgHookArray);
	}

	blood = 100;

	//预加载音效
	preLoadSound("res/hit.mp3");

	mciSendString("play res/bg.mp3", 0, 0, 0);
	score = 0;

	//加载数字图片
	for (int i = 0; i < 10; i++) {
		sprintf(name, "res/sz/%d.png", i);
		loadimage(&imgsz[i], name);
	}

}

void creatbarrier() {
	int i;
	for (i = 0; i < barrier_count; i++) {
		if (barrierNum[i].exist == false) {
			break;
		}
	}
	if (i >= barrier_count) {
		return;
	}

	barrierNum[i].exist = true;
	barrierNum[i].hited = false;
	barrierNum[i].imgindex = 0;
	barrierNum[i].type = (barrier_type)(rand() % 3); //随机出现障碍物
	if (barrierNum[i].type == hook1) {
		barrierNum[i].type += rand() % 4;
	}
	barrierNum[i].x = win_width;
	barrierNum[i].y = 345 - barrierImgs[barrierNum[i].type][0].getheight() + 5;
	 
	if (barrierNum[i].type == wugui) {   //服了，这两个等于号耗了我一下午，一直出bug，原来在这里！！！！！
		barrierNum[i].speed = 0;
		barrierNum[i].power = 100;
	}
	else if (barrierNum[i].type == lion) {  
		barrierNum[i].speed = 4;
		barrierNum[i].power = 100;
	}
	else if (barrierNum[i].type >= hook1 && barrierNum[i].type<=hook4) {
		barrierNum[i].speed = 0;
		barrierNum[i].power = 100;
		barrierNum[i].y = 0;
	}

	barrierNum[i].passed = false;
}

void checkHit() {  //判断矩形是否相交
	for (int i = 0; i < barrier_count; i++) {
		if (barrierNum[i].exist && barrierNum[i].hited == false) {
			int a1x, a1y, a2x, a2y;
			int off = 35;//设置偏差
			if (!herodown) {
				a1x = herox+off;//左上
				a1y = heroy + off;
				a2x = herox + imghero[heroindex].getwidth() - off;//右下
				a2y = heroy + imghero[heroindex].getheight();
			}
			else {
				a1x = herox + off;
				a1y = 345 - imgDown[heroindex].getheight();
				a2x = herox + imgDown[heroindex].getwidth() - off;
				a2y = 345;
			}

			int b1x = barrierNum[i].x + off;
			int b1y = barrierNum[i].y + off;
			int b2x = barrierNum[i].x + barrierImgs[barrierNum[i].type][barrierNum[i].imgindex].getwidth() - off;
			int b2y = barrierNum[i].y + barrierImgs[barrierNum[i].type][barrierNum[i].imgindex].getheight() - 10;

			if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y)) {
				blood -= barrierNum[i].power;
				playSound("res/hit.mp3");
				barrierNum[i].hited = true;
			}
		}
	}
}

void motion() {   //图片移动，每个图片移动的速度不一样
	for (int i = 0; i < 3; i++) {
		bgx[i] -= bgspeed[i];
		if (bgx[i] < -win_width) {
			bgx[i] = 0;
		}
	}
	//实现跳跃
	if (herojump) {   //问题：角色如何跳跃然后在落下  调试1；设立一个最高点，角色到达最高点，就会落下；
		if (heroy < jumpheightmax) {
			herojumpoff = 10;
		}

		heroy += herojumpoff;

		if (heroy > 345 - imghero[0].getheight()) {
			herojump = false;
			herojumpoff = -10;
		}
	}
	else if (herodown) {
		static int count = 0;
		int delays[2] = { 2,20 };
		count++;
		if (count >= delays[heroindex]) {
			count = 0;
			heroindex++;
				if (heroindex >= 2) {
					heroindex = 0;
						herodown = false;
			}
		}
	}
		
	else{ //不跳跃
		heroindex = (heroindex + 1) % 12;
	}

	//生成障碍物
	static int frameCount = 0;
	static int enemyFre = 50; //出现频率
	frameCount++;
	if (frameCount > enemyFre) {
		frameCount = 0;
		enemyFre = 50 + rand() % 50;
		creatbarrier();
	}

	//更新障碍物坐标
	for (int i = 0; i < barrier_count; i++) {
		if (barrierNum[i].exist) {
			barrierNum[i].x -= barrierNum[i].speed + bgspeed[2];
			if (barrierNum[i].x < -barrierImgs[barrierNum[i].type][0].getwidth() * 2) {
				barrierNum[i].exist = false;
			}
			int len = barrierImgs[barrierNum[i].type].size();
			barrierNum[i].imgindex = (barrierNum[i].imgindex + 1) % len;
		}
	}

	//碰撞检测
	checkHit();
}
//背景
void ori_bg() {                        //三重背景
	putimagePNG2(bgx[0], 0, &img[0]);
	putimagePNG2(bgx[1], 119, &img[1]);
	putimagePNG2(bgx[2], 330, &img[2]);
}

//玩家跳跃
void jump() {
	herojump = true;
	update = true; //跳跃就刷新画面
}

void down() {
	herodown = true;
	update = true;
	heroindex = 0;
}

//键盘控制
void keybroad() {
	char ch;
	if (_kbhit()) {  //按键反馈1
		ch = _getch();
		switch (ch) {
		case 'w':
		case 'W':
		case ' ':
			jump();
			break;
		case 80:
		case 's':
		case 'S':
			down();
			break;
		}
	}
}

//障碍物
void updateEnemy() {
	//小乌龟
	//if (wuguiexist) {
		//putimagePNG2(wuguix, wuguiy, win_width, &imgWugui);
	//}
	for (int i = 0; i < barrier_count; i++) {
		if (barrierNum[i].exist) {
			putimagePNG2(barrierNum[i].x, barrierNum[i].y, win_width, &barrierImgs[barrierNum[i].type][barrierNum[i].imgindex]);
		}
	}
}

void updateHero() {
	if (!herodown) {
		putimagePNG2(herox, heroy, &imghero[heroindex]);
	}
	else {
		int Y = 345 - imgDown[heroindex].getheight();
	    putimagePNG2(herox, Y, &imgDown[heroindex]);
	}
}

void checkover() { //判断游戏结束
	if (blood <= 0) {
		loadimage(0, "res/over.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
		origin();
		blood = 100;
		mciSendString("play res/bg.mp3", 0, 0, 0);
	}
}

void checkscore() {
	for (int i= 0; i < barrier_count; i++) {
		if (barrierNum[i].exist && barrierNum[i].passed == false && barrierNum[i].x + barrierImgs[barrierNum[i].type][0].getwidth() < herox) {
			score++;
			
			barrierNum[i].passed = true;
			printf("score: %d\n", score);
		}
	}
}

void updateScore() {
	char str[8];
	sprintf(str, "%d", score);

	int x = 20;
	int y = 25;
	for (int i = 0; str[i]; i++) {
		int sz = str[i] - '0';
		putimagePNG(x, y, &imgsz[sz]);
		x += imgsz[sz].getwidth() + 5;
	}
}

//主程序
int main() {
	origin();
	
	loadimage(0, "res/start.png");
	system("pause");
	int timer = 0;//优化帧率,计时器

	while (1) {
		keybroad();
		timer += getDelay(); //10ms
		if (timer > 30) {
			timer = 0;
			update = true;
		}

		if (update) {
			update = false;
			BeginBatchDraw();
			ori_bg();
			//putimagePNG2(herox, heroy, &imghero[heroindex]); //人物奔跑    更改为函数
			updateHero();
			updateEnemy();
			updateScore();
			EndBatchDraw();
			checkover();
			checkscore();
			motion();
		}

		//Sleep(20);//休眠，不建议用
	}
	system("pause");
	return 0;
}