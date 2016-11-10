#pragma warning(disable: 4996)

#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<process.h>
#include<conio.h>
#include<string.h>
#include<time.h>
#include"number.h"
#include"dice.h"

#define DIRECTION_LEFT 1
#define DIRECTION_RIGHT 2
#define DIRECTION_UP 3
#define DIRECTION_DOWN 4

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

#define FONT_DEFAULT_COLOR 7
#define FONT_RED_COLOR 4
#define FONT_LIGHT_RED_COLOR 12
#define FONT_GREED_COLOR 2
#define FONT_BLUE_COLOR 1
#define PLAYER_COLOR 9


typedef struct MAP {
	int x, y;
	int startX, startY;
	int count;
	int** map;
}MAP;

typedef struct NPC {
	int x, y;
	int direction;
	int remainMove;
	int number;
	int check[5] = { 0 };
	NPC * leftLink;
	NPC * rightLink;
}NPC;

typedef struct NPC_HEAD {
	int num=0;
	NPC * head;
	NPC * tail;
}NPC_HEAD;

typedef struct MISSILE {
	int x, y;
	int direction;
	boolean isEnable;
	MISSILE * next;
}MISSILE;

typedef struct SCORE {
	int number;
	int count;
	SCORE* next;
}SCORE;

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
int mainDiceNumber = 0;
int itemDiceNumber = 0;
int diceEnableNumber = 0;
int myX = 0;
int myY = 0;
boolean enableDice = false;
boolean enableMoveNPC = false;
boolean isGameOver = false;
boolean isSuccess = false;
boolean isNeedPrintMap = false;
MAP* ALL_MAP = NULL;
NPC_HEAD* NPC_LIST = NULL;
MISSILE* ALL_MISSILE = NULL;
SCORE* SCORE_LIST = NULL;

// 아이템 관련 전역 변수
int NPC_SPEED = 40;
int currentTankDirection = DIRECTION_UP;
int restrictSightRange = 2;
int remainDiceNumberByNPCSpeed = 0;
int remainDiceNumberBySight = 0;
int remainDiceNumberByNPCStop = 0;
int remainDiceNumberByTankUnbeatable = 0;
boolean enableSight = true;
boolean isAlreadyNPCSpeed = false;
boolean isAlreadySight = false;
boolean isAlreadyNPCStop = false;
boolean isAlreadyTankUnbeatable = false;
boolean isNPCStop = false;
boolean isTankUnbeatable = false;

// 함수 선언부
void SetCurrentCursorPos(int x, int y);
void SetCurrentCursorPos(COORD current);
COORD GetCurrentCursorPos(void);
void GetCurrentCursorPos(int& x, int & y);
void RemoveCursor(void);
void setFontColor(int color);

void setConsoleSize(int cols, int lines);
void initPage();
void drawInitDefault();
void removeAllNPCList();
void loadMap();
void addNPC(NPC_HEAD& npc_list, int x, int y, int number);
void removeNPC(int x, int y, boolean isDelete);
void helpPage();
void gameStart();
void drawGamePage();
void removeWall(int x, int y);
boolean moveTank(int direction);
boolean moveNPC();
void removeAllMissile();
void addMissile(int x, int y, int direction);
void addScore(int number);
void removeAllScore();
void printMissile();
void moveMissile();
boolean detectConflictWithTank(int checkX, int checkY);
boolean detectConflictWithNPC(int checkX, int checkY);
boolean detectConflictWithWall(int checkX, int checkY);
boolean detectConflictWithItem(int checkX, int checkY);
void useItem();
void NPCSpeedUp(void * param);
void restrictSight(void * param);
void NPCStop(void * param);
void tankUnbeatable(void * param);
void mapDestroy(boolean isUp);
void addNewNPC();
void removeOneNPC();
void printTank();
void printNPC();
void printMap();
void drawDice();
void reduceControlPoint();
void reduceControlPoint(int direction);
void drawRemainCount();
void showCurrentRoundScore(boolean success);
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
			gameStart();


			showCurrentRoundScore(isSuccess);

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
			case KEY_I:
				helpPage();
				drawInitDefault();
				break;
			case KEY_Q:
				exit(1);
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

// 초기 페이지를 그림
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
	printf("Help : i");
	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 4, INIT_PAGE_HEIGHT / 2 + 8);
	printf("Quit : q");
	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 11, INIT_PAGE_HEIGHT / 2 + 10);
	printf("press any key to start");
}

