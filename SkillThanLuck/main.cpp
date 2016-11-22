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
#define KEY_C 99

#define FONT_DEFAULT_COLOR 7
#define FONT_BLUE_COLOR 1
#define FONT_RED_COLOR 4
#define FONT_WHITE_COLOR 15
#define FONT_GREED_COLOR 2
#define FONT_JADE_COLOR 3
#define FONT_LIGHT_RED_COLOR 12
#define FONT_LIGHT_GREEN_COLOR 10
#define FONT_LIGHT_YELLOW_COLOR 14
#define FONT_LIGHT_BLUE_COLOR 9
#define FONT_LIGHT_JADE_COLOR 11
#define PLAYER_COLOR 9

#define DEFAULT_NPC_SPEED 800


typedef struct MAP {
	int x, y;
	int startX, startY;
	int count;
	int hook;
	int** map;
}MAP;

typedef struct NPC {
	int x, y;
	int direction;
	int remainMove;
	int number;
	int check[5];
	int newNPC;
	int color;
	NPC * leftLink;
	NPC * rightLink;
}NPC;

typedef struct NPC_HEAD {
	int num;
	NPC * head;
	NPC * tail;
}NPC_HEAD;

typedef struct MISSILE {
	int x, y;
	int direction;
	boolean isEnable;
	MISSILE * next;
}MISSILE;

typedef struct HOOK {

	int x, y;
	int direction;
	HOOK * next;

}HOOK;

typedef struct SCORE {
	int number;
	int count;
	SCORE* next;
}SCORE;


/*==============================*/
/*====== 전역 변수 선언부 ======*/

// 화면 요소 관련
const int GBOARD_ORIGIN_X = 8;
const int GBOARD_ORIGIN_Y = 4;
const int INIT_PAGE_WIDHT = 150;
const int INIT_PAGE_HEIGHT = 50;
const int SUB_GBOARD_WIDTH = 25;
const int SUB_GBOARD_HEIGHT = 20;
int CURRENT_CONSOLE_WIDTH = 0;
int CURRENT_CONSOLE_HEIGHT = 0;
int CURRENT_MAP_WIDTH = 0;
int CURRENT_MAP_HEIGHT = 0;

// 게임 진행에 필요한 변수
int ROUND = 0;
int TOTAL_SCORE = 0;
int MAXIMUM_ROUND = 20;
int LIFE = 0;
int mainDiceNumber = 0;
int controlPoint = 0;
int itemDiceNumber = 0;
int diceEnableNumber = 0;
int myX = 0;
int myY = 0;
int hookCount = 0;
boolean enableDice = false;
boolean enableMoveNPC = false;
boolean enableDirectionKey = true;
boolean isGameOver = false;
boolean isSuccess = false;
boolean isHook = false;
MAP* ALL_MAP = NULL;
NPC_HEAD* NPC_LIST = NULL;
MISSILE* ALL_MISSILE = NULL;
HOOK * HOOK_LIST = NULL;
SCORE* SCORE_LIST = NULL;

// 아이템 관련 전역 변수
int NPC_SPEED = DEFAULT_NPC_SPEED;
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

// 스크린 버퍼 관련 변수
int nBufferIndex = 0;
HANDLE hBuffer[2];

/*==============================*/
/*==============================*/


/*=========================*/
/*====== 함수 선언부 ======*/

// 초기 페이지
void initPage();
void drawInitDefault();
void loadMap();

// 연결 리스트와 관련된 함수
void addMissile(int x, int y, int direction);
void removeAllMissile();
void addHook(int x, int y, int direction, boolean isHead);
void removeAllHook();
void addNPC(NPC_HEAD& npc_list, int x, int y, int number, int newNPC);
void removeAllNPCList();
void addScore(int number);
void removeAllScore();

// 도움말 페이지
void helpPage();

// 게임 관련
void gameStart();
void gamePageInit();
void controlDirectionKey(int direction);
void drawGamePage();

// 맵 주변 안내 문구 출력부
void printLife();
void printRemainHook();
void printNotiMessage();
void printItemNotiMessage();

// 조작 관련된 UI 출력부
void drawDice();
void drawRemainCount();

// 맵과 연관된 객체 출력부
void printHook();
void printTank();
void printNPC();
void printMap();
void printMissile();

// 충돌 체크
boolean detectConflictWithTank(int checkX, int checkY);
boolean detectConflictWithNPC(int checkX, int checkY);
boolean detectConflictWithWall(int checkX, int checkY);
boolean detectConflictWithItem(int checkX, int checkY);

// 미사일과 관련된 액션
void removeNPC(int x, int y, boolean isDelete);
void removeWall(int x, int y);

// 객체의 이동과 관련된 함수
boolean moveTank(int direction);
void moveNPC(void * param);
void moveMissile(void * param);
void useHook(void * param);

// 아이템 사용과 관련된 함수
void useItem();
void removeOneNPC();
void mapDestroy(boolean isUp);
void addNewNPC();
void NPCSpeedUp(void * param);
void restrictSight(void * param);
void NPCStop(void * param);
void tankUnbeatable(void * param);

// 게임 종료와 관련된 함수
void showCurrentRoundScore(boolean success);
boolean showFinishPage();

// 부가 함수
void CreateBuffer();
void BufferWrite(int x, int y, char *string);
void Flipping();
void BufferClear();
void Release();
void setBufferFontColor(int color);
void setFontColor(int color);
void setConsoleSize(int cols, int lines);

void SetCurrentCursorPos(int x, int y);
void SetCurrentCursorPos(COORD current);
COORD GetCurrentCursorPos(void);
void GetCurrentCursorPos(int& x, int & y);
void RemoveCursor(void);

/*=========================*/
/*=========================*/



/*================================================*/
/*=====================메인=======================*/
/*================================================*/

int main() {

	RemoveCursor();
	
	boolean check = true;


	while (check) {

		ROUND = 0; // 라운드 초기화

		initPage(); // 인트로 페이지

		LIFE = 5; // 라이프 초기화

		while (ROUND<MAXIMUM_ROUND) { // 게임을 모두 클리어 하거나 라이프가 0일때까지 반복

			gameStart(); // 게임 시작
			Release(); // 버퍼 클리어

			showCurrentRoundScore(isSuccess); // 현재 라운드의 점수를 보여줌

			Sleep(DEFAULT_NPC_SPEED); // 잔여 스레드가 없도록 약간에 텀을 줌
			int key = _getch();
			while (_kbhit() != 0) { key = _getch(); } // 불필요한 입력 흡수

			ROUND += 1; // 라운드를 증가시키고 새로 게임 시작

			if (LIFE <= 0) { // 라이프가 모두 소진되면 게임 종료
				break;
			}

		}

		check = showFinishPage(); // 사용자가 종료를 원하면 FALSE, 계속을 원하면 TRUE

	}

	exit(1);

	return 0;
}

