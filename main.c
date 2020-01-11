#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <conio.h>

/*������ƫ�Ƴ���*/
const int TRIMX = 30;
const int TRIMY = 60;

/*��������*/
const int AXIS_WIDTH = 80;
const int AXIS_HEIGHT = 40;
const int AXIS_ANGLE_W = 1;
const int AXIS_ANGLE_H = 2;

/*�������ű���*/
const int ZOOM = 7;

/*�������*/
COORD g_coord[65];
int g_group[65];
struct CENTER{
	int X;
	int Y;
	int tryCnt;
	int flag;
	int color;
} g_center[10];

/*�ܷ��������*/
const int MAXTRY = 1000;
const int DIFF = 1;
int g_tag;

/*GDI��ر���*/
HDC g_hdc;

/*��������*/
int isRandCoordRepeat(int x,int y,COORD coord[],int n);
int initRandCoord(COORD coord[], int minN, int maxN);
void drawAxis();
void drawDotByCoord(COORD coord[],int n);
void initCenterDot(COORD coord[], int cnt, struct CENTER center[], int k);
void trimCenterDot(COORD coord[], int cnt, struct CENTER center[], int k);
void groupCenterDot(COORD coord[], int cnt, struct CENTER center[], int k);
HDC packedGetDC();
void packedDrawDot(int x,int y,int color,HDC hdc);
void packedDrawLine(int xS,int yS, int xE, int yE, int color,HDC hdc);
void packedPrintText(int x,int y,char str[],int len,int color,HDC hdc);
void packedClearWind(HDC hdc);

int main()
{
	int cnt,k,tryCnt=1;
	char ch;
	//�����Ļ
	system("cls");
	packedClearWind(g_hdc);
	//��ʼ������
	g_tag = 1;
	g_hdc = packedGetDC();
	//��ʼ�������������
	cnt = initRandCoord(g_coord,15,65);
	//����������
	drawAxis();
	//���������������
	drawDotByCoord(g_coord,cnt);
	printf("��%d����\n", cnt);
	//���з���
	printf("����������k(1-10): ");
	scanf("%d",&k);
	while(k>10 || k<1)
	{
		system("cls");
		printf("��%d����\n", cnt);
		printf("����������k(1-10): ");
		scanf("%d",&k);
	}
	system("cls");
	initCenterDot(g_coord,cnt,g_center,k);
	groupCenterDot(g_coord,cnt,g_center,k);
	drawAxis();
	while(g_tag)
	{
		system("cls");
		packedClearWind(g_hdc);
		drawAxis();
		trimCenterDot(g_coord,cnt,g_center,k);
		groupCenterDot(g_coord,cnt,g_center,k);
		drawAxis();
		tryCnt++;
		printf("�������: %d\n",tryCnt);
	}
	printf("�������, ���»س�����ʼ�µķ���\n");
	while(ch=getch())
	{
		if(ch==13) main();
	}
	return 0;
}

/*��ʼ�������������*/
/*�������ɵ���������*/
int initRandCoord(COORD coord[], int minN, int maxN)
{
	int x,y;
	int cnt=0,i,n;
	srand((unsigned)time(0));
	n = rand()%(maxN+1)+minN;
	srand((unsigned)time(0));
	for(i=0;i<n;i++)
	{
		x = rand()%80+1;
		y = rand()%40+1;
		while(isRandCoordRepeat(x,y,coord,cnt))
		{
			x = rand()%80+1;
			y = rand()%40+1;
		}
		coord[i].X = x;
		coord[i].Y = y;
		cnt++;
		if(cnt>=maxN)
		{
			break;
		}
	}
	return cnt;
}

/*�����ظ���������*/
/*1 - �ظ�, 0 - Ψһ*/
int isRandCoordRepeat(int x,int y,COORD coord[],int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		if(x==coord[i].X && y==coord[i].Y)
		{
			return 1;
		}
	}
	return 0;
}

