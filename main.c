#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <conio.h>

/*坐标轴偏移常量*/
const int TRIMX = 30;
const int TRIMY = 60;

/*坐标轴宽高*/
const int AXIS_WIDTH = 80;
const int AXIS_HEIGHT = 40;
const int AXIS_ANGLE_W = 1;
const int AXIS_ANGLE_H = 2;

/*绘制缩放比例*/
const int ZOOM = 7;

/*坐标变量*/
COORD g_coord[65];
int g_group[65];
struct CENTER{
	int X;
	int Y;
	int tryCnt;
	int flag;
	int color;
} g_center[10];

/*能否继续分组*/
const int MAXTRY = 1000;
const int DIFF = 1;
int g_tag;

/*GDI相关变量*/
HDC g_hdc;

/*函数声明*/
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
	//清除屏幕
	system("cls");
	packedClearWind(g_hdc);
	//初始化变量
	g_tag = 1;
	g_hdc = packedGetDC();
	//初始化随机坐标数组
	cnt = initRandCoord(g_coord,15,65);
	//绘制坐标轴
	drawAxis();
	//根据坐标数组描点
	drawDotByCoord(g_coord,cnt);
	printf("共%d个点\n", cnt);
	//进行分组
	printf("请输入组数k(1-10): ");
	scanf("%d",&k);
	while(k>10 || k<1)
	{
		system("cls");
		printf("共%d个点\n", cnt);
		printf("请输入组数k(1-10): ");
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
		printf("分组次数: %d\n",tryCnt);
	}
	printf("分组结束, 按下回车键开始新的分组\n");
	while(ch=getch())
	{
		if(ch==13) main();
	}
	return 0;
}

/*初始化随机坐标数组*/
/*返回生成的坐标数量*/
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

/*查找重复坐标数组*/
/*1 - 重复, 0 - 唯一*/
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

/*封装 - GetDC*/
HDC packedGetDC()
{
	HDC hdc;
	TCHAR title[256];
	//获取控制台标题
	GetConsoleTitle(title, 256);
	//根据控制台标题获得句柄从而获得HDC
	hdc = GetDC(FindWindow(0, title));
	return hdc;
}

/*封装 - 画点*/
void packedDrawDot(int x,int y,int color,HDC hdc)
{
	HPEN hPen,hPenOld;
	//创建画笔
	hPen = CreatePen(PS_SOLID, 3, color);
	//选择画笔
	hPenOld = (HPEN)SelectObject(hdc, hPen);
	//绘制
	MoveToEx(hdc, TRIMX+x*ZOOM, TRIMY+y*ZOOM, NULL);
	LineTo(hdc,TRIMX+x*ZOOM, TRIMY+y*ZOOM);
}

/*封装 - 划线*/
void packedDrawLine(int xS,int yS,int xE,int yE,int color,HDC hdc)
{
	HPEN hPen,hPenOld;
	hPen = CreatePen(PS_SOLID, 2, color);
	hPenOld = (HPEN)SelectObject(hdc, hPen);
	MoveToEx(hdc, TRIMX+xS*ZOOM, TRIMY+yS*ZOOM, NULL);
	LineTo(hdc, TRIMX+xE*ZOOM, TRIMY+yE*ZOOM);
}

/*封装 - 输出文本*/
void packedPrintText(int x,int y,char str[],int len,int color,HDC hdc)
{
	SetTextColor(hdc,color);
	SetBkColor(hdc,RGB(0,0,0));
	SetBkMode(hdc,TRANSPARENT);
	TextOut(hdc,TRIMX+x*ZOOM,TRIMX+y*ZOOM,str,len);
}

/*封装 - 清空屏幕*/
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

/*绘制坐标轴*/
void drawAxis()
{
	int color;
	color = RGB(65,105,225);
	/*绘制y轴 - 从下到上*/
	packedDrawLine(0,AXIS_HEIGHT,0,0,color,g_hdc);
	packedDrawLine(0,0,0-AXIS_ANGLE_W,AXIS_ANGLE_H,color,g_hdc);
	packedDrawLine(0,0,AXIS_ANGLE_W,AXIS_ANGLE_H,color,g_hdc);
	/*绘制x轴 - 从做到右*/
	packedDrawLine(0,AXIS_HEIGHT,AXIS_WIDTH,AXIS_HEIGHT,color,g_hdc);
	packedDrawLine(AXIS_WIDTH,AXIS_HEIGHT,AXIS_WIDTH-AXIS_ANGLE_H,AXIS_HEIGHT-AXIS_ANGLE_W,color,g_hdc);
	packedDrawLine(AXIS_WIDTH,AXIS_HEIGHT,AXIS_WIDTH-AXIS_ANGLE_H,AXIS_HEIGHT+AXIS_ANGLE_W,color,g_hdc);
}

/*根据点坐标描点*/
void drawDotByCoord(COORD coord[], int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		//绘制点坐标
		packedDrawDot(coord[i].X,coord[i].Y,RGB(255,255,240),g_hdc);
	}
}

/*初始化中心点*/
void initCenterDot(COORD coord[], int cnt, struct CENTER center[], int k)
{
	int i,rndNum,x,y;
	COORD centerTmp[10];
	srand((unsigned)time(0));
	/*无重复初始化中心点*/
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
	/*将中心点坐标赋值到全局变量*/
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

/*更新中心点*/
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
		//误差及微调阈值
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
	//遍历所有中心点是否可以微调
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

/*分组中心点(第n次分组)*/
void groupCenterDot(COORD coord[], int cnt, struct CENTER center[], int k)
{
	char cnum[5]={0};
	int i,j,minI,len;
	double distance,minDistance;
	/*设置分组颜色*/
	srand((unsigned)time(0));
	for(i=0;i<k;i++)
	{
		if(!center[i].color){
			center[i].color = RGB(rand()%255+1,rand()%255+1,rand()%255+1);
		}
	}
	/*计算到中心点最短距离并分组*/
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
	/*绘制所有点编号*/
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