/*================================================*/
/*================================================*/
/*================================================*/


// 초기 페이지
void initPage() {

	loadMap();

	drawInitDefault();

	TOTAL_SCORE = 0;
	int tempRound = -1;

	while (1) {

		if (ROUND != tempRound) { // 라운드가 바뀌면 화면 새로고침
			tempRound = ROUND;
			SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 4, INIT_PAGE_HEIGHT / 2 + 2);
			printf("◀  %d  ▶", ROUND + 1);
		}


		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT: // 시작 라운드 감소
				ROUND = (ROUND - 1);
				if (ROUND < 0)
					ROUND = MAXIMUM_ROUND - 1;
				break;
			case RIGHT: // 시작 라운드 증가
				ROUND = (ROUND + 1) % MAXIMUM_ROUND;
				break;
			case KEY_I: // 안내페이지로 이동
				helpPage();
				drawInitDefault();
				break;
			case KEY_Q: // 게임 종료
				system("cls");
				SetCurrentCursorPos(INIT_PAGE_WIDHT/2-10, INIT_PAGE_HEIGHT/2);
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

		Sleep(20);
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
		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - x, INIT_PAGE_HEIGHT / 2 - (y - j) - 4);
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


	// 안내 문구 출력
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
		ALL_MAP = NULL;
	}
	ALL_MAP = (MAP*)malloc(sizeof(MAP)*MAXIMUM_ROUND);
	removeAllNPCList();
	NPC_LIST = (NPC_HEAD*)malloc(sizeof(NPC_HEAD)*MAXIMUM_ROUND);

	for (int i = 0; i < MAXIMUM_ROUND; i++) {
		fscanf(f, "%d", &ALL_MAP[i].x);
		fscanf(f, "%d", &ALL_MAP[i].y);
		fscanf(f, "%d", &ALL_MAP[i].count);
		fscanf(f, "%d", &ALL_MAP[i].hook);

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
					addNPC(NPC_LIST[i], x, y, number, 0);
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


/*=======================================*/
/*====== 연결 리스트와 관련된 함수 ======*/

// x, y좌표를 시작점으로 가지고 방향이 direction인 미사일 추가
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

// 미사일 리스트 초기화
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

// x, y좌표에 방향이 direction인 갈고리 추가
void addHook(int x, int y, int direction) {

	if (HOOK_LIST == NULL) {
		HOOK_LIST = (HOOK*)malloc(sizeof(HOOK));
		HOOK_LIST->x = x;
		HOOK_LIST->y = y;
		HOOK_LIST->direction = direction;
		HOOK_LIST->next = NULL;
	}
	else {

		HOOK* temp = HOOK_LIST;

		while (temp->next != NULL) {
			temp = temp->next;
		}

		HOOK* hook = (HOOK*)malloc(sizeof(HOOK));
		hook->x = x;
		hook->y = y;
		hook->direction = direction;
		hook->next = NULL;

		temp->next = hook;

	}

}

// 갈고리 초기화
void removeAllHook() {

	HOOK * temp = HOOK_LIST;
	HOOK * temp1;

	if (temp == NULL) {
		return;
	}

	while (temp != NULL) {
		temp1 = temp->next;
		free(temp);
		temp = temp1;
	}

	HOOK_LIST = NULL;

}

// NPC를 원형 연결리스트에 추가
void addNPC(NPC_HEAD& npc_list, int x, int y, int number, int newNPC) {

	npc_list.num += 1;
	int check[5] = { 0 };

	if (npc_list.head == NULL) {

		npc_list.head = (NPC*)malloc(sizeof(NPC));
		npc_list.head->x = x;
		npc_list.head->y = y;
		npc_list.head->number = number;
		npc_list.head->remainMove = 0;
		npc_list.head->direction = 0;
		npc_list.head->newNPC = newNPC;
		npc_list.head->color = FONT_DEFAULT_COLOR;

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
		npc_list.tail->newNPC = newNPC;
		npc_list.tail->color = FONT_DEFAULT_COLOR;

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
		n->newNPC = newNPC;
		n->color = FONT_DEFAULT_COLOR;

		n->rightLink = npc_list.tail;
		n->leftLink = npc_list.tail->leftLink;
		npc_list.tail->leftLink->rightLink = n;
		npc_list.tail->leftLink = n;


	}

}

// NPC 리스트 초기화
void removeAllNPCList() {

	if (NPC_LIST == NULL) {
		return;
	}

	for (int i = 0; i < ROUND; i++) {
		NPC_HEAD npc_list = NPC_LIST[i];

		NPC* temp = npc_list.head->rightLink;
		NPC* temp1;
		do {
			temp1 = temp->rightLink;
			free(temp);
			temp = temp1;
		} while (temp != npc_list.head);

		free(npc_list.head);
		npc_list.head = NULL;
		npc_list.tail = NULL;
	}



	NPC_LIST = NULL;

}

// 점수 추가
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

		if (temp->number == number) { // 이미 저장된적이 있는 객체일 경우 카운트만 증가 시키고 함수 종료
			temp->count += 1;
			return;
		}

		temp1 = temp;
		temp = temp->next;
	}

	// 저장되어있지 않은 객체일 경우 새롭게 추가
	temp = (SCORE*)malloc(sizeof(SCORE));
	temp->count = 1;
	temp->number = number;
	temp->next = NULL;

	temp1->next = temp;

}

// 점수 초기화
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

/*=======================================*/
/*=======================================*/


