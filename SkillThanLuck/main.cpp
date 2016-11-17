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

#define DEFAULT_NPC_SPEED 800

// ��ũ�� ���� ���� ����
int nBufferIndex = 0;
HANDLE hBuffer[2];

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
	int check[5];
	int newNPC;
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

typedef struct SCORE {
	int number;
	int count;
	SCORE* next;
}SCORE;

// ���� ���� �����
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
boolean enableDice = false;
boolean enableMoveNPC = false;
boolean isGameOver = false;
boolean isSuccess = false;
boolean isNeedPrintMap = false;
MAP* ALL_MAP = NULL;
NPC_HEAD* NPC_LIST = NULL;
MISSILE* ALL_MISSILE = NULL;
SCORE* SCORE_LIST = NULL;

// ������ ���� ���� ����
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

// �Լ� �����
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
void addNPC(NPC_HEAD& npc_list, int x, int y, int number, int newNPC);
void removeNPC(int x, int y, boolean isDelete);
void helpPage();
void gameStart();
void drawGamePage();
void printLife();
void printItemNotiMessage();
void removeWall(int x, int y);
boolean moveTank(int direction);
void moveNPC(void * param);
void removeAllMissile();
void addMissile(int x, int y, int direction);
void addScore(int number);
void removeAllScore();
void printMissile();
void moveMissile(void * param);
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

// ��ũ�� ���� ���� �Լ�
void CreateBuffer();
void BufferWrite(int x, int y, char *string);
void Flipping();
void BufferClear();
void Release();
void setBufferFontColor(int color);


/*================================================*/
/*=====================����=======================*/
/*================================================*/

int main() {

	RemoveCursor();

	boolean check = true;


	while (check) {

		ROUND = 0; // ���� �ʱ�ȭ

		initPage(); // ��Ʈ�� ������

		LIFE = 5; // ������ �ʱ�ȭ

		while (ROUND<MAXIMUM_ROUND) { // ������ ��� Ŭ���� �ϰų� �������� 0�϶����� �ݺ�

			gameStart(); // ���� ����
			Release(); // ���� Ŭ����

			showCurrentRoundScore(isSuccess); // ���� ������ ������ ������

			Sleep(DEFAULT_NPC_SPEED); // �ܿ� �����尡 ������ �ణ�� ���� ��
			int key = _getch();
			while (_kbhit() != 0) { key = _getch(); } // ���ʿ��� �Է� ���

			ROUND += 1; // ���带 ������Ű�� ���� ���� ����

			if (LIFE <= 0) { // �������� ��� �����Ǹ� ���� ����
				break;
			}

		}

		check = showFinishPage(); // ����ڰ� ���Ḧ ���ϸ� FALSE, ����� ���ϸ� TRUE

	}

	exit(1);

	return 0;
}

/*================================================*/
/*================================================*/
/*================================================*/


// �ʱ� ������
void initPage() {

	loadMap();

	drawInitDefault();

	TOTAL_SCORE = 0;
	int tempRound = -1;

	while (1) {

		if (ROUND != tempRound) { // ���尡 �ٲ�� ȭ�� ���ΰ�ħ
			tempRound = ROUND;
			SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 4, INIT_PAGE_HEIGHT / 2 + 2);
			printf("��  %d  ��", ROUND + 1);
		}


		int key;
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT: // ���� ���� ����
				ROUND = (ROUND - 1);
				if (ROUND < 0)
					ROUND = MAXIMUM_ROUND - 1;
				break;
			case RIGHT: // ���� ���� ����
				ROUND = (ROUND + 1) % MAXIMUM_ROUND;
				break;
			case KEY_I: // �ȳ��������� �̵�
				helpPage();
				drawInitDefault();
				break;
			case KEY_Q: // ���� ����
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

// �ʱ� �������� �׸�
void drawInitDefault() {

	system("cls");
	setConsoleSize(INIT_PAGE_WIDHT, INIT_PAGE_HEIGHT);

	// ���Ϸκ��� ū ���� �Է�
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

	// ū���� ���
	for (int j = 0; j < y; j++) {
		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - x, INIT_PAGE_HEIGHT / 2 - (y - j) - 4);
		for (int i = 0; i < x; i++) {
			if (str[j][i] == 0) {
				printf("��");
			}
			else {
				printf("��");
			}
		}
		printf("\n");
	}


	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 5, INIT_PAGE_HEIGHT / 2);
	printf("���� ����");


	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 4, INIT_PAGE_HEIGHT / 2 + 6);
	printf("Help : i");
	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 4, INIT_PAGE_HEIGHT / 2 + 8);
	printf("Quit : q");
	SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 11, INIT_PAGE_HEIGHT / 2 + 10);
	printf("press any key to start");
}