// 데이터로부터 맵을 로딩
void loadMap() {

	FILE *f;
	f = fopen("map.txt", "r");
	fscanf(f, "%d", &MAXIMUM_ROUND);

	if (ALL_MAP != NULL) {
		free(ALL_MAP);
	}
	ALL_MAP = (MAP*)malloc(sizeof(MAP)*MAXIMUM_ROUND);
	removeAllNPCList();
	NPC_LIST = (NPC_HEAD*)malloc(sizeof(NPC_HEAD)*MAXIMUM_ROUND);

	for (int i = 0; i < MAXIMUM_ROUND; i++) {
		fscanf(f, "%d", &ALL_MAP[i].x);
		fscanf(f, "%d", &ALL_MAP[i].y);
		fscanf(f, "%d", &ALL_MAP[i].count);

		// 맵 초기화 구문
		ALL_MAP[i].map = (int**)malloc(sizeof(int*)*ALL_MAP[i].y);
		for (int k = 0; k < ALL_MAP[i].y; k++) {
			ALL_MAP[i].map[k] = (int*)malloc(sizeof(int)*ALL_MAP[i].x);
		}


		// NPC_LIST 초기화 구문
		NPC_LIST[i].num = 0;
		NPC_LIST[i].head = NULL;
		NPC_LIST[i].tail = NULL;


		// 맵 탐색 구문
		for (int y = 0; y < ALL_MAP[i].y; y++) {
			for (int x = 0; x < ALL_MAP[i].x; x++) {
				int number;
				fscanf(f, "%d", &number);

				if (number == 5 || number == 6) {
					ALL_MAP[i].map[y][x] = 0;
					addNPC(NPC_LIST[i], x, y, number);
				}
				else if (number == 7) {
					ALL_MAP[i].map[y][x] = 0;
					ALL_MAP[i].startX = x;
					ALL_MAP[i].startY = y;
				}
				else {
					ALL_MAP[i].map[y][x] = number;
				}
				
			}
		}

	}

	fclose(f);

}

void addMissile(int x, int y, int direction) {

	if (ALL_MISSILE == NULL) {
		ALL_MISSILE = (MISSILE*)malloc(sizeof(MISSILE));
		ALL_MISSILE->x = x;
		ALL_MISSILE->y = y;
		ALL_MISSILE->isEnable = true;
		ALL_MISSILE->direction = direction;
		ALL_MISSILE->next = NULL;
	}
	else {

		MISSILE* temp = ALL_MISSILE;

		while (temp != NULL) {

			if (!temp->isEnable) {
				temp->x = x;
				temp->y = y;
				temp->direction = direction;
				temp->isEnable = true;
				return;
			}

			temp = temp->next;

		}

		temp = (MISSILE*)malloc(sizeof(MISSILE));
		temp->x = x;
		temp->y = y;
		temp->direction = direction;
		temp->isEnable = true;
		temp->next = ALL_MISSILE->next;
		ALL_MISSILE->next = temp;



	}

}

void removeAllMissile() {

	MISSILE *temp = ALL_MISSILE;
	MISSILE *temp1;

	if (temp == NULL) {
		return;
	}

	while (temp != NULL) {

		temp1 = temp->next;
		free(temp);
		temp = temp1;

	}

	ALL_MISSILE = NULL;
}

// NPC를 원형 연결리스트에 추가
void addNPC(NPC_HEAD& npc_list, int x, int y, int number) {

	npc_list.num += 1;
	int check[5] = { 0 };

	if (npc_list.head == NULL) {

		npc_list.head = (NPC*)malloc(sizeof(NPC));
		npc_list.head->x = x;
		npc_list.head->y = y;
		npc_list.head->number = number;
		npc_list.head->remainMove = 0;
		npc_list.head->direction = 0;

		npc_list.head->rightLink = npc_list.head;
		npc_list.head->leftLink = npc_list.head;
		
	}
	else if (npc_list.tail == NULL) {

		npc_list.tail = (NPC*)malloc(sizeof(NPC));
		npc_list.tail->x = x;
		npc_list.tail->y = y;
		npc_list.tail->number = number;
		npc_list.tail->remainMove = 0;
		npc_list.tail->direction = 0;

		npc_list.tail->rightLink = npc_list.head;
		npc_list.tail->leftLink = npc_list.head;

		npc_list.head->leftLink = npc_list.tail;
		npc_list.head->rightLink = npc_list.tail;

	}
	else {

		NPC* n = (NPC*)malloc(sizeof(NPC));

		n->x = x;
		n->y = y;
		n->number = number;
		n->remainMove = 0;
		n->direction = 0;

		n->rightLink = npc_list.tail;
		n->leftLink = npc_list.tail->leftLink;
		npc_list.tail->leftLink->rightLink = n;
		npc_list.tail->leftLink = n;


	}
	
}

void removeAllNPCList() {

	if (NPC_LIST == NULL) {
		return;
	}

	NPC* temp = NPC_LIST->head;
	NPC* temp1;
	while (temp != NULL) {
		temp1 = temp->rightLink;
		free(temp);
		temp = temp1;
	}

	NPC_LIST = NULL;

}

void addScore(int number) {

	if (SCORE_LIST == NULL) {
		SCORE_LIST = (SCORE*)malloc(sizeof(SCORE));
		SCORE_LIST->number = number;
		SCORE_LIST->count = 1;
		SCORE_LIST->next = NULL;
		return;
	}

	SCORE* temp = SCORE_LIST;
	SCORE* temp1 = SCORE_LIST;

	while (temp != NULL) {

		if (temp->number == number) {
			temp->count += 1;
			return;
		}

		temp1 = temp;
		temp = temp->next;
	}

	temp = (SCORE*)malloc(sizeof(SCORE));
	temp->count = 1;
	temp->number = number;
	temp->next = NULL;

	temp1->next = temp;

}