// 도움말 페이지를 표시
void helpPage() {

	system("cls");
	SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y);
	printf("당신은 ▲입니다. NPC를 피해 도착지점까지 무사히 도착하세요!\n\n\n");

	int y = GetCurrentCursorPos().Y;
	SetCurrentCursorPos(GBOARD_ORIGIN_X, GetCurrentCursorPos().Y);
	printf("● Key\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* ↑ : 위로 이동\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* ↓ : 아래로 이동\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* ← : 왼쪽으로 이동\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* → : 오른쪽으로 이동\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* c : 갈고리 발사\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* Space bar : 미사일 발사\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* Q : 초기화면에서 도움말\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* ESC : 뒤로 가기\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 30, y);
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
	printf("* ＝,∥ : 미사일\n\n\n\n\n");


	SetCurrentCursorPos(GBOARD_ORIGIN_X, GetCurrentCursorPos().Y);
	printf("● 아이템\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X+2, GetCurrentCursorPos().Y);
	printf("아이템 주사위의 눈금이 짝수면 유리한 아이템, 홀수면 불리한 아이템 중 랜덤으로 실행됩니다.\n\n\n");

	y = GetCurrentCursorPos().Y;

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("● 유리한 아이템\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* NPC 일시 정지\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* 조작 횟수 +2\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* 모든 부실 수 있는 벽 단계 감소\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* 탱크 일시 무적\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* 원하는 NPC 하나 삭제\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 40, y);
	printf("● 불리한 아이템\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* NPC 속도 증가\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* 조작 횟수 반감\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* 플레이어 시야 가리기\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* 모든 부실 수 있는 벽 단계 증가\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* NPC 추가\n\n");
	

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


// 게임에 관련된 변수 초기화
void gamePageInit() {

	MAP map = ALL_MAP[ROUND];

	// 각종 리스트 초기화
	removeAllMissile();
	removeAllScore();
	removeAllHook();

	// 화면 크기 조절
	CURRENT_CONSOLE_WIDTH = (map.x * 2) + (GBOARD_ORIGIN_X) * 4 + SUB_GBOARD_WIDTH * 2;
	CURRENT_CONSOLE_HEIGHT = INIT_PAGE_HEIGHT;
	setConsoleSize(CURRENT_CONSOLE_WIDTH, CURRENT_CONSOLE_HEIGHT);

	// 이중 버퍼 사용을 위한 버퍼 생성
	CreateBuffer();

	// 현재 맵 크기 저장
	CURRENT_MAP_WIDTH = map.x * 2;
	CURRENT_MAP_HEIGHT = map.y;

	// 조작에 필요한 변수 초기화
	mainDiceNumber = 0;
	controlPoint = 0;
	itemDiceNumber = 0;
	diceEnableNumber = map.count;
	myX = map.startX;
	myY = map.startY;
	hookCount = map.hook;
	currentTankDirection = DIRECTION_UP;

	enableDice = true;
	enableMoveNPC = false;
	enableDirectionKey = true;
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
	NPC_SPEED = DEFAULT_NPC_SPEED;
	restrictSightRange = 3;
	remainDiceNumberByNPCSpeed = 0;
	remainDiceNumberBySight = 0;
	remainDiceNumberByNPCStop = 0;
	remainDiceNumberByTankUnbeatable = 0;

	drawGamePage(); // 화면 그림

}

// 게임 시작
void gameStart() {

	// 게임시작에 필요한 초기값
	gamePageInit();

	_beginthread(moveNPC, 1, NULL); // NPC 움직임 시작
	_beginthread(moveMissile, 2, NULL); // 미사일 움직임 시작

	/*=======================*/

	while (diceEnableNumber >= 0) {

		if (isGameOver || isSuccess) { // 게임 종료 조건이 충족되면 함수 종료
			isGameOver = true;
			return;
		}

		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				controlDirectionKey(DIRECTION_LEFT);
				break;
			case RIGHT:
				controlDirectionKey(DIRECTION_RIGHT);
				break;
			case UP:
				controlDirectionKey(DIRECTION_UP);
				break;
			case DOWN:
				controlDirectionKey(DIRECTION_DOWN);
				break;
			case KEY_C:
				if (!enableDice) {
					if (hookCount > 0) {
						enableDirectionKey = false;
						_beginthread(useHook, 5, NULL);
					}
				}
				break;
			case SPACE:
				if (enableDice) { // 다이스를 굴릴 필요가 있다면 다이스를 굴림
					enableDice = false;
					enableMoveNPC = true;

					diceEnableNumber -= 1;
					srand((unsigned int)time(NULL));
					mainDiceNumber = rand() % 6 + 1;
					controlPoint = mainDiceNumber;
					itemDiceNumber = rand() % 6 + 1;

					if (mainDiceNumber == itemDiceNumber) {
						hookCount += 1;
					}
					//drawDice();
					//drawRemainCount();
				}else { // 아닐경우 미사일 발사
					controlDirectionKey(10);
				}
				break;
			}

		}

		// 20ms 단위로 화면 갱신 및 키 입력받기
		drawGamePage();
		Sleep(20);
	}

}

// 게임 중 발생하는 키 입력에 대응
void controlDirectionKey(int direction) {

	if (direction == 10) { // 스페이스바가 눌린 경우

		if (enableDirectionKey) {

			addMissile(myX, myY, currentTankDirection); // 미사일 발사

			controlPoint -= 1;


		}

	}
	else { // 방향키가 눌린 경우

		if ((!enableDice) && enableDirectionKey) {
			if (moveTank(direction)) { // 탱크가 움직일 조건을 충족하고 움직였다면
				controlPoint -= 1;
			}
		}

	}


	if (controlPoint <= 0) {
		if (diceEnableNumber == 0) { // 더이상 게임을 진행하지 못할 경우
			isGameOver = true;
			return;
		}

		enableDice = true;
		enableMoveNPC = false;
		itemDiceNumber = 0;
		mainDiceNumber = 0;
	}

	

}

// 가장 초기 게임플레이 화면을 그림
void drawGamePage() {

	BufferClear(); // 화면 버퍼 초기화

	MAP map = ALL_MAP[ROUND];

	// 현재 라운드 정보 출력
	char string[12];
	sprintf(string, "Round #%d", ROUND + 1);
	BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y - 1, string);


	printLife();
	setBufferFontColor(FONT_DEFAULT_COLOR);
	printMap();
	printNPC();
	printMissile();
	printHook();

	// 중앙선
	for (int y = 0; y < CURRENT_CONSOLE_HEIGHT; y++) {
		BufferWrite(CURRENT_CONSOLE_WIDTH - SUB_GBOARD_WIDTH * 2 - (GBOARD_ORIGIN_X * 2), y, "│");
	}

	drawDice();
	drawRemainCount();

	printNotiMessage();
	printItemNotiMessage();
	printRemainHook();


	printTank();

	Flipping(); // 그려진 화면으로 전환

}