// �����ͷκ��� ���� �ε�
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

		// �� �ʱ�ȭ ����
		ALL_MAP[i].map = (int**)malloc(sizeof(int*)*ALL_MAP[i].y);
		for (int k = 0; k < ALL_MAP[i].y; k++) {
			ALL_MAP[i].map[k] = (int*)malloc(sizeof(int)*ALL_MAP[i].x);
		}


		// NPC_LIST �ʱ�ȭ ����
		NPC_LIST[i].num = 0;
		NPC_LIST[i].head = NULL;
		NPC_LIST[i].tail = NULL;


		// �� Ž�� ����
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

// NPC�� ���� ���Ḯ��Ʈ�� �߰�
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

// ���� �������� ǥ��
void helpPage() {

	system("cls");
	SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y);
	printf("����� ���Դϴ�. NPC�� ���� ������������ ������ �����ϼ���!\n\n\n");

	int y = GetCurrentCursorPos().Y;
	SetCurrentCursorPos(GBOARD_ORIGIN_X, GetCurrentCursorPos().Y);
	printf("�� Key\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* �� : ���� �̵�\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* �� : �Ʒ��� �̵�\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* �� : �������� �̵�\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* �� : ���������� �̵�\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* Space bar : �̻��� �߻�\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* Q : �ʱ�ȭ�鿡�� ����\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("* ESC : �ڷ� ����\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 30, y);
	printf("�� ���� ���\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* �� : 1�ܰ� ��\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* �� : 2�ܰ� ��\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* �� : 3�ܰ� ��\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* �� : 1�ܰ� NPC\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* �� : 2�ܰ� NPC\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* �� : ����\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 32, GetCurrentCursorPos().Y);
	printf("* ��,�� : �̻���\n\n\n");


	SetCurrentCursorPos(GBOARD_ORIGIN_X, GetCurrentCursorPos().Y);
	printf("�� ������\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X+2, GetCurrentCursorPos().Y);
	printf("������ �ֻ����� ������ ¦���� ������ ������, Ȧ���� �Ҹ��� ������ �� �������� ����˴ϴ�.\n\n\n");

	y = GetCurrentCursorPos().Y;

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GetCurrentCursorPos().Y);
	printf("�� ������ ������\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* NPC �Ͻ� ����\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* ���� Ƚ�� +2\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* ��� �ν� �� �ִ� �� �ܰ� ����\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* ��ũ �Ͻ� ����\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 4, GetCurrentCursorPos().Y);
	printf("* ���ϴ� NPC �ϳ� ����\n\n");

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 40, y);
	printf("�� �Ҹ��� ������\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* NPC �ӵ� ����\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* ���� Ƚ�� �ݰ�\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* �÷��̾� �þ� ������\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* ��� �ν� �� �ִ� �� �ܰ� ����\n\n");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 42, GetCurrentCursorPos().Y);
	printf("* NPC �߰�\n\n");
	

	SetCurrentCursorPos(GBOARD_ORIGIN_X, GetCurrentCursorPos().Y+2);
	printf("ESC, BACKSPACE : �ڷΰ���");

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