void removeAllScore() {

	if (SCORE_LIST == NULL) {
		return;
	}

	SCORE* temp = SCORE_LIST;
	SCORE* temp1;

	while (temp != NULL) {

		temp1 = temp;
		temp = temp->next;
		free(temp1);

	}

	SCORE_LIST = NULL;
}

// 도움말 페이지를 표시
void helpPage() {

	system("cls");
	SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y);
	printf("당신은 ▲입니다. NPC를 피해 도착지점까지 무사히 도착하세요!\n\n\n");

	int y = GetCurrentCursorPos().Y;
	SetCurrentCursorPos(GBOARD_ORIGIN_X, GetCurrentCursorPos().Y);
	printf("● Key\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X+2, GetCurrentCursorPos().Y);
	printf("* ↑ : 위로 이동\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* ↓ : 아래로 이동\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* ← : 왼쪽으로 이동\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* → : 오른쪽으로 이동\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* Space bar : 미사일 발사\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* Q : 초기화면에서 도움말\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* ESC : 뒤로 가기\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X+30, y);
	printf("● 게임 요소\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* □ : 1단계 벽\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* ■ : 2단계 벽\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* ▣ : 3단계 벽\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* ♤ : 1단계 NPC\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* ♠ : 2단계 NPC\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* ▩ : 함정\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* ＝,∥ : 미사일\n\n");




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

// 게임 시작
void gameStart() {

	// 게임시작에 필요한 초기값
	MAP map = ALL_MAP[ROUND];
	removeAllMissile();
	removeAllScore();

	mainDiceNumber = 0;
	itemDiceNumber = 0;
	diceEnableNumber = map.count;
	myX = map.startX;
	myY = map.startY;
	currentTankDirection = DIRECTION_UP;

	enableDice = true;
	enableMoveNPC = false;
	isNeedPrintMap = false;
	isGameOver = false;
	isSuccess = false;

	// 아이템 관련 전역변수 초기화
	enableSight = true;
	isAlreadyNPCSpeed = false;
	isAlreadySight = false;
	isAlreadyNPCStop = false;
	isAlreadyTankUnbeatable = false;
	isNPCStop = false;
	isTankUnbeatable = false;
	NPC_SPEED = 40;
	restrictSightRange = 3;
	remainDiceNumberByNPCSpeed = 0;
	remainDiceNumberBySight = 0;
	remainDiceNumberByNPCStop = 0;
	remainDiceNumberByTankUnbeatable = 0;

	drawGamePage();

	/*=======================*/
	int counting = 0;

	while (diceEnableNumber >= 0) {

		if (isGameOver || isSuccess) {
			return;
		}

		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				if (!enableDice) {
					if (moveTank(DIRECTION_LEFT)) {
						mainDiceNumber -= 1;
						drawRemainCount();
					}

					if (mainDiceNumber <= 0) {
						if (diceEnableNumber == 0) {
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						drawDice();
					}
				}
				break;
			case RIGHT:
				if (!enableDice) {
					if (moveTank(DIRECTION_RIGHT)) {
						mainDiceNumber -= 1;
						drawRemainCount();
					}

					if (mainDiceNumber <= 0) {
						if (diceEnableNumber == 0) {
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						drawDice();
					}
				}
				break;
			case UP:
				if (!enableDice) {
					if (moveTank(DIRECTION_UP)) {
						mainDiceNumber -= 1;
						drawRemainCount();
					}

					if (mainDiceNumber <= 0) {
						if (diceEnableNumber == 0) {
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						drawDice();
					}
				}
				break;
			case DOWN:
				if (!enableDice) {
					if (moveTank(DIRECTION_DOWN)) {
						mainDiceNumber -= 1;
						drawRemainCount();
					}

					if (mainDiceNumber <= 0) {
						if (diceEnableNumber == 0) {
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						drawDice();
					}
				}
				break;
			case SPACE:
				if (enableDice) {
					enableDice = false;
					enableMoveNPC = true;

					diceEnableNumber -= 1;
					srand((unsigned int)time(NULL));
					mainDiceNumber = rand() % 6 + 1;
					itemDiceNumber = rand() % 7;
					drawDice();
					drawRemainCount();
				}
				else {
					addMissile(myX, myY, currentTankDirection); 

					mainDiceNumber -= 1;
					drawRemainCount();

					if (mainDiceNumber <= 0) {
						if (diceEnableNumber == 0) {
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						drawDice();
					}
				}
				break;
			}

		}

		if (counting % 10 == 0) {
			moveMissile();
			printMissile();
		}

		/*if (!enableDice) {
			if (counting % 40 == 0) {
				if (moveNPC()) {
					return;
				}
				printNPC();
			}
		}*/
		if(!isNPCStop){

			if (counting % NPC_SPEED == 0) {
				if (moveNPC()) {
					return;
				}
				printNPC();
			}

		}
		

		if (isNeedPrintMap) {
			isNeedPrintMap = false;
			printMap();
		}
		
		//printTank();
		counting += 1;
		Sleep(20);
	}

}


// 가장 초기 게임플레이 화면을 그림
void drawGamePage() {

	MAP map = ALL_MAP[ROUND];

	system("cls");
	CURRENT_CONSOLE_WIDTH = (map.x * 2) + (GBOARD_ORIGIN_X) * 4 + SUB_GBOARD_WIDTH*2;
	int height = INIT_PAGE_HEIGHT;
	setConsoleSize(CURRENT_CONSOLE_WIDTH, height);

	SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y/2);
	printf("Round #%d", ROUND+1);

	printMap();
	printNPC();
	printTank();

	// 중앙선
	for (int y = 0; y < height; y++) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - SUB_GBOARD_WIDTH*2 - (GBOARD_ORIGIN_X * 2), y);
		printf("│");
	}
	
	drawDice();

	drawRemainCount();
	

}

// 주사위 영역을 그림
void drawDice() {

	SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 18, GBOARD_ORIGIN_Y);
	printf("주사위를 굴릴 수 있는 남은 횟수 : %2d", diceEnableNumber);

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
			if (dice[mainDiceNumber][y][x] == 0)
				printf("　");
			else if (dice[mainDiceNumber][y][x] == 1)
				printf("┌");
			else if (dice[mainDiceNumber][y][x] == 2)
				printf("┐");
			else if (dice[mainDiceNumber][y][x] == 3)
				printf("└");
			else if (dice[mainDiceNumber][y][x] == 4)
				printf("┘");
			else if (dice[mainDiceNumber][y][x] == 5)
				printf("─");
			else if (dice[mainDiceNumber][y][x] == 6)
				printf("│");
			else if (dice[mainDiceNumber][y][x] == 7)
				printf("●");
		}

	}
	SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 10, GBOARD_ORIGIN_Y + 3 + DICE_HEIGHT);
	printf("조작 횟수");
	for (int y = 0; y < DICE_HEIGHT; y++) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH *2, GBOARD_ORIGIN_Y + 3 + y);

		for (int x = 0; x < DICE_WIDTH; x++) {
			if (dice[itemDiceNumber][y][x] == 0)
				printf("　");
			else if (dice[itemDiceNumber][y][x] == 1)
				printf("┌");
			else if (dice[itemDiceNumber][y][x] == 2)
				printf("┐");
			else if (dice[itemDiceNumber][y][x] == 3)
				printf("└");
			else if (dice[itemDiceNumber][y][x] == 4)
				printf("┘");
			else if (dice[itemDiceNumber][y][x] == 5)
				printf("─");
			else if (dice[itemDiceNumber][y][x] == 6)
				printf("│");
			else if (dice[itemDiceNumber][y][x] == 7)
				printf("●");
		}
	}
	SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 11 + DICE_WIDTH * 2, GBOARD_ORIGIN_Y + 3 + DICE_HEIGHT);
	printf("아이템 번호");


	if (mainDiceNumber == 0) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 18, GBOARD_ORIGIN_Y + 2 + DICE_HEIGHT / 2+1);
		printf("Spacebar를 눌러 주사위를 굴려주세요.");
	}

}