/*��װ - GetDC*/
HDC packedGetDC()
{
	HDC hdc;
	TCHAR title[256];
	//��ȡ����̨����
	GetConsoleTitle(title, 256);
	//���ݿ���̨�����þ���Ӷ����HDC
	hdc = GetDC(FindWindow(0, title));
	return hdc;
}

/*��װ - ����*/
void packedDrawDot(int x,int y,int color,HDC hdc)
{
	HPEN hPen,hPenOld;
	//��������
	hPen = CreatePen(PS_SOLID, 3, color);
	//ѡ�񻭱�
	hPenOld = (HPEN)SelectObject(hdc, hPen);
	//����
	MoveToEx(hdc, TRIMX+x*ZOOM, TRIMY+y*ZOOM, NULL);
	LineTo(hdc,TRIMX+x*ZOOM, TRIMY+y*ZOOM);
}

/*��װ - ����*/
void packedDrawLine(int xS,int yS,int xE,int yE,int color,HDC hdc)
{
	HPEN hPen,hPenOld;
	hPen = CreatePen(PS_SOLID, 2, color);
	hPenOld = (HPEN)SelectObject(hdc, hPen);
	MoveToEx(hdc, TRIMX+xS*ZOOM, TRIMY+yS*ZOOM, NULL);
	LineTo(hdc, TRIMX+xE*ZOOM, TRIMY+yE*ZOOM);
}

/*��װ - ����ı�*/
void packedPrintText(int x,int y,char str[],int len,int color,HDC hdc)
{
	SetTextColor(hdc,color);
	SetBkColor(hdc,RGB(0,0,0));
	SetBkMode(hdc,TRANSPARENT);
	TextOut(hdc,TRIMX+x*ZOOM,TRIMX+y*ZOOM,str,len);
}

/*��װ - �����Ļ*/
void packedClearWind(HDC hdc)
{
	HPEN hPen;
	HBRUSH hBrush;
	hBrush = CreateSolidBrush(RGB(0, 0, 0));
	hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	SelectObject(hdc, hBrush);
	SelectObject(hdc, hPen);
	Rectangle(hdc,0,0,TRIMX+AXIS_WIDTH*ZOOM+20,TRIMY+AXIS_HEIGHT*ZOOM+20);
}

/*����������*/
void drawAxis()
{
	int color;
	color = RGB(65,105,225);
	/*����y�� - ���µ���*/
	packedDrawLine(0,AXIS_HEIGHT,0,0,color,g_hdc);
	packedDrawLine(0,0,0-AXIS_ANGLE_W,AXIS_ANGLE_H,color,g_hdc);
	packedDrawLine(0,0,AXIS_ANGLE_W,AXIS_ANGLE_H,color,g_hdc);
	/*����x�� - ��������*/
	packedDrawLine(0,AXIS_HEIGHT,AXIS_WIDTH,AXIS_HEIGHT,color,g_hdc);
	packedDrawLine(AXIS_WIDTH,AXIS_HEIGHT,AXIS_WIDTH-AXIS_ANGLE_H,AXIS_HEIGHT-AXIS_ANGLE_W,color,g_hdc);
	packedDrawLine(AXIS_WIDTH,AXIS_HEIGHT,AXIS_WIDTH-AXIS_ANGLE_H,AXIS_HEIGHT+AXIS_ANGLE_W,color,g_hdc);
}

/*���ݵ��������*/
void drawDotByCoord(COORD coord[], int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		//���Ƶ�����
		packedDrawDot(coord[i].X,coord[i].Y,RGB(255,255,240),g_hdc);
	}
}