// ���� ����
void gameStart() {

	// ���ӽ��ۿ� �ʿ��� �ʱⰪ
	MAP map = ALL_MAP[ROUND];
	removeAllMissile();
	removeAllScore();


	CURRENT_CONSOLE_WIDTH = (map.x * 2) + (GBOARD_ORIGIN_X) * 4 + SUB_GBOARD_WIDTH * 2;
	CURRENT_CONSOLE_HEIGHT = INIT_PAGE_HEIGHT;
	setConsoleSize(CURRENT_CONSOLE_WIDTH, CURRENT_CONSOLE_HEIGHT);

	CreateBuffer();

	CURRENT_MAP_WIDTH = map.x * 2;
	CURRENT_MAP_HEIGHT = map.y;

	mainDiceNumber = 0;
	controlPoint = 0;
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

	// ������ ���� �������� �ʱ�ȭ
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

	drawGamePage();

	_beginthread(moveNPC, 1, NULL);
	_beginthread(moveMissile, 2, NULL);

	/*=======================*/

	while (diceEnableNumber >= 0) {

		if (isGameOver || isSuccess) {
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
				if (!enableDice) {
					if (moveTank(DIRECTION_LEFT)) {
						controlPoint -= 1;
						//drawRemainCount();
					}

					if (controlPoint <= 0) {
						if (diceEnableNumber == 0) {
							isGameOver = true;
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						mainDiceNumber = 0;
						//drawDice();
					}
				}
				break;
			case RIGHT:
				if (!enableDice) {
					if (moveTank(DIRECTION_RIGHT)) {
						controlPoint -= 1;
						//drawRemainCount();
					}

					if (controlPoint <= 0) {
						if (diceEnableNumber == 0) {
							isGameOver = true;
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						mainDiceNumber = 0;
						//drawDice();
					}
				}
				break;
			case UP:
				if (!enableDice) {
					if (moveTank(DIRECTION_UP)) {
						controlPoint -= 1;
						//drawRemainCount();
					}

					if (controlPoint <= 0) {
						if (diceEnableNumber == 0) {
							isGameOver = true;
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						mainDiceNumber = 0;
						//drawDice();
					}
				}
				break;
			case DOWN:
				if (!enableDice) {
					if (moveTank(DIRECTION_DOWN)) {
						controlPoint -= 1;
						//drawRemainCount();
					}

					if (controlPoint <= 0) {
						if (diceEnableNumber == 0) {
							isGameOver = true;
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						mainDiceNumber = 0;
						//drawDice();
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
					controlPoint = mainDiceNumber;
					itemDiceNumber = rand() % 6 + 1;
					//drawDice();
					//drawRemainCount();
				}
				else {
					addMissile(myX, myY, currentTankDirection);

					controlPoint -= 1;
					//drawRemainCount();

					if (controlPoint <= 0) {
						if (diceEnableNumber == 0) {
							isGameOver = true;
							return;
						}
						enableDice = true;
						enableMoveNPC = false;
						itemDiceNumber = 0;
						mainDiceNumber = 0;
						//drawDice();
					}
				}
				break;
			}

		}


		drawGamePage();

		Sleep(20);
	}

}


// ���� �ʱ� �����÷��� ȭ���� �׸�
void drawGamePage() {

	BufferClear();

	MAP map = ALL_MAP[ROUND];

	char string[12];

	sprintf(string, "Round #%d", ROUND + 1);

	BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y - 1, string);


	printLife();
	setBufferFontColor(FONT_DEFAULT_COLOR);
	printMap();
	printNPC();
	printMissile();
	printTank();

	// �߾Ӽ�
	for (int y = 0; y < CURRENT_CONSOLE_HEIGHT; y++) {
		BufferWrite(CURRENT_CONSOLE_WIDTH - SUB_GBOARD_WIDTH * 2 - (GBOARD_ORIGIN_X * 2), y, "��");
	}

	drawDice();

	drawRemainCount();

	printItemNotiMessage();

	Flipping();

}


void printLife() {

	for (int x = 1; x <= 5; x++) {
		setBufferFontColor(FONT_LIGHT_RED_COLOR);
		if (x <= LIFE) {
			BufferWrite(GBOARD_ORIGIN_X + CURRENT_MAP_WIDTH - (x * 2), GBOARD_ORIGIN_Y - 1, "��");
		}
		else {
			break;
		}
		setBufferFontColor(FONT_DEFAULT_COLOR);
	}

}

// �ֻ��� ������ �׸�
void drawDice() {

	char string[40];

	sprintf(string, "�ֻ����� ���� �� �ִ� ���� Ƚ�� : %2d", diceEnableNumber);

	BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 18, GBOARD_ORIGIN_Y, string);

	// ���̽� Ʋ
	for (int y = 0; y < DICE_HEIGHT + 3; y++) {
		if (y == 0) {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "��");
				}
				else if (x == SUB_GBOARD_WIDTH - 1) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "��");
				}
				else {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "��");
				}
			}
		}
		else if (y == DICE_HEIGHT + 3 - 1) {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "��");
				}
				else if (x == SUB_GBOARD_WIDTH - 1) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "��");
				}
				else {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "��");
				}
			}
		}
		else {
			for (int x = 0; x < SUB_GBOARD_WIDTH; x++) {
				if (x == 0 || x == SUB_GBOARD_WIDTH - 1) {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "��");
				}
				else {
					BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 2 + y, "��");
				}
			}
		}

	}


	for (int y = 0; y < DICE_HEIGHT; y++) {

		for (int x = 0; x < DICE_WIDTH; x++) {
			if (dice[mainDiceNumber][y][x] == 0) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[mainDiceNumber][y][x] == 1) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[mainDiceNumber][y][x] == 2) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[mainDiceNumber][y][x] == 3) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[mainDiceNumber][y][x] == 4) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[mainDiceNumber][y][x] == 5) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[mainDiceNumber][y][x] == 6) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[mainDiceNumber][y][x] == 7) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 6 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
		}

	}

	BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 10, GBOARD_ORIGIN_Y + 3 + DICE_HEIGHT, "���� Ƚ��");

	for (int y = 0; y < DICE_HEIGHT; y++) {

		for (int x = 0; x < DICE_WIDTH; x++) {
			if (dice[itemDiceNumber][y][x] == 0) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[itemDiceNumber][y][x] == 1) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[itemDiceNumber][y][x] == 2) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[itemDiceNumber][y][x] == 3) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[itemDiceNumber][y][x] == 4) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[itemDiceNumber][y][x] == 5) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[itemDiceNumber][y][x] == 6) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
			else if (dice[itemDiceNumber][y][x] == 7) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 8 + DICE_WIDTH * 2 + x * 2, GBOARD_ORIGIN_Y + 3 + y, "��");
			}
		}
	}
	BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH * 2 + 11 + DICE_WIDTH * 2, GBOARD_ORIGIN_Y + 3 + DICE_HEIGHT, "������ ��ȣ");


	if (controlPoint == 0) {
		BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 18, GBOARD_ORIGIN_Y + 2 + DICE_HEIGHT / 2 + 1, "Spacebar�� ���� �ֻ����� �����ּ���.");
	}

}