/*=======================================*/
/*======= 맵 주변 안내 문구 출력부 ======*/

// 라이프를 출력함
void printLife() {

	// 맵 상단 우측 모서리에 라이프 출력
	for (int x = 1; x <= 5; x++) {
		setBufferFontColor(FONT_LIGHT_RED_COLOR);
		if (x <= LIFE) {
			BufferWrite(GBOARD_ORIGIN_X + CURRENT_MAP_WIDTH - (x * 2), GBOARD_ORIGIN_Y - 1, "♥");
		}
		else {
			break;
		}
		setBufferFontColor(FONT_DEFAULT_COLOR);
	}

}

// 남은 갈고리 횟수를 출력함
void printRemainHook() {

	// 맵 우측 하단에 남은 갈고리 개수 출력
	setBufferFontColor(FONT_GREED_COLOR);
	for (int i = 1; i <= hookCount; i++) {
		BufferWrite(GBOARD_ORIGIN_X + CURRENT_MAP_WIDTH - (i * 2), GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT, "§");
	}
	setBufferFontColor(FONT_DEFAULT_COLOR);

}

// 안내 메시지를 출력함
void printNotiMessage() {

	/*if (diceEnableNumber <= 5) {

		char string[30];

		sprintf(string, "경고 : 남은 조작 횟수 %d번", diceEnableNumber);

		setBufferFontColor(FONT_LIGHT_RED_COLOR);
		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 1, string);
		setBufferFontColor(FONT_DEFAULT_COLOR);

	}*/


}

// 아이템관련 안내 메시지를 출력함
void printItemNotiMessage() {

	int count = 0;

	if (isAlreadyNPCSpeed) {
		char string[30];

		sprintf(string, "NPC 속도 증가 종료까지 : %d번", diceEnableNumber - remainDiceNumberByNPCSpeed);

		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 3 + count, string);

		count += 1;
	}

	if (isAlreadyNPCStop) {
		char string[26];

		sprintf(string, "NPC 멈춤 종료까지 : %d번", diceEnableNumber - remainDiceNumberByNPCStop);

		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 3 + count, string);

		count += 1;
	}

	if (isAlreadyTankUnbeatable) {
		char string[30];

		sprintf(string, "플레이어 무적 종료까지 : %d번", diceEnableNumber - remainDiceNumberByTankUnbeatable);

		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 3 + count, string);

		count += 1;
	}

	if (isAlreadySight) {
		char string[26];

		sprintf(string, "시야가리기 종료까지 : %d번", diceEnableNumber - remainDiceNumberBySight);

		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 3 + count, string);

		count += 1;
	}

}

/*=======================================*/
/*=======================================*/


/*=====================================*/
/*====== 조작과 관련된 UI 출력부 ======*/

// 주사위 영역을 그림
void drawDice() {

	// 남은 주사위 횟수 출력
	char string[40];
	sprintf(string, "주사위를 굴릴 수 있는 남은 횟수 : %2d", diceEnableNumber);
	if (diceEnableNumber <= 5) {
		setBufferFontColor(FONT_LIGHT_RED_COLOR);
	}
	BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 18, GBOARD_ORIGIN_Y, string);
	setBufferFontColor(FONT_DEFAULT_COLOR);

	// 다이스 틀 출력
	for (int y = 0; y < DICE_HEIGHT + 3; y++) {
		if (y == 0) {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "┌");
				}
				else if (x == SUB_GBOARD_WIDTH - 1) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "┐");
				}
				else {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "─");
				}
			}
		}
		else if (y == DICE_HEIGHT + 3 - 1) {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "└");
				}
				else if (x == SUB_GBOARD_WIDTH - 1) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "┘");
				}
				else {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "─");
				}
			}
		}
		else {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0 || x == SUB_GBOARD_WIDTH - 1) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "│");
				}
				else {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "　");
				}
			}
		}

	}

	// 조작 횟수와 관련된 주사위 출력
	for (int y = 0; y < DICE_HEIGHT; y++) {

		for (int x = 0; x < DICE_WIDTH; x++) {
			if (dice[mainDiceNumber][y][x] == 0) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "　");
			}
			else if (dice[mainDiceNumber][y][x] == 1) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "┌");
			}
			else if (dice[mainDiceNumber][y][x] == 2) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "┐");
			}
			else if (dice[mainDiceNumber][y][x] == 3) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "└");
			}
			else if (dice[mainDiceNumber][y][x] == 4) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "┘");
			}
			else if (dice[mainDiceNumber][y][x] == 5) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "─");
			}
			else if (dice[mainDiceNumber][y][x] == 6) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "│");
			}
			else if (dice[mainDiceNumber][y][x] == 7) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "●");
			}
		}

	}
	BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 10, GBOARD_ORIGIN_Y + 3 + DICE_HEIGHT, "조작 횟수");


	// 아이템과 관련된 주사위 출력
	for (int y = 0; y < DICE_HEIGHT; y++) {

		for (int x = 0; x < DICE_WIDTH; x++) {
			if (dice[itemDiceNumber][y][x] == 0) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "　");
			}
			else if (dice[itemDiceNumber][y][x] == 1) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "┌");
			}
			else if (dice[itemDiceNumber][y][x] == 2) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "┐");
			}
			else if (dice[itemDiceNumber][y][x] == 3) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "└");
			}
			else if (dice[itemDiceNumber][y][x] == 4) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "┘");
			}
			else if (dice[itemDiceNumber][y][x] == 5) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "─");
			}
			else if (dice[itemDiceNumber][y][x] == 6) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "│");
			}
			else if (dice[itemDiceNumber][y][x] == 7) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "●");
			}
		}
	}
	BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 11 + DICE_WIDTH * 2, GBOARD_ORIGIN_Y + 3 + DICE_HEIGHT, "아이템 번호");


	// 주사위를 굴리지 못한다면 안내문구 출력
	if (controlPoint == 0) {
		setBufferFontColor(FONT_LIGHT_GREEN_COLOR);
		BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 18, GBOARD_ORIGIN_Y + 2 + DICE_HEIGHT / 2 + 1, "Spacebar를 눌러 주사위를 굴려주세요.");
		setBufferFontColor(FONT_DEFAULT_COLOR);
	}

}

