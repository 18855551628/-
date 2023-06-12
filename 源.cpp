/*
�������ܿ���
1.������Ŀ
2.�����ز�
3.��������
	a.������Ϸ����
	b.���ر�����Դ(tools.h���޸�ͼƬ����ͷ�ļ������˴��еģ�
	c.��ɫ����
	d.�����Ծ
	e:����¶�
	f.����ϰ���
	g.��ײ���
	e.��¼����
*/

#include<stdio.h>
#include<easyx.h> //ͼ�ο⺯��
#include"tools.h"
#include<conio.h>
#include<vector> //c++�ṩ���ȿɱ������
using namespace std;

#define win_width 1012
#define win_height 396
#define barrier_count 10



//����ͼƬ
IMAGE img[3];

//��ɫͼƬ
IMAGE imghero[12];
int herox; //����x����
int heroy; //����y����
int heroindex;//��ұ��ܵ����

//IMAGE imgwugui;
//int wuguix; //�ڹ��x����
//int wuguiy; //�ڹ��y����
//bool wuguiexist; //��ǰ�Ƿ���С�ڹ� ��ɾȥ�������Ѷ�
//int wuguiindex; //�ڹ����


int blood;//���Ѫ��
int score; //����

typedef enum {   //ö��   �Ż��ṹ���װ�Ż��ϰ���
	wugui, //0
	lion,  //1
	hook1,
	hook2,
	hook3,
	hook4,
	barrier_typeCount,
}barrier_type;


vector<vector<IMAGE>> barrierImgs; //����ϰ���ͼƬ    ��ά�ɱ�����

typedef struct barrier {
	int type;//�ϰ��������
	int imgindex; //��ǰ��ʾ��ͼƬ���
	int x, y; //�ϰ��������
	int speed;
	int power; //�˺�
	bool exist;
	bool hited; //�ж��Ƿ���ײ��
	bool passed; //�ж��Ƿ�ͨ���ϰ���
}barrier_t;

barrier_t barrierNum[barrier_count];

IMAGE imgDown[2];
bool herodown;//��ʾ������ڶ�

bool herojump; //��ʾ���������Ծ

int jumpheightmax;
int herojumpoff; //��Ծƫ����
int update; //��ʾ�Ƿ�Ҫ����ˢ�»���

int bgx[3];//����ͼƬx����

//int a = 2;
//int b = 3;
//int c = 8;
int bgspeed[3] = { 2,3,8 };


IMAGE imgsz[10];

//��Ϸ��ʼ��
void origin()
{
	initgraph(win_width, win_height,EW_SHOWCONSOLE);

	//���ر�����Դ
	char name[64];
	
	for (int i = 0; i < 3; i++) {
		sprintf(name, "res/bg%03d.png", i + 1);
		loadimage(&img[i], name);
		bgx[i] = 0;
	}

	//���ؽ�ɫ��Դ
	for (int i = 0; i < 12; i++) {
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&imghero[i], name);
	}

	//������ҵĳ�ʼλ��,��������㷨
	herox = win_width * 0.5 - imghero[0].getwidth() * 0.5;
	heroy = 345 - imghero[0].getheight();
	heroindex = 0;
	herojump = false;
	jumpheightmax = 345 - imghero[0].getheight() - 120; //Ϊɶ�Ǽ�120����Ϊ���곯������
	herojumpoff = -11;

	update = true;

	//����С�ڹ�
	//loadimage(&imgwugui,"res/t1.png");
	//wuguiexist = false;
	//wuguiy = 345 - imgwugui.getheight()+5;
	IMAGE imgWugui;
	loadimage(&imgWugui, "res/t1.png");
	vector<IMAGE>imgWuguiArray;
	imgWuguiArray.push_back(imgWugui); //��ͼƬ��ŵ�imgWuguiArray������ȥ
	barrierImgs.push_back(imgWuguiArray);

	IMAGE imgLion;
	vector<IMAGE>imgLionArray;
	for (int i = 0; i < 6; i++) {
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	barrierImgs.push_back(imgLionArray);

	//��ʼ���ϰ����
	for (int i = 0; i < barrier_count; i++) {
		barrierNum[i].exist = false;
	}

	//�����¶�
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

	//Ԥ������Ч
	preLoadSound("res/hit.mp3");

	mciSendString("play res/bg.mp3", 0, 0, 0);
	score = 0;

	//��������ͼƬ
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
	barrierNum[i].type = (barrier_type)(rand() % 3); //��������ϰ���
	if (barrierNum[i].type == hook1) {
		barrierNum[i].type += rand() % 4;
	}
	barrierNum[i].x = win_width;
	barrierNum[i].y = 345 - barrierImgs[barrierNum[i].type][0].getheight() + 5;
	 
	if (barrierNum[i].type == wugui) {   //���ˣ����������ںź�����һ���磬һֱ��bug��ԭ���������������
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

void checkHit() {  //�жϾ����Ƿ��ཻ
	for (int i = 0; i < barrier_count; i++) {
		if (barrierNum[i].exist && barrierNum[i].hited == false) {
			int a1x, a1y, a2x, a2y;
			int off = 35;//����ƫ��
			if (!herodown) {
				a1x = herox+off;//����
				a1y = heroy + off;
				a2x = herox + imghero[heroindex].getwidth() - off;//����
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

void motion() {   //ͼƬ�ƶ���ÿ��ͼƬ�ƶ����ٶȲ�һ��
	for (int i = 0; i < 3; i++) {
		bgx[i] -= bgspeed[i];
		if (bgx[i] < -win_width) {
			bgx[i] = 0;
		}
	}
	//ʵ����Ծ
	if (herojump) {   //���⣺��ɫ�����ԾȻ��������  ����1������һ����ߵ㣬��ɫ������ߵ㣬�ͻ����£�
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
		
	else{ //����Ծ
		heroindex = (heroindex + 1) % 12;
	}

	//�����ϰ���
	static int frameCount = 0;
	static int enemyFre = 50; //����Ƶ��
	frameCount++;
	if (frameCount > enemyFre) {
		frameCount = 0;
		enemyFre = 50 + rand() % 50;
		creatbarrier();
	}

	//�����ϰ�������
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

	//��ײ���
	checkHit();
}
//����
void ori_bg() {                        //���ر���
	putimagePNG2(bgx[0], 0, &img[0]);
	putimagePNG2(bgx[1], 119, &img[1]);
	putimagePNG2(bgx[2], 330, &img[2]);
}

//�����Ծ
void jump() {
	herojump = true;
	update = true; //��Ծ��ˢ�»���
}

void down() {
	herodown = true;
	update = true;
	heroindex = 0;
}

//���̿���
void keybroad() {
	char ch;
	if (_kbhit()) {  //��������1
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

//�ϰ���
void updateEnemy() {
	//С�ڹ�
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

void checkover() { //�ж���Ϸ����
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

//������
int main() {
	origin();
	
	loadimage(0, "res/start.png");
	system("pause");
	int timer = 0;//�Ż�֡��,��ʱ��

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
			//putimagePNG2(herox, heroy, &imghero[heroindex]); //���ﱼ��    ����Ϊ����
			updateHero();
			updateEnemy();
			updateScore();
			EndBatchDraw();
			checkover();
			checkscore();
			motion();
		}

		//Sleep(20);//���ߣ���������
	}
	system("pause");
	return 0;
}