// ���� ����Ƚ�� ������ �׸�
void drawRemainCount() {


	char string[20];

	sprintf(string, "���� ���� Ƚ�� %d��", controlPoint);

	BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 10, GBOARD_ORIGIN_Y + 4 + DICE_HEIGHT + 2, string);


	for (int j = 0; j < BIG_NUMBER_HEIGHT; j++) {
		for (int i = 0; i < BIG_NUMBER_WIDHT; i++) {
			if (bigNumber[controlPoint][j][i] == 0) {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - BIG_NUMBER_WIDHT + i * 2, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + j, "��");
			}
			else {
				BufferWrite(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - BIG_NUMBER_WIDHT + i * 2, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + j, "��");
			}
		}
	}

}


// ��ũ�� �浹�� üũ(�浹�Ͻ� true��ȯ)
boolean detectConflictWithTank(int checkX, int checkY) {

	if (checkX == myX && checkY == myY) {
		return true;
	}

	return false;
}

// NPC�� �浹�� üũ(�浹�Ͻ� true��ȯ)
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

// ������ �浹�� üũ(�浹�Ͻ� true��ȯ)
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

// ������ ��ϰ� �浹�� üũ(�浹�Ͻ� true��ȯ)
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
		addScore(temp);
		break;
	case 2:
	case 3:
		ALL_MAP[ROUND].map[y][x] -= 1;
		addScore(temp);
		break;
	}

}

