#pragma warning(disable: 4996)

#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<process.h>
#include<conio.h>
#include<string.h>
#include"number.h"
#include"dice.h"

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32
#define ENTER 13
#define BACKSPACE 8
#define ESC 27
#define KEY_I 105
#define KEY_Q 113
#define KEY_R 114

typedef struct MAP {
	int x, y;
	int count;
	int** map;
}MAP;

// 전역 변수 선언부
const int GBOARD_ORIGIN_X = 8;
const int GBOARD_ORIGIN_Y = 4;
const int INIT_PAGE_WIDHT = 150;
const int INIT_PAGE_HEIGHT = 50;
const int SUB_GBOARD_WIDTH = 25;
const int SUB_GBOARD_HEIGHT = 20;
int CURRENT_CONSOLE_WIDTH = 0;

int ROUND = 0;
int MAXIMUM_ROUND = 20;
MAP* ALL_MAP;

// 함수 선언부
void SetCurrentCursorPos(int x, int y);
void SetCurrentCursorPos(COORD current);
COORD GetCurrentCursorPos(void);
void GetCurrentCursorPos(int& x, int & y);
void RemoveCursor(void);

void setConsoleSize(int cols, int lines);
void initPage();
void drawInitDefault();
void loadMap();
void helpPage();
void gameStart(int round);
void drawGamePage(MAP map);
void printMap(MAP map);
void drawDice(int num, int mainDice, int itemDice);
void drawRemainCount(int num);
void showCurrentRoundScore();
boolean showFinishPage();

/*================================================*/
/*=====================메인=======================*/
/*================================================*/

int main() {

	RemoveCursor();

	boolean check = true;

	while (check) {

		ROUND = 0;

		initPage();

		while (ROUND<MAXIMUM_ROUND) {
			gameStart(ROUND);

			getchar();

			showCurrentRoundScore();

			ROUND += 1;
		}

		check = showFinishPage();

	}
	
	exit(1);
	
	getchar();

	return 0;
}

/*================================================*/
/*================================================*/
/*================================================*/



// 초기 페이지
void initPage() {

	loadMap();

	drawInitDefault();

	while(1){


		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 4, INIT_PAGE_HEIGHT / 2+2);
		printf("◀  %d  ▶", ROUND+1);

		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				ROUND = (ROUND - 1);
				if (ROUND < 0)
					ROUND = MAXIMUM_ROUND-1;
				break;
			case RIGHT:
				ROUND = (ROUND + 1) % MAXIMUM_ROUND;
				break;
			case KEY_Q:
				helpPage();
				drawInitDefault();
				break;
			case SPACE:
			case ENTER:
				return;
				break;
			default:
				break;
			}

		}
	}
	
}


void drawInitDefault() {
	system("cls");
	setConsoleSize(INIT_PAGE_WIDHT, INIT_PAGE_HEIGHT);

	// 파일로부터 큰 제목 입력
	FILE *f;
	f = fopen("title.txt", "r");
	int x, y;
	fscanf(f, "%d", &x);
	fscanf(f, "%d", &y);

	int** str;
	str = (int**)malloc(sizeof(int*) *y);
	for (int i = 0; i < y; i++) {
		str[i] = (int*)malloc(sizeof(int)*x);
	}
	for (int j = 0; j < y; j++) {
		for (int i = 0; i < x; i++) {
			fscanf(f, "%d", &str[j][i]);
		}
	}

	fclose(f);

	// 큰제목 출력
	for (int j = 0; j < y; j++) {
		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - x, INIT_PAGE_HEIGHT / 2 - (y-j) - 4);
		for (int i = 0; i < x; i++) {
			if (str[j][i] == 0) {
				printf("　");
			}
			else {
				printf("■");
			}
		}
		printf("\n");
	}


	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 5, INIT_PAGE_HEIGHT / 2);
	printf("시작 라운드");


	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 4, INIT_PAGE_HEIGHT / 2 + 6);
	printf("Help : q");
	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 11, INIT_PAGE_HEIGHT / 2 + 8);
	printf("press any key to start");
}