// 남은 조작횟수 영역을 그림
void drawRemainCount() {


	char string[20];
	sprintf(string, "남은 조작 횟수 %d번", controlPoint);
	BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 10, GBOARD_ORIGIN_Y + 4 + DICE_HEIGHT + 2, string);

	// 7세그먼트로 출력
	for (int j = 0; j < BIG_NUMBER_HEIGHT; j++) {
		for (int i = 0; i < BIG_NUMBER_WIDHT; i++) {
			if (bigNumber[controlPoint][j][i] == 0) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - BIG_NUMBER_WIDHT + i * 2, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + j, "　");
			}
			else {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - BIG_NUMBER_WIDHT + i * 2, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + j, "■");
			}
		}
	}

}

/*=====================================*/
/*=====================================*/


/*=====================================*/
/*====== 맵과 연관된 객체 출력부 ======*/

// 갈고리 출력
void printHook() {

	if (isHook) {

		HOOK* temp = HOOK_LIST;

		while (temp != NULL) {

			int x = temp->x;
			int y = temp->y;
			int direction = temp->direction;

			switch (direction) {
			case DIRECTION_LEFT:
			case DIRECTION_RIGHT:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "─");
				break;
			case DIRECTION_DOWN:
			case DIRECTION_UP:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "│");
				break;
			}

			temp = temp->next;


		}

	}

}

// 미사일 출력
void printMissile() {

	MISSILE* m = ALL_MISSILE;
	while (m != NULL) {

		if (m->isEnable) {
			int x = m->x;
			int y = m->y;
			int direction = m->direction;

			if (!enableSight) { // 시야를 가릴 필요가 있을 경우
				if ((myX - restrictSightRange > x && x > myX + restrictSightRange) && (myY - restrictSightRange < y && y>myY + restrictSightRange)) {
					m = m->next;
					continue;
				}
			}

			switch (direction) {
			case DIRECTION_LEFT:
			case DIRECTION_RIGHT:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "＝");
				break;
			case DIRECTION_UP:
			case DIRECTION_DOWN:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "∥");
				break;
			}
		}

		m = m->next;
	}



}

// 탱크의 현재 위치를 출력
void printTank() {

	if (isTankUnbeatable) {
		setBufferFontColor(FONT_RED_COLOR);
	}
	else {
		setBufferFontColor(PLAYER_COLOR);
	}
	switch (currentTankDirection) {
	case DIRECTION_LEFT:
		BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "◀");
		break;
	case DIRECTION_RIGHT:
		BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "▶");
		break;
	case DIRECTION_UP:
		BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "▲");
		break;
	case DIRECTION_DOWN:
		BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "▼");
		break;
	}
	setBufferFontColor(FONT_DEFAULT_COLOR);

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
		int newNPC = npc->newNPC;
		int color = npc->color;

		if (!enableSight) { // 시야를 가릴 필요가 있을 경우
			if ((myX - restrictSightRange <= x && x <= myX + restrictSightRange) && (myY - restrictSightRange <= y && y <= myY + restrictSightRange)) {

			}
			else {
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "　");
				npc = npc->rightLink;
				continue;
			}
		}

		if (number == 5) {
			if (newNPC > 0) { // 새롭게 추가된 NPC일 경우
				npc->newNPC -= 1;
				setBufferFontColor(FONT_GREED_COLOR);
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♤");
				setBufferFontColor(FONT_DEFAULT_COLOR);
			}
			else {
				setBufferFontColor(color);
				if (isNPCStop) { setBufferFontColor(3); }
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♤");
				setBufferFontColor(FONT_DEFAULT_COLOR);
			}
		}
		else if (number == 6) {
			if (newNPC > 0) {
				npc->newNPC -= 1;
				setBufferFontColor(FONT_GREED_COLOR);
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♠");
				setBufferFontColor(FONT_DEFAULT_COLOR);
			}
			else {
				setBufferFontColor(color);
				if (isNPCStop) { setBufferFontColor(FONT_JADE_COLOR); }
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♠");
				setBufferFontColor(FONT_DEFAULT_COLOR);
			}
		}

		npc = npc->rightLink;
	} while (npc != npc_list.head);

}

// 맵을 출력
void printMap() {

	MAP map = ALL_MAP[ROUND];

	for (int y = 0; y < map.y; y++) {
		for (int x = 0; x < map.x; x++) {

			if (!enableSight) {
				if ((myX - restrictSightRange <= x && x <= myX + restrictSightRange) && (myY - restrictSightRange <= y && y <= myY + restrictSightRange)) {

				}
				else {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "　");
					continue;
				}
			}

			switch (map.map[y][x]) {
			case 0:
				break;
			case 1:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "□");
				break;
			case 2:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "■");
				break;
			case 3:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "▣");
				break;
			case 4:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "＊");
				break;
			case 5:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♠");
				break;
			case 6:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♤");
				break;
			case 7:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "◀");
				break;
			case 8:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "★");
				break;
			case 9:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "●");
				break;
			default:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "○");
				break;
			}
		}
	}

}

/*=====================================*/
/*=====================================*/


/*=======================*/
/*====== 충돌 체크 ======*/

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

/*=======================*/
/*=======================*/


/*==================================*/
/*====== 미사일에 관련된 액션 ======*/