// ��ũ�� �̵���Ű�� �Լ�(�̵������ϸ� ture, �Ұ����ϸ� false ��ȯ)
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
			//BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "��");
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

	}
	else {
		currentTankDirection = direction;
	}



	return check;

}

// NPC�� ������(��ũ�� �浹�� ��� true ����)
void moveNPC(void * param) {

	OutputDebugString(L"Start Move NPC\n");

	while (!isGameOver) {

		if (!isNPCStop) {

			NPC_HEAD npc_list = NPC_LIST[ROUND];

			if (npc_list.num == 0) {
				continue;
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
							isGameOver = true;
							return;
						}

					}

					if (detectConflictWithWall(x, y) || detectConflictWithNPC(x, y)) {
						npc->remainMove = 0;
						continue;
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

		}

		Sleep(NPC_SPEED);

	}


	OutputDebugString(L"End Move NPC\n");

}

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

void printItemNotiMessage() {

	int count = 0;

	if (isAlreadyNPCSpeed) {
		char string[30];

		sprintf(string, "NPC �ӵ� ���� ������� : %d��", diceEnableNumber - remainDiceNumberByNPCSpeed);

		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 2 + count, string);

		count += 1;
	}

	if (isAlreadyNPCStop) {
		char string[26];

		sprintf(string, "NPC ���� ������� : %d��", diceEnableNumber - remainDiceNumberByNPCStop);

		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 2 + count, string);

		count += 1;
	}

	if (isAlreadyTankUnbeatable) {
		char string[30];

		sprintf(string, "�÷��̾� ���� ������� : %d��", diceEnableNumber - remainDiceNumberByTankUnbeatable);

		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 2 + count, string);

		count += 1;
	}

	if (isAlreadySight) {
		char string[26];

		sprintf(string, "�þ߰����� ������� : %d��", diceEnableNumber - remainDiceNumberBySight);

		BufferWrite(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + CURRENT_MAP_HEIGHT + 2 + count, string);

		count += 1;
	}

}

void useItem() {

	srand((unsigned int)time(NULL));
	int item = rand() % 5 + 6 - 5 * (itemDiceNumber % 2);


	switch (item) {
	case 1:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("������NPC ���ǵ� ��������");
		_beginthread(NPCSpeedUp, 1, NULL);
		break;
	case 2:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("����������Ƚ�� �ݰ�������");
		controlPoint = (controlPoint + 1) / 2;
		break;
	case 3:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("���������þ� �����⡡����");
		_beginthread(restrictSight, 2, NULL);
		Sleep(10);
		isNeedPrintMap = true;
		break;
	case 4:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("�������� �ܰ� ���� ������");
		mapDestroy(true);
		break;
	case 5:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("��������NPC  �߰���������");
		addNewNPC();
		break;
	case 6:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("����������Ƚ�� ����������");
		controlPoint += 2;
		break;
	case 7:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("�������� �ܰ� ���� ������");
		mapDestroy(false);
		break;
	case 8:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("��������NPC ���߱⡡������");
		_beginthread(NPCStop, 3, NULL);
		break;
	case 9:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("�������÷��̾� ����������");
		_beginthread(tankUnbeatable, 4, NULL);
		Sleep(10);
		printTank();
		break;
	case 10:
		//SetCurrentCursorPos(CURRENT_CONSOLE_WIDTH - GBOARD_ORIGIN_X - SUB_GBOARD_WIDTH - 13, GBOARD_ORIGIN_Y + 6 + DICE_HEIGHT + 2 + 10);
		//printf("���������ϴ� NPC ��������");

		removeOneNPC();

		break;
	}

}