// 남은 조작횟수 영역을 그림
void drawRemainCount() {

	SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 10, GBOARD_ORIGIN_Y + 4 + DICE_HEIGHT+2);
	printf("남은 조작 횟수 %d번", mainDiceNumber);


	for (int j = 0; j < BIG_NUMBER_HEIGHT; j++) {
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - BIG_NUMBER_WIDHT, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + j);
		for (int i = 0; i < BIG_NUMBER_WIDHT; i++) {
			if (bigNumber[mainDiceNumber][j][i] == 0) {
				printf("　");
			}
			else {
				printf("■");
			}
		}
	}

}

// NPC를 움직임(탱크와 충돌할 경우 true 리턴)
boolean moveNPC() {

	NPC_HEAD npc_list = NPC_LIST[ROUND];

	if (npc_list.num == 0) {
		return false;
	}

	NPC* npc = npc_list.head;
	MAP map = ALL_MAP[ROUND];

	do {

		while (1) {

			int x = npc->x;
			int y = npc->y;
			int direction = npc->direction;
			int remainMove = npc->remainMove;
			int* check = npc->check;

			if (remainMove == 0) {

				if (check[1] == 1 && check[2] == 1 && check[3] == 1 && check[4] == 1) {
					npc->check[1] = 0;
					npc->check[2] = 0;
					npc->check[3] = 0;
					npc->check[4] = 0;
					break;
				}
				while (1) {
					direction = rand() % 4 + 1;
					if (check[direction] != 1) {
						break;
					}
				}
					
				npc->direction = direction;
				npc->check[direction] = 1;

				remainMove = rand() % 4 + 1;
				npc->remainMove = remainMove;
			}

			switch (direction) {
			case DIRECTION_LEFT:
				x -= 1;
				break;
			case DIRECTION_RIGHT:
				x += 1;
				break;
			case DIRECTION_UP:
				y -= 1;
				break;
			case DIRECTION_DOWN:
				y += 1;
				break;
			}

			if (detectConflictWithTank(x, y)) {
				if (isTankUnbeatable) {
					npc->remainMove = 0;
					continue;
				}
				else {
					return true;
				}
				
			}

			if (detectConflictWithWall(x, y) || detectConflictWithNPC(x,y)) {
				npc->remainMove = 0;
				continue;
			}
			SetCurrentCursorPos(GBOARD_ORIGIN_X + (npc->x * 2), GBOARD_ORIGIN_Y + npc->y);
			if (map.map[npc->y][npc->x] == 4) {
				printf("＊");
			}
			else {
				printf("　");
			}
			
			npc->x = x;
			npc->y = y;
			npc->check[1] = 0;
			npc->check[2] = 0;
			npc->check[3] = 0;
			npc->check[4] = 0;

			break;

		}




		npc = npc->rightLink;

	} while (npc != npc_list.head);

	return false;

}