// x, y좌표의 NPC를 지움
void removeNPC(int x, int y, boolean isDelete) {
	NPC_HEAD npc_list = NPC_LIST[ROUND];

	NPC* temp = npc_list.head;

	if (npc_list.num == 0) {
		return;
	}

	do {

		if (temp->x == x && temp->y == y) {

			addScore(temp->number); // 파괴한 NPC 점수 계산에 추가

			temp->number -= 1;
			if (isDelete) { // 완전 삭제가 필요한 경우
				temp->number -= 1;
			}

			if (temp->number < 5) { // 리스트에서 영구적으로 삭제함

				NPC_LIST[ROUND].num -= 1;

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

// x, y좌표의 벽을 지움
void removeWall(int x, int y) {

	int temp = ALL_MAP[ROUND].map[y][x];

	switch (temp) {
	case 1:
		ALL_MAP[ROUND].map[y][x] -= 1;
		addScore(temp);
		break;
	case 2:
	case 3:
		ALL_MAP[ROUND].map[y][x] -= 1;
		addScore(temp);
		break;
	}

}

/*==================================*/
/*==================================*/


/*=======================================*/
/*====== 객체의 이동과 관련된 함수 ======*/

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

		if (detectConflictWithNPC(tempX, tempY)) { // NPC와 충돌 체크
			if (isTankUnbeatable) { // 탱크가 무적일 경우
				return false;
			}
			else {
				isGameOver = true; // 게임을 종료 시킴
				return check;
			}
		}

		switch (map.map[tempY][tempX]) {
		case 0: // 탱크 이동시키기
		case 4:
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


		// 아이템을 먹은 경우
		if (detectConflictWithItem(myX, myY)) {
			ALL_MAP[ROUND].map[myY][myX] = 0;
			useItem();
		}

	}
	else {
		currentTankDirection = direction;
	}



	return check;

}

// NPC를 움직임 (스레드 사용)
void moveNPC(void * param) {

	OutputDebugString(L"Start Move NPC\n");

	while (!isGameOver) { // 게임이 종료될때까지 반복

		if (!isNPCStop) { // NPC가 멈춰야할 경우가 아니라면

			NPC_HEAD npc_list = NPC_LIST[ROUND];

			if (npc_list.num == 0) { // NPC가 없으면 종료
				continue;
			}

			NPC* npc = npc_list.head;
			MAP map = ALL_MAP[ROUND];

			do {

				while (1) { // 이동가능한 방향을 찾기 위해

					int x = npc->x;
					int y = npc->y;
					int direction = npc->direction;
					int remainMove = npc->remainMove;
					int* check = npc->check;


					boolean isInduce = true;

					if (isTankUnbeatable) {

						isInduce = false;

					}else if (x == myX) {
						if (y < myY) {
							direction = DIRECTION_DOWN;
							for (int y2 = y+1; y2 < myY; y2++) {
								if (detectConflictWithWall(x, y2)) {
									isInduce = false;
									break;
								}
							}
						}
						else {
							direction = DIRECTION_UP;
							for (int y2 = myY+1; y2 < y; y2++) {
								if (detectConflictWithWall(x, y2)) {
									isInduce = false;
									break;
								}
							}
						}
					}
					else if (y == myY) {
						if (x < myX) {
							direction = DIRECTION_RIGHT;
							for (int x2 = x+1; x2 < myX; x2++) {
								if (detectConflictWithWall(x2, y)) {
									isInduce = false;
									break;
								}
							}
						}
						else {
							direction = DIRECTION_LEFT;
							for (int x2 = myX+1; x2 < x; x2++) {
								if (detectConflictWithWall(x2, y)) {
									isInduce = false;
									break;
								}
							}
						}
					}
					else {
						isInduce = false;
					}

 					if (!isInduce) {

						if (remainMove == 0) { // NPC의 남은 이동횟수를 모두 소진한 경우

							if (check[1] == 1 && check[2] == 1 && check[3] == 1 && check[4] == 1) { // 모든 방향으로 이동이 불가능할때
								npc->check[1] = 0;
								npc->check[2] = 0;
								npc->check[3] = 0;
								npc->check[4] = 0;
								break;
							}

							while (1) { // 아직 체크하지 않은 방향을 얻어냄
								direction = rand() % 4 + 1;
								if (check[direction] != 1) {
									break;
								}
							}

							npc->direction = direction;
							npc->check[direction] = 1;

							remainMove = rand() % 5 + 1;
							npc->remainMove = remainMove;
						}

					}
					else {
						npc->direction = direction;
						remainMove = rand() % 5 + 1;
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
							isGameOver = true;
							return;
						}

					}

					if (!isInduce) {
						if (detectConflictWithWall(x, y) || detectConflictWithNPC(x, y)) { // 이동하지 못하는 경우에 새로운 이동방향을 찾아내기 위해
							npc->remainMove = 0;
							continue;
						}
					}


					npc->x = x;
					npc->y = y;
					npc->remainMove -= 1;
					npc->check[1] = 0;
					npc->check[2] = 0;
					npc->check[3] = 0;
					npc->check[4] = 0;
					if (isInduce) {
						npc->color = FONT_LIGHT_RED_COLOR;
					}
					else {
						npc->color = FONT_DEFAULT_COLOR;
					}

					break;

				}




				npc = npc->rightLink;

			} while (npc != npc_list.head);

		}

		Sleep(NPC_SPEED);

	}


	OutputDebugString(L"End Move NPC\n");

}

// 미사일을 이동시킴 (스레드 사용)
void moveMissile(void * param) {

	OutputDebugString(L"Start Move Missile\n");

	while (!isGameOver) {

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

				if (detectConflictWithNPC(m->x, m->y)) {
					m->isEnable = false;
					removeNPC(m->x, m->y, false);
				}
				else if (detectConflictWithNPC(x, y)) {
					m->isEnable = false;
					removeNPC(x, y, false);
				}
				else if (detectConflictWithWall(x, y)) {
					m->isEnable = false;
					removeWall(x, y);
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

		Sleep(200);

	}


	OutputDebugString(L"End Move Missile\n");

}

// 갈고리를 사용함
void useHook(void * param) {


	int hookDelay = 20;

	removeAllHook();

	int direction = currentTankDirection;

	int sumX, sumY;

	switch (direction) {
	case DIRECTION_LEFT:
		sumX = -1;
		sumY = 0;
		break;
	case DIRECTION_RIGHT:
		sumX = 1;
		sumY = 0;
		break;
	case DIRECTION_DOWN:
		sumX = 0;
		sumY = 1;
		break;
	case DIRECTION_UP:
		sumX = 0;
		sumY = -1;
		break;
	}

	isHook = true; // 훅이 출력될 수 있게끔

	MAP map = ALL_MAP[ROUND];

	int x = myX + sumX;
	int y = myY + sumY;

	boolean isNPCConflict = false;

	// 훅 이동을 위한 반복문
	while (1) {

		// 훅이 이동 가능한 상황일 경우
		if (map.map[y][x] != 1 && map.map[y][x] != 2 && map.map[y][x] != 3 && map.map[y][x] != 9 && map.map[y][x] != 4 && map.map[y][x] != 8) {

			addHook(x, y, direction);

			x += sumX;
			y += sumY;

			if (detectConflictWithNPC(x, y)) {
				isNPCConflict = true;
				break;
			}

			Sleep(hookDelay);

		}
		else {
			break;
		}

	}

	HOOK* temp = HOOK_LIST;
	HOOK * temp1;

	// 탱크 이동을 위한 반복문
	while (temp != NULL) {

		int x = temp->x;
		int y = temp->y;

		if (!isNPCConflict) { // NPC에 의해 튕겨날 경우를 연출하기 위해
			myX = x;
			myY = y;
		}

		HOOK_LIST = temp->next;
		free(temp);
		temp = HOOK_LIST;


		Sleep(hookDelay);

	}

	if (!isNPCConflict) {
		hookCount -= 1;
	}

	isHook = false;
	enableDirectionKey = true;



}

/*=======================================*/
/*=======================================*/

/*=======================================*/
/*====== 아이템 사용과 관련된 함수 ======*/

// 아이템을 사용
void useItem() {

	srand((unsigned int)time(NULL));
	int item = rand() % 5 + 6 - 5 * (itemDiceNumber % 2);


	switch (item) {
	case 1: // NPC 스피드 업
		_beginthread(NPCSpeedUp, 1, NULL);
		break;
	case 2: // 조작횟수 반감
		controlPoint = (controlPoint + 1) / 2;
		break;
	case 3: // 시야 가리기
		_beginthread(restrictSight, 2, NULL);
		Sleep(10);
		break;
	case 4: // 벽 단계 증가
		mapDestroy(true);
		break;
	case 5: // NPC 추가
		addNewNPC();
		break;
	case 6: // 조작횟수 증가
		controlPoint += 2;
		break;
	case 7: // 벽 단계 감소
		mapDestroy(false);
		break;
	case 8: // NPC 멈추기
		_beginthread(NPCStop, 3, NULL);
		break;
	case 9: // 플레이어 무적
		_beginthread(tankUnbeatable, 4, NULL);
		Sleep(10);
		printTank();
		break;
	case 10: // 원하는 NPC 삭제
		removeOneNPC();
		break;
	}

}

// 원하는 NPC 1개 삭제
void removeOneNPC() {

	BufferClear();

	// 시야가리기 일시적으로 취소
	boolean sightTemp = enableSight;
	enableSight = true;

	// NPC 일시적으로 멈추기
	boolean npcStopTemp = isNPCStop;
	isNPCStop = true;

	NPC_HEAD npc_list = NPC_LIST[ROUND];
	MAP map = ALL_MAP[ROUND];

	// NPC가 없다면 함수 종료
	if (npc_list.num == 0) {
		isNPCStop = npcStopTemp;
		enableSight = sightTemp;
		return;
	}


	NPC* npc = npc_list.head;

	int x = npc->x;
	int y = npc->y;
	int number = npc->number;

	// 버퍼 2개 모두 필요한 부분만 그려둠
	/*======================================*/

	printMap();
	printNPC();
	printTank();

	setBufferFontColor(FONT_LIGHT_RED_COLOR);
	if (number == 5) {
		BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♤");
	}
	else if (number == 6) {
		BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♠");
	}
	setBufferFontColor(FONT_DEFAULT_COLOR);

	Flipping();

	/*======================================*/

	BufferClear();

	printMap();
	printNPC();
	printTank();

	setBufferFontColor(FONT_LIGHT_RED_COLOR);
	if (number == 5) {
		BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♤");
	}
	else if (number == 6) {
		BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♠");
	}
	setBufferFontColor(FONT_DEFAULT_COLOR);

	Flipping();

	/*======================================*/

	boolean isSelected = false;


	while (!isSelected) {

		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				printNPC(); // NPC만 새로그리기
				npc = npc->leftLink;
				x = npc->x;
				y = npc->y;
				number = npc->number;

				// 선택된 NPC만 빨간색으로 그리기
				setBufferFontColor(FONT_LIGHT_RED_COLOR);
				if (number == 5) {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♤");
				}
				else if (number == 6) {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♠");
				}
				setBufferFontColor(FONT_DEFAULT_COLOR);

				Flipping();

				break;
			case RIGHT:
				printNPC();
				npc = npc->rightLink;
				x = npc->x;
				y = npc->y;
				number = npc->number;

				setBufferFontColor(FONT_LIGHT_RED_COLOR);
				if (number == 5) {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♤");
				}
				else if (number == 6) {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "♠");
				}
				setBufferFontColor(FONT_DEFAULT_COLOR);

				Flipping();

				break;
			case SPACE:
				removeNPC(x, y, true); // NPC 제거
				isSelected = true;
				break;
			}

		}


		Sleep(20);
	}

	isNPCStop = npcStopTemp;
	enableSight = sightTemp;
}