void loadMap() {

	FILE *f;
	f = fopen("map.txt", "r");
	fscanf(f, "%d", &MAXIMUM_ROUND);

	ALL_MAP = (MAP*)malloc(sizeof(MAP)*MAXIMUM_ROUND);

	for (int i = 0; i < MAXIMUM_ROUND; i++) {
		fscanf(f, "%d", &ALL_MAP[i].x);
		fscanf(f, "%d", &ALL_MAP[i].y);
		fscanf(f, "%d", &ALL_MAP[i].count);

		ALL_MAP[i].map = (int**)malloc(sizeof(int*)*ALL_MAP[i].y);
		for (int k = 0; k < ALL_MAP[i].y; k++) {
			ALL_MAP[i].map[k] = (int*)malloc(sizeof(int)*ALL_MAP[i].x);
		}

		for (int y = 0; y < ALL_MAP[i].y; y++) {
			for (int x = 0; x < ALL_MAP[i].x; x++) {
				fscanf(f, "%d", &ALL_MAP[i].map[y][x]);
			}
		}

	}

	fclose(f);

}

void helpPage() {

	system("cls");
	SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y);
	printf("이곳은 도움말이 표시됩니다.");

	while (1) {
		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case BACKSPACE:
			case ESC:
				return;
			}

		}
	}
}

void gameStart(int round) {

	drawGamePage(ALL_MAP[round]);

}

void drawGamePage(MAP map) {

	system("cls");
	CURRENT_CONSOLE_WIDTH = (map.x * 2) + (GBOARD_ORIGIN_X) * 4 + SUB_GBOARD_WIDTH*2;
	int height = INIT_PAGE_HEIGHT;
	setConsoleSize(CURRENT_CONSOLE_WIDTH, height);

	SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y/2);
	printf("Round #%d", ROUND+1);

	printMap(map);

	// 중앙선
	for (int y = 0; y < height; y++) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - SUB_GBOARD_WIDTH*2 - (GBOARD_ORIGIN_X * 2), y);
		printf("│");
	}
	
	drawDice(map.count, 0, 0);

	drawRemainCount(0);
	

}

void drawDice(int num, int mainDice, int itemDice) {

	SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 18, GBOARD_ORIGIN_Y);
	printf("주사위를 굴릴 수 있는 남은 횟수 : %d", num);

	// 다이스 틀
	for (int y = 0; y < DICE_HEIGHT + 3; y++) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2, GBOARD_ORIGIN_Y + 2 + y);
		if (y == 0) {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0)
					printf("┌");
				else if (x == SUB_GBOARD_WIDTH - 1)
					printf("┐");
				else
					printf("─");
			}
		}
		else if (y == DICE_HEIGHT + 3 - 1) {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0)
					printf("└");
				else if (x == SUB_GBOARD_WIDTH - 1)
					printf("┘");
				else
					printf("─");
			}
		}
		else {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0 || x == SUB_GBOARD_WIDTH - 1)
					printf("│");
				else
					printf("　");
			}
		}

	}


	for (int y = 0; y < DICE_HEIGHT; y++) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6, GBOARD_ORIGIN_Y + 3 + y);

		for (int x = 0; x < DICE_WIDTH; x++) {
			if (dice[mainDice][y][x] == 0)
				printf("　");
			else if (dice[mainDice][y][x] == 1)
				printf("┌");
			else if (dice[mainDice][y][x] == 2)
				printf("┐");
			else if (dice[mainDice][y][x] == 3)
				printf("└");
			else if (dice[mainDice][y][x] == 4)
				printf("┘");
			else if (dice[mainDice][y][x] == 5)
				printf("─");
			else if (dice[mainDice][y][x] == 6)
				printf("│");
			else if (dice[mainDice][y][x] == 7)
				printf("●");
		}

	}
	SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 10, GBOARD_ORIGIN_Y + 3 + DICE_HEIGHT);
	printf("조작 횟수");
	for (int y = 0; y < DICE_HEIGHT; y++) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH *2, GBOARD_ORIGIN_Y + 3 + y);

		for (int x = 0; x < DICE_WIDTH; x++) {
			if (dice[itemDice][y][x] == 0)
				printf("　");
			else if (dice[itemDice][y][x] == 1)
				printf("┌");
			else if (dice[itemDice][y][x] == 2)
				printf("┐");
			else if (dice[itemDice][y][x] == 3)
				printf("└");
			else if (dice[itemDice][y][x] == 4)
				printf("┘");
			else if (dice[itemDice][y][x] == 5)
				printf("─");
			else if (dice[itemDice][y][x] == 6)
				printf("│");
			else if (dice[itemDice][y][x] == 7)
				printf("●");
		}
	}
	SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 11 + DICE_WIDTH * 2, GBOARD_ORIGIN_Y + 3 + DICE_HEIGHT);
	printf("아이템 번호");


	if (mainDice == 0) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 18, GBOARD_ORIGIN_Y + 2 + DICE_HEIGHT / 2+1);
		printf("Spacebar를 눌러 주사위를 굴려주세요.");
	}

}