void removeOneNPC() {

	BufferClear();

	boolean sightTemp = enableSight;
	enableSight = true;

	boolean npcStopTemp = isNPCStop;
	isNPCStop = true;

	NPC_HEAD npc_list = NPC_LIST[ROUND];
	MAP map = ALL_MAP[ROUND];

	if (npc_list.num == 0) {
		isNPCStop = npcStopTemp;
		enableSight = sightTemp;
		return;
	}


	NPC* npc = npc_list.head;

	int x = npc->x;
	int y = npc->y;
	int number = npc->number;

	/*======================================*/

	printMap();
	printNPC();
	printTank();

	setBufferFontColor(FONT_LIGHT_RED_COLOR);
	if (number == 5) {
		BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
	}
	else if (number == 6) {
		BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
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
		BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
	}
	else if (number == 6) {
		BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
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
				printNPC();
				npc = npc->leftLink;
				x = npc->x;
				y = npc->y;
				number = npc->number;

				setBufferFontColor(FONT_LIGHT_RED_COLOR);
				if (number == 5) {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				}
				else if (number == 6) {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
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
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				}
				else if (number == 6) {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				}
				setBufferFontColor(FONT_DEFAULT_COLOR);

				Flipping();

				break;
			case SPACE:
				removeNPC(x, y, true);
				isSelected = true;
				break;
			}

		}


		Sleep(20);
	}

	isNPCStop = npcStopTemp;
	enableSight = sightTemp;
}

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
	else {
		remainDiceNumberByNPCSpeed = diceEnableNumber - 3;
		NPC_SPEED = NPC_SPEED / 2;
		if (NPC_SPEED <= DEFAULT_NPC_SPEED / 4) {
			NPC_SPEED = DEFAULT_NPC_SPEED / 4;
		}
	}

	OutputDebugString(L"NPCSpeedUp : OUT\n");

}

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
		isNeedPrintMap = true;
		isAlreadySight = false;

	}
	else {
		remainDiceNumberBySight = diceEnableNumber - 4;
	}

	OutputDebugString(L"restrictSight : OUT\n");


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
				if (1 <= number && number <= 3) {
					ALL_MAP[ROUND].map[y][x] -= 1;
					if (ALL_MAP[ROUND].map[y][x] == 0) {
						BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
					}
				}
			}
		}
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

	addNPC(NPC_LIST[ROUND], x, y, number, 50);

}

void printMissile() {

	MISSILE* m = ALL_MISSILE;
	while (m != NULL) {

		if (m->isEnable) {
			int x = m->x;
			int y = m->y;
			int direction = m->direction;

			if (!enableSight) {
				if ((myX - restrictSightRange > x && x > myX + restrictSightRange) && (myY - restrictSightRange < y && y>myY + restrictSightRange)) {
					m = m->next;
					continue;
				}
			}

			switch (direction) {
			case DIRECTION_LEFT:
			case DIRECTION_RIGHT:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case DIRECTION_UP:
			case DIRECTION_DOWN:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			}
		}

		m = m->next;
	}



}