// 탱크와 충돌을 체크(충돌일시 true반환)
boolean detectConflictWithTank(int checkX, int checkY) {

	if (checkX == myX && checkY == myY) {
		return true;
	}

	return false;
}

// NPC와 충돌을 체크(충돌일시 true반환)
boolean detectConflictWithNPC(int checkX, int checkY) {

	NPC_HEAD npc_list = NPC_LIST[ROUND];

	if (npc_list.num == 0) {
		return false;
	}

	NPC* npc = npc_list.head;

	do {

		int x = npc->x;
		int y = npc->y;

		if (checkX == x && checkY == y)
			return true;

		npc = npc->rightLink;
	} while (npc != npc_list.head);

	return false;
}

// 벽과의 충돌을 체크(충돌일시 true반환)
boolean detectConflictWithWall(int checkX, int checkY) {

	MAP map = ALL_MAP[ROUND];

	int number = map.map[checkY][checkX];

	if (number <= 3 && number > 0) {
		return true;
	}
	else if (number == 9 || number == 8) {
		return true;
	}

	return false;

}

// 아이템 블록과 충돌을 체크(충돌일시 true반환)
boolean detectConflictWithItem(int checkX, int checkY) {

	MAP map = ALL_MAP[ROUND];

	int number = map.map[checkY][checkX];

	if (number == 4) {
		return true;
	}

	return false;
}

void removeNPC(int x, int y, boolean isDelete) {
	NPC_HEAD npc_list = NPC_LIST[ROUND];

	NPC* temp = npc_list.head;

	if (npc_list.num == 0) {
		return;
	}

	do {

		if (temp->x == x && temp->y == y) {

			addScore(temp->number);

			
			temp->number -= 1;
			if (isDelete) {
				temp->number -= 1;
			}

			if (temp->number < 5) {

				NPC_LIST[ROUND].num -= 1;
				SetCurrentCursorPos(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y);
				printf("　");



				NPC* right = temp->rightLink;
				NPC* left = temp->leftLink;

				right->leftLink = left;
				left->rightLink = right;

				if (temp == npc_list.head) {
					NPC_LIST[ROUND].head = right;
					npc_list.head = right;
				}
				else if (temp == npc_list.tail) {
					NPC_LIST[ROUND].tail = left;
					npc_list.tail = left;
				}

				temp->rightLink = NULL;
				temp->leftLink = NULL;
				free(temp);

				temp = right;

				continue;

			}

		}

		temp = temp->rightLink;


	} while (temp != npc_list.head);

}

void removeWall(int x, int y) {

	int temp = ALL_MAP[ROUND].map[y][x];

	switch (temp) {
	case 1:
		ALL_MAP[ROUND].map[y][x] -= 1;
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y);
		printf("　");
		addScore(temp);
		break;
	case 2:
	case 3:
		ALL_MAP[ROUND].map[y][x] -= 1;
		addScore(temp);
		break;
	}

}

// 탱크를 이동시키는 함수(이동가능하면 ture, 불가능하면 false 반환)
boolean moveTank(int direction) {

	boolean check = false;

	if (direction == currentTankDirection) {
		int tempX = myX;
		int tempY = myY;

		MAP map = ALL_MAP[ROUND];

		switch (direction) {
		case DIRECTION_LEFT:
			tempX -= 1;
			break;
		case DIRECTION_RIGHT:
			tempX += 1;
			break;
		case DIRECTION_UP:
			tempY -= 1;
			break;
		case DIRECTION_DOWN:
			tempY += 1;
			break;
		}

		if (detectConflictWithNPC(tempX, tempY)) {
			if (isTankUnbeatable) {
				return false;
			}
			else {
				isGameOver = true;
				return check;
			}
		}

		switch (map.map[tempY][tempX]) {
		case 0:
		case 4:
			SetCurrentCursorPos(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY);
			printf("　");
			myX = tempX;
			myY = tempY;
			check = true;
			break;
		case 8:
			isSuccess = true;
			break;
		default:
			break;
		}


		if (detectConflictWithItem(myX, myY)) {
			ALL_MAP[ROUND].map[myY][myX] = 0;
			useItem();
		}

		printTank();
	}
	else {
		currentTankDirection = direction;

		SetCurrentCursorPos(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY);
		setFontColor(PLAYER_COLOR);
		switch (currentTankDirection) {
		case DIRECTION_LEFT:
			printf("◀");
			break;
		case DIRECTION_RIGHT:
			printf("▶");
			break;
		case DIRECTION_UP:
			printf("▲");
			break;
		case DIRECTION_DOWN:
			printf("▼");
			break;
		}
		setFontColor(FONT_DEFAULT_COLOR);
	}

	

	return check;

}