// NPC 속도 증가
void NPCSpeedUp(void * param) {

	OutputDebugString(L"NPCSpeedUp : IN\n");

	if (!isAlreadyNPCSpeed) {

		isAlreadyNPCSpeed = true;
		remainDiceNumberByNPCSpeed = diceEnableNumber - 3;

		NPC_SPEED = NPC_SPEED / 2;
		if (NPC_SPEED <= DEFAULT_NPC_SPEED / 4) {
			NPC_SPEED = DEFAULT_NPC_SPEED / 4;
		}

		while (remainDiceNumberByNPCSpeed < diceEnableNumber && (!isGameOver)) {
			Sleep(100);
		}

		NPC_SPEED = DEFAULT_NPC_SPEED;
		isAlreadyNPCSpeed = false;

	}
	else { // 이미 같은 스레드가 실행 중일 경우
		remainDiceNumberByNPCSpeed = diceEnableNumber - 3;
		NPC_SPEED = NPC_SPEED / 2;
		if (NPC_SPEED <= DEFAULT_NPC_SPEED / 4) {
			NPC_SPEED = DEFAULT_NPC_SPEED / 4;
		}
	}

	OutputDebugString(L"NPCSpeedUp : OUT\n");

}

// NPC 멈추기
void NPCStop(void * param) {

	OutputDebugString(L"NPCStop : IN\n");

	if (!isAlreadyNPCStop) {

		isAlreadyNPCStop = true;
		remainDiceNumberByNPCStop = diceEnableNumber - 3;

		isNPCStop = true;

		while (remainDiceNumberByNPCStop < diceEnableNumber && (!isGameOver)) {
			Sleep(100);
		}

		isNPCStop = false;
		isAlreadyNPCStop = false;

	}
	else {
		remainDiceNumberByNPCStop = diceEnableNumber - 3;
	}

	OutputDebugString(L"NPCStop : OUT\n");
}