// ��ũ�� ���� ��ġ�� ���
void printTank() {

	if (isTankUnbeatable) {
		setBufferFontColor(FONT_RED_COLOR);
	}
	else {
		setBufferFontColor(PLAYER_COLOR);
	}
	switch (currentTankDirection) {
	case DIRECTION_LEFT:
		BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "��");
		break;
	case DIRECTION_RIGHT:
		BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "��");
		break;
	case DIRECTION_UP:
		BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "��");
		break;
	case DIRECTION_DOWN:
		BufferWrite(GBOARD_ORIGIN_X + (myX * 2), GBOARD_ORIGIN_Y + myY, "��");
		break;
	}
	setBufferFontColor(FONT_DEFAULT_COLOR);

}

// NPC�� ���� ��ġ�� ���
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

		if (!enableSight) {
			if ((myX - restrictSightRange <= x && x <= myX + restrictSightRange) && (myY - restrictSightRange <= y && y <= myY + restrictSightRange)) {

			}
			else {
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				npc = npc->rightLink;
				continue;
			}
		}

		if (number == 5) {
			if (newNPC > 0) {
				npc->newNPC -= 1;
				setBufferFontColor(FONT_GREED_COLOR);
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				setBufferFontColor(FONT_DEFAULT_COLOR);
			}
			else {
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
			}
		}
		else if (number == 6) {
			if (newNPC > 0) {
				npc->newNPC -= 1;
				setBufferFontColor(FONT_GREED_COLOR);
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				setBufferFontColor(FONT_DEFAULT_COLOR);
			}
			else {
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
			}
		}

		npc = npc->rightLink;
	} while (npc != npc_list.head);

}

// ���� ���(NPC, TANK ��� ��µ�)
void printMap() {

	MAP map = ALL_MAP[ROUND];

	for (int y = 0; y < map.y; y++) {
		for (int x = 0; x < map.x; x++) {

			if (!enableSight) {
				if ((myX - restrictSightRange <= x && x <= myX + restrictSightRange) && (myY - restrictSightRange <= y && y <= myY + restrictSightRange)) {

				}
				else {
					BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
					continue;
				}
			}

			switch (map.map[y][x]) {
			case 0:
				break;
			case 1:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case 2:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case 3:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case 4:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case 5:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case 6:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case 7:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case 8:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			case 9:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			default:
				BufferWrite(GBOARD_ORIGIN_X + (x * 2), GBOARD_ORIGIN_Y + y, "��");
				break;
			}
		}
	}

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

// ���� ���忡�� ȹ���� ������ �����ִ� ������(�������� ��� ����, ������ ��� ����� ȭ��)
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
					printf("��");
					break;
				case 2:
					printf("��");
					break;
				case 3:
					printf("��");
					break;
				case 5:
					printf("��");
					break;
				case 6:
					printf("��");
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
		printf("���� ���� Ƚ�� %d X 200 = %d", diceEnableNumber, diceEnableNumber * 200);
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
		printf("%2d ���� �� ���� = %d", ROUND+1, totalScore);
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
	else { // �ش� ���� ����
		ROUND -= 1;
		LIFE -= 1;
		SetCurrentCursorPos(INIT_PAGE_WIDHT / 2 - 14, INIT_PAGE_HEIGHT / 2);
		loadMap();
		printf("Press any key to restart");
	}


	// Ű �Է��� ����
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

// ���� Ŭ���� ������
boolean showFinishPage() {




	int count = 0;
	int temp = TOTAL_SCORE;

	while (temp > 0) {

		count += 1;
		temp /= 10;

	}

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
					printf("��");
				}
				else {
					SetCurrentCursorPos(tempWidth + k * 10 + i * 2, INIT_PAGE_HEIGHT / 2 - BIG_NUMBER_HEIGHT / 2 + j);
					printf("��");
				}

			}
		}


	}

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

// �ΰ� �Լ�
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

// ��ũ�� ���� �Լ�
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

// �������� �Լ�
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