void useItem() {

	srand((unsigned int)time(NULL));
	int item = rand() % 10 + 1;

	switch (item) {
	case 1:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　NPC 스피드 업　　　");
		_beginthread(NPCSpeedUp, 0, NULL);
		break;
	case 2:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　조작횟수 반감　　　");
		mainDiceNumber = (mainDiceNumber+1)/2;
		break;
	case 3:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　　시야 가리기　　　");
		_beginthread(restrictSight, 0, NULL);
		Sleep(10);
		printMap();
		break;
	case 4:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　벽 단계 증가 　　　");
		mapDestroy(true);
		break;
	case 5:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　　NPC  추가　　　　");
		addNewNPC();
		break;
	case 6:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　조작횟수 증가　　　");
		mainDiceNumber += 2;
		break;
	case 7:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　벽 단계 감소 　　　");
		mapDestroy(false);
		break;
	case 8:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　　NPC 멈추기　　　　");
		_beginthread(NPCStop, 0, NULL);
		break;
	case 9:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　플레이어 무적　　　");
		_beginthread(tankUnbeatable, 0, NULL);
		break;
	case 10:
		SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		printf("　　　원하는 NPC 삭제　　");

		removeOneNPC();

		break;
	}

}

void removeOneNPC() {

	boolean temp = enableSight;
	enableSight = true;

	NPC_HEAD npc_list = NPC_LIST[ROUND];
	MAP map = ALL_MAP[ROUND];

	if (npc_list.num == 0) {
		isNPCStop = false;
		isTankUnbeatable = false;
		enableSight = temp;
		return;
	}

	printMap();
	printNPC();
	printTank();

	boolean isSelected = false;

	NPC* npc = npc_list.head;

	int x = npc->x;
	int y = npc->y;
	int number = npc->number;

	SetCurrentCursorPos(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y);

	setFontColor(FONT_LIGHT_RED_COLOR);
	if (number == 5) {
		printf("♤");
	}
	else if (number == 6) {
		printf("♠");
	}
	setFontColor(FONT_DEFAULT_COLOR);

	while (!isSelected) {

		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				printNPC();
				npc = npc->leftLink;
				x = npc->x;
				y = npc->y;
				number = npc->number;
				SetCurrentCursorPos(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y);

				setFontColor(FONT_LIGHT_RED_COLOR);
				if (number == 5) {
					printf("♤");
				}
				else if (number == 6) {
					printf("♠");
				}
				setFontColor(FONT_DEFAULT_COLOR);

				break;
			case RIGHT:
				printNPC();
				npc = npc->rightLink;
				x = npc->x;
				y = npc->y;
				number = npc->number;
				SetCurrentCursorPos(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y);

				setFontColor(FONT_LIGHT_RED_COLOR);
				if (number == 5) {
					printf("♤");
				}
				else if (number == 6) {
					printf("♠");
				}
				setFontColor(FONT_DEFAULT_COLOR);

				break;
			case SPACE:
				removeNPC(x, y, true);
				printNPC();
				isSelected = true;
				break;
			}

		}


		Sleep(20);
	}

	enableSight = temp;
}

void NPCSpeedUp(void * param) {

	if (!isAlreadyNPCSpeed) {

		isAlreadyNPCSpeed = true;
		remainDiceNumberByNPCSpeed = diceEnableNumber - 2;

		NPC_SPEED = NPC_SPEED/2;
		if (NPC_SPEED < 2) {
			NPC_SPEED = 2;
		}

		while (remainDiceNumberByNPCSpeed < diceEnableNumber) {

		}

		NPC_SPEED = 40;
		isAlreadyNPCSpeed = false;

	}
	else {
		remainDiceNumberByNPCSpeed = diceEnableNumber - 2;
		NPC_SPEED = NPC_SPEED / 2;
		if (NPC_SPEED < 2) {
			NPC_SPEED = 2;
		}
	}

}

void NPCStop(void * param) {

	if (!isAlreadyNPCStop) {

		isAlreadyNPCStop = true;
		remainDiceNumberByNPCStop = diceEnableNumber - 2;

		isNPCStop = true;

		while (remainDiceNumberByNPCSpeed < diceEnableNumber) {

		}

		isNPCStop = false;
		isAlreadyNPCStop = false;

	}
	else {
		remainDiceNumberByNPCStop = diceEnableNumber - 2;
	}

}