// 탱크 무적
void tankUnbeatable(void * param) {

	OutputDebugString(L"tankUnbeatable : IN\n");

	if (!isAlreadyTankUnbeatable) {

		isAlreadyTankUnbeatable = true;
		remainDiceNumberByTankUnbeatable = diceEnableNumber - 3;

		isTankUnbeatable = true;

		while (remainDiceNumberByTankUnbeatable < diceEnableNumber && (!isGameOver)) {
			Sleep(100);
		}

		isTankUnbeatable = false;
		isAlreadyTankUnbeatable = false;

	}
	else {
		remainDiceNumberByTankUnbeatable = diceEnableNumber - 3;
	}
	OutputDebugString(L"tankUnbeatable : OUT\n");

}

// 시야 가리기
void restrictSight(void * param) {

	OutputDebugString(L"restrictSight : IN\n");
	if (!isAlreadySight) {

		isAlreadySight = true;
		remainDiceNumberBySight = diceEnableNumber - 4;

		enableSight = false;

		while (remainDiceNumberBySight < diceEnableNumber && (!isGameOver)) {
			Sleep(100);
		}

		enableSight = true;
		isAlreadySight = false;

	}
	else {
		remainDiceNumberBySight = diceEnableNumber - 4;
	}

	OutputDebugString(L"restrictSight : OUT\n");


}

// true일 경우 맵 파괴 단계 증가, flase일 경우 감소
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
				if (1 <= number && number <= 3) {
					ALL_MAP[ROUND].map[y][x] -= 1;
					if (ALL_MAP[ROUND].map[y][x] == 0) {
						BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "　");
					}
				}
			}
		}
	}

}

// 새로운 NPC 추가
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
		else {// 생성 가능한 위치가 아닐 경우 새롭게 좌표 생성
			x = rand() % ALL_MAP[ROUND].x;
			x = x - (x % 2);
			y = rand() % ALL_MAP[ROUND].y;
			y = y - (y % 2);
		}
	}

	addNPC(NPC_LIST[ROUND], x, y, number, 50);

}

/*=======================================*/
/*=======================================*/


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

				SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 9, GBOARD_ORIGIN_Y + (i * 5));

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

		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 15, GBOARD_ORIGIN_Y + (i * 5));
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
		printf("%2d 라운드 총 점수 = %d", ROUND+1, totalScore);
		TOTAL_SCORE += totalScore;
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
		LIFE -= 1;
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

	int count = 0;
	int temp = TOTAL_SCORE;

	while (temp > 0) {

		count += 1;
		temp /= 10;

	}

	// 기본 설정 너비가 예상 너비보다 작을 경우 너비를 늘려줌
	int width = 0;
	if (count * 10 > INIT_PAGE_WIDHT) {
		width = count * 10;
	}
	else {
		width = INIT_PAGE_WIDHT;
	}
	int tempWidth = count * 10;
	tempWidth = width / 2 - tempWidth / 2;

	system("cls");
	setConsoleSize(width, INIT_PAGE_HEIGHT);


	// 점수를 사용하기 편하게 배열에 추가
	temp = TOTAL_SCORE;
	int* num = (int*)malloc(sizeof(int)*count);

	for (int i = 0; i < count; i++) {
		num[i] = temp;
		temp /= 10;
	}

	for (int k = 0; k < count; k++) {
		//SetCurrentCursorPos(k * 10, INIT_PAGE_HEIGHT / 2);
		int a = num[count-k-1];
		for (int j = 0; j < BIG_NUMBER_HEIGHT; j++) {
			for (int i = 0; i < BIG_NUMBER_WIDHT; i++) {

				if (bigNumber[a%10][j][i] == 0) {
					SetCurrentCursorPos(tempWidth + k * 10 + i*2, INIT_PAGE_HEIGHT / 2 - BIG_NUMBER_HEIGHT/2 + j);
					printf("　");
				}
				else {
					SetCurrentCursorPos(tempWidth + k * 10 + i * 2, INIT_PAGE_HEIGHT / 2 - BIG_NUMBER_HEIGHT / 2 + j);
					printf("■");
				}

			}
		}


	}

	// 각종 안내 문구 출력
	if (LIFE <= 0) {
		SetCurrentCursorPos(width / 2 - 6, 5);
		printf("Game Over!!!");
	}
	else {
		SetCurrentCursorPos(width / 2 - 6, 5);
		printf("Game Clear!!");
	}

	SetCurrentCursorPos(width / 2 - 5, 15);
	printf("Total Score");

	SetCurrentCursorPos(width / 2 - 12, INIT_PAGE_HEIGHT / 2 + 15);
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


/*=======================*/
/*====== 부가 함수 ======*/

void setConsoleSize(int cols, int lines) {

	char commend[40];

	sprintf(commend, "mode con:cols=%d lines=%d", cols, lines);

	system(commend);

}

void setFontColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/*------ 스크린 버퍼 함수 ------*/

void CreateBuffer() {

	CONSOLE_CURSOR_INFO cci;
	COORD size = { CURRENT_CONSOLE_WIDTH, CURRENT_CONSOLE_HEIGHT };
	SMALL_RECT rect;

	rect.Left = 0;
	rect.Right = CURRENT_CONSOLE_WIDTH - 1;
	rect.Top = 0;
	rect.Bottom = CURRENT_CONSOLE_HEIGHT - 1;

	hBuffer[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleScreenBufferSize(hBuffer[0], size);
	SetConsoleWindowInfo(hBuffer[0], TRUE, &rect);
	hBuffer[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleScreenBufferSize(hBuffer[1], size);
	SetConsoleWindowInfo(hBuffer[1], TRUE, &rect);

	cci.dwSize = 1;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hBuffer[0], &cci);
	SetConsoleCursorInfo(hBuffer[1], &cci);

}

void BufferWrite(int x, int y, char *string) {
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(hBuffer[nBufferIndex], CursorPosition);
	WriteFile(hBuffer[nBufferIndex], string, strlen(string), &dw, NULL);
}

void Flipping() {
	SetConsoleActiveScreenBuffer(hBuffer[nBufferIndex]);
	nBufferIndex = !nBufferIndex;

}

void BufferClear() {
	COORD Coor = { 0, 0 };
	DWORD dw;
	FillConsoleOutputCharacter(hBuffer[nBufferIndex], ' ', CURRENT_CONSOLE_WIDTH * CURRENT_CONSOLE_HEIGHT, Coor, &dw);
}

void Release() {
	CloseHandle(hBuffer[0]);
	CloseHandle(hBuffer[1]);
}

void setBufferFontColor(int color) {
	SetConsoleTextAttribute(hBuffer[nBufferIndex], color);
}

/*------------------------------*/

/*=======================*/
/*=======================*/


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