/*��ʼ�����ĵ�*/
void initCenterDot(COORD coord[], int cnt, struct CENTER center[], int k)
{
	int i,rndNum,x,y;
	COORD centerTmp[10];
	srand((unsigned)time(0));
	/*���ظ���ʼ�����ĵ�*/
	for(i=0;i<cnt;i++)
	{
		rndNum = rand()%cnt;
		x = coord[rndNum].X;
		y = coord[rndNum].Y;
		while(isRandCoordRepeat(x,y,centerTmp,i+1))
		{
			rndNum = rand()%cnt;
			x = coord[rndNum].X;
			y = coord[rndNum].Y;
		}
		centerTmp[i].X = x;
		centerTmp[i].Y = y;
		if(i>k)
		{
			break;
		}
	}
	/*�����ĵ����긳ֵ��ȫ�ֱ���*/
	for(i=0;i<k;i++)
	{
		g_center[i].X = centerTmp[i].X;
		g_center[i].Y = centerTmp[i].Y;
		g_center[i].tryCnt = 1;
		g_center[i].flag = 1;
		g_center[i].color = 0;
		packedDrawDot(centerTmp[i].X,centerTmp[i].Y,RGB(255,0,0),g_hdc);
	}
}

/*�������ĵ�*/
void trimCenterDot(COORD coord[], int cnt, struct CENTER center[], int k)
{
	int i,j,groupNum,tag=0;
	double avgX,avgY;
	for(i=0;i<k;i++)
	{
		if(!center[i].flag) continue;
		avgX = 0;
		avgY = 0;
		groupNum=0;
		for(j=0;j<cnt;j++)
		{
			if(g_group[j]!=i) continue;
			avgX+=coord[j].X;
			avgY+=coord[j].Y;
			groupNum++;
		}
		avgX/=groupNum;
		avgY/=groupNum;
		//��΢����ֵ
		if((int)avgX-center[i].X<=DIFF && (int)avgY-center[i].Y<=DIFF)
		{
			center[i].flag = 0;
			continue;
		}
		else
		{
			if(center[i].tryCnt>=MAXTRY)
			{
				center[i].flag = 0;
				continue;
			}
			center[i].X = (int)avgX;
			center[i].Y = (int)avgY;
			center[i].tryCnt++;
		}
	}
	//�����������ĵ��Ƿ����΢��
	for(i=0;i<k;i++)
	{
		if(center[i].flag)
		{
			tag=1;
			return;
		}
	}
	if(!tag)
	{
		g_tag = 0;
	}
}

/*�������ĵ�(��n�η���)*/
void groupCenterDot(COORD coord[], int cnt, struct CENTER center[], int k)
{
	char cnum[5]={0};
	int i,j,minI,len;
	double distance,minDistance;
	/*���÷�����ɫ*/
	srand((unsigned)time(0));
	for(i=0;i<k;i++)
	{
		if(!center[i].color){
			center[i].color = RGB(rand()%255+1,rand()%255+1,rand()%255+1);
		}
	}
	/*���㵽���ĵ���̾��벢����*/
	for(i=0;i<cnt;i++)
	{
		minDistance=-1;
		minI=-1;
		for(j=0;j<k;j++)
		{
			distance = (int)sqrt((coord[i].X-center[j].X)*(coord[i].X-center[j].X)+(coord[i].Y-center[j].Y)*(coord[i].Y-center[j].Y));
			if(minDistance<0)
			{
				minDistance = distance;
				minI = j;
				continue;
			}
			if(distance<minDistance)
			{
				minDistance = distance;
				minI = j;
			}
		}

		g_group[i] = minI;
	}
	/*�������е���*/
	for(i=0;i<cnt;i++)
	{
		j=g_group[i]+1;
		len=0;
		if(j<10)
		{
			cnum[0]=j+'0';
			len=1;
		}
		else if(j==10)
		{
			cnum[0]='1';
			cnum[1]='0';
			len=2;
		}
		packedDrawLine(coord[i].X,coord[i].Y,center[g_group[i]].X,center[g_group[i]].Y,center[g_group[i]].color,g_hdc);
		packedPrintText(coord[i].X,coord[i].Y,cnum,len,RGB(255,255,255),g_hdc);
	}
}