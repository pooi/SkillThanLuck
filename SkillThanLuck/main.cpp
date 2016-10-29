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
int currentTankDirection = DIRECTION_UP;
boolean enableDice = false;
boolean enableMoveNPC = false;
boolean isGameOver = false;
boolean isSuccess = false;
MAP* ALL_MAP = NULL;
NPC_HEAD* NPC_LIST = NULL;
MISSILE* ALL_MISSILE = NULL;
SCORE* SCORE_LIST = NULL;

// 함수 선언부
void SetCurrentCursorPos(int x, int y);
void SetCurrentCursorPos(COORD current);
COORD GetCurrentCursorPos(void);
void GetCurrentCursorPos(int& x, int & y);
void RemoveCursor(void);

void setConsoleSize(int cols, int lines);
void initPage();
void drawInitDefault();
void removeAllNPCList();
void loadMap();
void addNPC(NPC_HEAD& npc_list, int x, int y, int number);
void removeNPC(int x, int y);
void helpPage();
void gameStart();
void drawGamePage();
void removeWall(int x, int y);
boolean moveTank(int direction);
void moveNPCWithThread(void *param);
void removeAllMissile();
void addMissile(int x, int y, int direction);
void addScore(int number);
void removeAllScore();
void printMissile();
void moveMissile();
boolean detectConflictWithNPC(int checkX, int checkY);
boolean detectConflictWithWall(int checkX, int checkY);
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
		MISSILE* temp1;

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

	int i;

	while (temp != NULL) {

		temp1 = temp->next;
		free(temp);
		temp = temp1;

	}

	ALL_MISSILE = NULL;
}

// NPC를 원형 큐에 추가
void addNPC(NPC_HEAD& npc_list, int x, int y, int number) {

	npc_list.num += 1;

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
	isGameOver = false;
	isSuccess = false;

	drawGamePage();

	_beginthread(moveNPCWithThread, 1, NULL);

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

			if (counting % 25 == 0) {
				printNPC();
			}
		
		
		printTank();
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

void moveNPCWithThread(void *param) {

	//while (1) {

	//	NPC_HEAD npc_list = NPC_LIST[ROUND];

	//	if (npc_list.num == 0) {
	//		return;
	//	}

	//	NPC* npc = npc_list.head;
	//	MAP map = ALL_MAP[ROUND];


	//	while (!enableMoveNPC) {

	//	}

	//	do {

	//		int x = npc->x;
	//		int y = npc->y;
	//		int direction = npc->direction;
	//		int remainMove = npc->remainMove;

	//		if (remainMove == 0) {
	//			direction = rand() % 4 + 1;
	//			npc->direction = direction;
	//			remainMove = rand() % 10 + 1;
	//			npc->remainMove = remainMove;

	//		}

	//		// 예상 이동방향 설정
	//		switch (direction) {
	//		case DIRECTION_LEFT:
	//			x -= 1;
	//			break;
	//		case DIRECTION_RIGHT:
	//			x += 1;
	//			break;
	//		case DIRECTION_UP:
	//			y -= 1;
	//			break;
	//		case DIRECTION_DOWN:
	//			y += 1;
	//			break;
	//		}

	//		if (detectConflictWithNPC(x, y)) {
	//			npc->remainMove = 0;
	//			continue;
	//		}

	//		switch (map.map[y][x]) {
	//		case 0:
	//			if (myX == x && myY == y) {
	//				isGameOver = false;
	//				//return;
	//			}
	//			npc->x = x;
	//			npc->y = y;
	//			npc->remainMove -= 1;
	//			break;
	//		default:
	//			npc->remainMove = 0;
	//			break;
	//		}

	//		npc = npc->rightLink;
	//	} while (npc != npc_list.head);

	//	//printMap();
	//	Sleep(500);
	//}

}

// NPC와 탱크의 충돌을 체크(충돌일시 true반환)
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

	if (map.map[checkY][checkX] <= 3 && map.map[checkY][checkX] > 0) {

		return true;
	}

	return false;

}

void removeNPC(int x, int y) {
	NPC_HEAD npc_list = NPC_LIST[ROUND];

	NPC* temp = npc_list.head;

	if (npc_list.num == 0) {
		return;
	}

	do {

		if (temp->x == x && temp->y == y) {

			addScore(temp->number);

			temp->number -= 1;
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
			isGameOver = true;
			return check;
		}

		switch (map.map[tempY][tempX]) {
		case 0:
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

		printTank();
	}
	else {
		currentTankDirection = direction;

		SetCurrentCursorPos(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY);
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
	}

	

	return check;

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
				removeNPC(x, y);
				printNPC();
			}
			else if (detectConflictWithWall(x, y)) {
				m->isEnable = false;
				removeWall(x, y);
				printMap();
			}
			else {
				if (map.map[y][x] == 0) {
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