void drawRemainCount(int num) {

	SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 10, GBOARD_ORIGIN_Y + 4 + DICE_HEIGHT+2);
	printf("남은 조작 횟수 %d번", num);


	for (int j = 0; j < BIG_NUMBER_HEIGHT; j++) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - BIG_NUMBER_WIDHT, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + j);
		for (int i = 0; i < BIG_NUMBER_WIDHT; i++) {
			if (bigNumber[num][j][i] == 0) {
				printf("　");
			}
			else {
				printf("■");
			}
		}
	}

}

void printMap(MAP map) {

	for (int y = 0; y < map.y; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
		for (int x = 0; x < map.x; x++) {
			switch (map.map[y][x]) {
			case 0:
				printf("　");
				break;
			case 1:
				printf("□");
				break;
			case 2:
				printf("■");
				break;
			case 3:
				printf("▣");
				break;
			case 4:
				printf("▩");
				break;
			case 5:
				printf("♠");
				break;
			case 6:
				printf("♤");
				break;
			case 7:
				printf("◀");
				break;
			case 8:
				printf("★");
				break;
			case 9:
				printf("●");
				break;
			default:
				printf("○");
				break;
			}
		}
	}

	
}

void showCurrentRoundScore() {

	int a = 10;
	int b = 20;
	int c = 30;

	system("cls");
	setConsoleSize(INIT_PAGE_WIDHT, INIT_PAGE_HEIGHT);

	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 18, INIT_PAGE_HEIGHT / 2 - 5);
	printf("부순 적 탱크 수 : %d  X  100   =  ", a);
	Sleep(800);
	printf("%d", a * 100);
	Sleep(800);
	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 18, INIT_PAGE_HEIGHT / 2);
	printf("부순 벽수 : %d  X  100  =  ", b);
	Sleep(800);
	printf("%d", b * 100);
	Sleep(800);
	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 18, INIT_PAGE_HEIGHT / 2 + 5);
	printf("남은 주사위 횟수 : %d  X  100  =  ", c);
	Sleep(800);
	printf("%d", c * 100);
	Sleep(500);

	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 14, INIT_PAGE_HEIGHT / 2 + 15);
	printf("Press any key to continue");

	while (1) {
		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			default:
				return;
			}

		}
	}

}

boolean showFinishPage() {

	system("cls");
	setConsoleSize(INIT_PAGE_WIDHT, INIT_PAGE_HEIGHT);


	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 4, INIT_PAGE_HEIGHT / 2);
	printf("클리어!!");


	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 14, INIT_PAGE_HEIGHT / 2 + 15);
	printf("Press any key to continue");

	while (1) {
		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case KEY_R:
				return true;
			default:
				return false;
			}

		}
	}
}

// 부가 함수
void println() {
	printf("\n");
}
void setConsoleSize(int cols, int lines) {

	char commend[40];

	sprintf(commend, "mode con:cols=%d lines=%d", cols, lines);

	system(commend);

}
// 제공받은 함수
void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void SetCurrentCursorPos(COORD current)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), current);
}

COORD GetCurrentCursorPos(void)
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}

void GetCurrentCursorPos(int& x, int & y) {

	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	x = curInfo.dwCursorPosition.X;
	y = curInfo.dwCursorPosition.Y;

}

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}