void tankUnbeatable(void * param) {

	if (!isAlreadyTankUnbeatable) {

		isAlreadyTankUnbeatable = true;
		remainDiceNumberByTankUnbeatable = diceEnableNumber - 2;

		isTankUnbeatable = true;

		while (remainDiceNumberByNPCSpeed < diceEnableNumber) {

		}

		isTankUnbeatable = false;
		isAlreadyTankUnbeatable = false;

	}
	else {
		remainDiceNumberByTankUnbeatable = diceEnableNumber - 2;
	}

}

void mapDestroy(boolean isUp) {

	for (int y = 0; y < ALL_MAP[ROUND].y; y++) {
		for (int x = 0; x < ALL_MAP[ROUND].x; x++) {

			int number = ALL_MAP[ROUND].map[y][x];
			if (isUp) {
				if (1 <= number && number <= 2) {
					ALL_MAP[ROUND].map[y][x] += 1;
				}
			}
			else {
				if (2 <= number && number <= 3) {
					ALL_MAP[ROUND].map[y][x] -= 1;
				}
			}
		}
	}

	printMap();

}

void restrictSight(void * param) {

	if (!isAlreadySight) {

		isAlreadySight = true;
		remainDiceNumberBySight = diceEnableNumber - 10;

		enableSight = false;

		while (remainDiceNumberBySight < diceEnableNumber) {

		}

		enableSight = true;
		isNeedPrintMap = true;
		isAlreadySight = false;

	}
	else {
		remainDiceNumberBySight = diceEnableNumber - 10;
	}

	

}

void addNewNPC() {

	int x = rand() % ALL_MAP[ROUND].x;
	x = x - (x % 2);
	int y = rand() % ALL_MAP[ROUND].y;
	y = y - (y % 2);
	int number = 5;

	while (1) {
		if ((detectConflictWithTank(x, y) == false) && (detectConflictWithNPC(x, y) == false) && (detectConflictWithItem(x, y) == false) && (detectConflictWithWall(x, y) == false)) {
			break;
		}
		else {
			x = rand() % ALL_MAP[ROUND].x;
			x = x - (x % 2);
			y = rand() % ALL_MAP[ROUND].y;
			y = y - (y % 2);
		}
	}

	addNPC(NPC_LIST[ROUND], x, y, number);

}

void moveMissile() {

	MISSILE* m = ALL_MISSILE;

	MAP map = ALL_MAP[ROUND];

	while (m != NULL) {

		if (m->isEnable) {

			int x = m->x;
			int y = m->y;
			int direction = m->direction;

			switch (direction) {
			case DIRECTION_LEFT:
				x -= 1;
				break;
			case DIRECTION_RIGHT:
				x += 1;
				break;
			case DIRECTION_UP:
				y -= 1;
				break;
			case DIRECTION_DOWN:
				y += 1;
				break;
			}

			if (m->x == myX && m->y == myY) {

			}
			else {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + (m->x * 2), GBOARD_ORIGIN_Y + m->y);
				printf("　");
			}

			if (detectConflictWithNPC(x, y)) {
				m->isEnable = false;
				removeNPC(x, y, false);
				printNPC();
			}
			else if (detectConflictWithWall(x, y)) {
				m->isEnable = false;
				removeWall(x, y);
				printMap();
			}
			else {
				if (map.map[y][x] == 0 || map.map[y][x] == 4) {
					m->x = x;
					m->y = y;
				}
				else {
					m->isEnable = false;
				}
			}

			

		}


		m = m->next;
	}

}

void printMissile() {

	MISSILE* m = ALL_MISSILE;
	while (m != NULL) {

		if (m->isEnable) {
			int x = m->x;
			int y = m->y;
			int direction = m->direction;

			if(!enableSight){
				if ((myX - restrictSightRange > x && x > myX + restrictSightRange) && (myY - restrictSightRange < y && y>myY+ restrictSightRange)) {
					m = m->next;
					continue;
				}
			}

			SetCurrentCursorPos(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y);
			switch (direction) {
			case DIRECTION_LEFT:
			case DIRECTION_RIGHT:
				printf("＝");
				break;
			case DIRECTION_UP:
			case DIRECTION_DOWN:
				printf("∥");
				break;
			}
		}

		m = m->next;
	}

	

}

// 탱크의 현재 위치를 출력
void printTank() {

	SetCurrentCursorPos(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY);
	setFontColor(PLAYER_COLOR);
	switch (currentTankDirection) {
	case DIRECTION_LEFT:
		printf("◀");
		break;
	case DIRECTION_RIGHT:
		printf("▶");
		break;
	case DIRECTION_UP:
		printf("▲");
		break;
	case DIRECTION_DOWN:
		printf("▼");
		break;
	}
	setFontColor(FONT_DEFAULT_COLOR);

	if(!enableSight){
		printMap();
	}
	
}

// NPC의 현재 위치를 출력
void printNPC() {
	
	NPC_HEAD npc_list = NPC_LIST[ROUND];
	MAP map = ALL_MAP[ROUND];

	if (npc_list.num == 0) {
		return;
	}

	NPC* npc = npc_list.head;

	do {

		int x = npc->x;
		int y = npc->y;
		int number = npc->number;

		SetCurrentCursorPos(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y);

		if (!enableSight) {
			if ((myX - restrictSightRange <= x && x <= myX + restrictSightRange) && (myY - restrictSightRange <= y && y <= myY + restrictSightRange)) {

			}
			else {
				printf("　");
				npc = npc->rightLink;
				continue;
			}
		}

		if (number == 5) {
			printf("♤");
		}
		else if (number == 6) {
			printf("♠");
		}

		npc = npc->rightLink;
	} while (npc != npc_list.head);
	

}

// 맵을 출력(NPC, TANK 모두 출력됨)
void printMap() {

	MAP map = ALL_MAP[ROUND];

	for (int y = 0; y < map.y; y++) {
		for (int x = 0; x < map.x; x++) {

			SetCurrentCursorPos(GBOARD_ORIGIN_X + (x*2), GBOARD_ORIGIN_Y + y);

			if (!enableSight) {
				if ((myX - restrictSightRange <= x && x <= myX + restrictSightRange) && (myY - restrictSightRange <= y && y <= myY + restrictSightRange)) {
					
				}
				else {
					printf("　");
					continue;
				}
			}

			switch (map.map[y][x]) {
			case 0:
				//printf("　");
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
				printf("＊");
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

	//printNPC();
	//printTank();

}

void reduceControlPoint() {
	mainDiceNumber -= 1;
	drawRemainCount();

	if (mainDiceNumber <= 0) {
		if (diceEnableNumber == 0) {
			return;
		}
		enableDice = true;
		enableMoveNPC = false;
		itemDiceNumber = 0;
		drawDice();
	}
}
void reduceControlPoint(int direction) {

	if (moveTank(direction)) {
		mainDiceNumber -= 1;
		drawRemainCount();
	}

	if (mainDiceNumber <= 0) {
		if (diceEnableNumber == 0) {
			return;
		}
		enableDice = true;
		enableMoveNPC = false;
		itemDiceNumber = 0;
		drawDice();
	}

}

// 현재 라운드에서 획득한 점수를 보여주는 페이지(성공했을 경우 점수, 실패할 경우 재시작 화면)
void showCurrentRoundScore(boolean success) {

	int key;

	system("cls");
	setConsoleSize(INIT_PAGE_WIDHT, INIT_PAGE_HEIGHT);

	if (success) {

		int delay = 800;
		int i = 0;
		int totalScore = 0;

		if (SCORE_LIST != NULL) {
			
			SCORE* temp = SCORE_LIST;

			while (temp != NULL) {

				SetCurrentCursorPos(INIT_PAGE_WIDHT/2 -9, GBOARD_ORIGIN_Y + (i * 5));

				switch (temp->number) {
				case 1:
					printf("□");
					break;
				case 2:
					printf("■");
					break;
				case 3:
					printf("▣");
					break;
				case 5:
					printf("♤");
					break;
				case 6:
					printf("♠");
					break;
				default:
					printf("%2d", temp->number);
					break;
				}

				printf(" : %d X 100 = %d", temp->count, temp->count * 100);
				totalScore += temp->count * 100;
				i += 1;
				temp = temp->next;

				for (int k = 0; k < delay / 25; k++) {
					if (_kbhit() != 0)
					{
						key = _getch();
						switch (key)
						{
						default:
							delay = 0;
							break;
						}

					}
					Sleep(20);
				}
			}

			
		}

		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 -15, GBOARD_ORIGIN_Y + (i * 5));
		printf("남은 조작 횟수 %d X 200 = %d", diceEnableNumber, diceEnableNumber * 200);
		totalScore += diceEnableNumber * 200;
		i += 1;
		for (int k = 0; k < delay / 25; k++) {
			if (_kbhit() != 0)
			{
				key = _getch();
				switch (key)
				{
				default:
					delay = 0;
					break;
				}

			}
			Sleep(20);
		}

		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 12, GBOARD_ORIGIN_Y + (i * 5));
		printf("%2d 라운드 총 점수 = %d", ROUND, totalScore);
		i += 1;
		for (int k = 0; k < delay / 25; k++) {
			if (_kbhit() != 0)
			{
				key = _getch();
				switch (key)
				{
				default:
					delay = 0;
					break;
				}

			}
			Sleep(20);
		}

		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 14, INIT_PAGE_HEIGHT - 5);
		printf("Press any key to continue");
	}
	else { // 해당 라운드 실패
		ROUND -= 1;
		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 14, INIT_PAGE_HEIGHT / 2);
		loadMap();
		printf("Press any key to restart");
	}

	
	// 키 입력을 받음
	while (1) {
		
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

// 게임 클리어 페이지
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
			default:
				return true;
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
void setFontColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
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