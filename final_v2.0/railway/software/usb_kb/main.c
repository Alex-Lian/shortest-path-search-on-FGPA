/*---------------------------------------------------------------------------
  --      main.c                                                    	   --
  --      Christine Chen                                                   --
  --      Ref. DE2-115 Demonstrations by Terasic Technologies Inc.         --
  --      Fall 2014                                                        --
  --                                                                       --
  --      For use with ECE 298 Experiment 7                                --
  --      UIUC ECE Department                                              --
  ---------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>
#include "system.h"
#include "alt_types.h"
#include <unistd.h>  // usleep
#include "sys/alt_irq.h"
#include <string.h>
#include "io_handler.h"

#include "cy7c67200.h"
#include "usb.h"
#include "lcp_cmd.h"
#include "lcp_data.h"

#define X 100000

#define MAP_LENTH 46
#define SPEED 60

#define ERROR -1


typedef struct Open_list Open_list, * pOpen_list;
typedef struct Node
{
    //父节�?
    pOpen_list  pFather;
    int   N;
    float G;
    float H;
    //F�?
    float F;
    //x,y坐标
    float x;
    float y;
}Node, * pNode;

typedef struct Open_list
{
    struct Open_list * next;
    struct Node node;
}Open_list, * pOpen_list;

//---------------------------------------------------------------------------//
//相关函数
float my_abs(float x);
//距离函数（给两个坐标(x1,y1)和（x2，y2））
//float my_distance(int x1, int y1, int x2, int y2);
//添加链表
void list_add_tail(pOpen_list my_list, pOpen_list add_node);
//遍历链表,返回与tmpY和tmpY匹配的节点指�?
pOpen_list list_browse(pOpen_list my_list, int N);
//判断链表中是不是有某个节点（通过坐标来确定），有的话返回0，没有返�?
int judge_node_exist(pOpen_list mylist, int N);
//删除链表中某个节点，通过坐标删除,并且返回这个删除的节点指针，方便加入到close list
pOpen_list list_delete_point(pOpen_list my_list, int N);
//找到链表中最小的f值的函数,输入链表，返回最小f的节�?
pOpen_list find_min_f(pOpen_list my_list);
//打印open list中的各个节点坐标以及F�?
void msg_open_list(pOpen_list my_list);
//打印父节点坐标函�?
void printf_father_node(pOpen_list my_list, int *p1, int *p2,int map[46][46], int *dis1, int *dis2);

void ptr(pOpen_list my_list, int *p1, int *p2);

int find_line(int p1, int p2);

int strtoint(char *a);
//---------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------//
//
//                                Main function
//
//----------------------------------------------------------------------------------------//
int main(void)
{
	IO_init();

	/*while(1)
	{
		IO_write(HPI_MAILBOX,COMM_EXEC_INT);
		printf("[ERROR]:routine mailbox data is %x\n",IO_read(HPI_MAILBOX));
		//UsbWrite(0xc008,0x000f);
		//UsbRead(0xc008);
		usleep(10*10000);
	}*/

	alt_u16 intStat;
	alt_u16 usb_ctl_val;
	static alt_u16 ctl_reg = 0;
	static alt_u16 no_device = 0;
	alt_u16 fs_device = 0;
	int keycode = 0;
	alt_u8 toggle = 0;
	alt_u8 data_size;
	alt_u8 hot_plug_count;
	alt_u16 code;


    float dis[MAP_LENTH][2] = //定义每个点的绝对位置
    {{ 571.0 , 97.0 } ,
    { 563.0 , 125.0 } ,
    { 547.0 , 160.0 } ,
    { 465.0 , 186.0 } ,
    { 478.0 , 199.0 } ,
    { 489.0 , 229.0 } ,
    { 527.0 , 235.0 } ,
    { 530.0 , 301.0 } ,
    { 520.0 , 311.0 } ,
    { 537.0 , 317.0 } ,
    { 511.0 , 375.0 } ,
    { 494.0 , 388.0 } ,
    { 498.0 , 286.0 } ,
    { 478.0 , 297.0 } ,
    { 463.0 , 334.0 } ,
    { 426.0 , 407.0 } ,
    { 436.0 , 416.0 } ,
    { 436.0 , 423.0 } ,
    { 392.0 , 458.0 } ,
    { 372.0 , 413.0 } ,
    { 375.0 , 259.0 } ,
    { 403.0 , 166.0 } ,
    { 444.0 , 213.0 } ,
    { 421.0 , 213.0 } ,
    { 434.0 , 311.0 } ,
    { 420.0 , 340.0 } ,
    { 333.0 , 195.0 } ,
    { 300.0 , 297.0 } ,
    { 324.0 , 318.0 } ,
    { 325.0 , 347.0 } ,
    { 285.0 , 367.0 } ,
    { 269.0 , 223.0 } ,
    { 182.0 , 303.0 } ,
    { 160.0 , 107.0 } ,
    { 75.0 ,  153.0 } ,
    { 177.0 , 125.0 } ,
    { 225.0 , 138.0 } ,
    { 236.0 , 149.0 } ,
    { 258.0 , 171.0 } ,
    { 289.0 , 233.0 } ,
    { 537.0 , 389.0 } ,
    { 528.0 , 414.0 } ,
    { 332.0 , 260.0 } ,
    { 377.0 , 390.0 } ,
    { 392.0 , 420.0 },
    { 478.0, 219.0 }};

    int map[MAP_LENTH][MAP_LENTH] =
{{0, 61, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 4500, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{61, 0, 159, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, 159, 0, 227, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, 227, 0, 30, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 164, 79, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, 30, 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 51},
{X, X, X, X, X, 0, 147, X, X, X, X, X, 189, 189, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 51},
{X, X, X, X, X, 147, 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, 0, 123, X, X, X, 215, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, 123, 0, 73, X, X, 96, 237, 225, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, 73, 0, 240, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, 240, 0, 129, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, 129, 0, X, X, 293, X, 220, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, 189, X, 215, 96, X, X, X, 0, 48, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, 189, X, X, 237, X, X, X, 48, 0, 250, X, X, X, X, X, X, X, 287, X, 141, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, 225, X, X, 293, X, 250, 0, X, X, X, X, X, X, X, X, X, 122, 92, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, 79, X, X, X, X, X, X, X, X, 173, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 405, X},
{X, X, X, X, X, X, X, X, X, X, X, 220, X, X, X, 79, 0, 18, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 18, 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 75, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, X, X, X, X, X, X, X, X, X, X, 149, X, X, X, X, X, X, X, X, X, X, X, X, 85, 350, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, X, X, 144, 278, X, X, X, 325, X, X, X, X, X, X, X, X, X, X, X, X, X, 57, X, X, X},
{4500, X, X, 164, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, X, X, X, X, 454, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, 79, X, X, X, X, X, X, X, X, X, 287, X, X, X, X, X, X, X, X, 0, 81, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 68},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 144, X, 81, 0, X, X, 381, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, 141, 122, X, X, X, X, X, 278, X, X, X, 0, 99, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, 92, 173, X, X, X, X, X, X, X, X, 99, 0, X, X, 521, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 272, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 454, X, 381, X, X, 0, X, X, X, X, X, X, X, X, X, X, X, X, 497, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, 78, X, 395, X, X, X, X, X, X, X, X, X, X, X, 275, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 325, X, X, X, X, 521, X, 78, 0, 306, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 306, 0, 143, X, X, X, X, X, X, X, X, X, X, X, X, 225, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 149, X, X, X, X, X, X, X, 395, X, 143, 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, 1289, X, X, X, X, X, X, 196, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 1289, 0, X, X, X, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, X, 53, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, 839, X, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 53, 839, 0, 163, X, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 163, 0, 513, X, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 513, 0, 244, X, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 244, 0, 538, X, X, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 497, X, X, X, X, 196, X, X, X, X, X, X, 538, 0, X, X, 137, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, 300, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 300, 0, X, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 57, X, X, X, X, X, X, 275, X, X, X, X, X, X, X, X, X, X, X, 137, X, X, 0, X, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 85, X, X, X, X, X, 272, X, X, X, 225, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, X, X},
{X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 405, X, X, 75, 350, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0, X},
{X, X, X, X, 51, 51, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 68, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 0}};


    volatile unsigned int * path1 = (unsigned int*) 0x000000c0; //lower path
	volatile unsigned int * path2 = (unsigned int*) 0x00000010; //higher path

    volatile unsigned int * pos_x = (unsigned int*) 0x000000e0; //input x position
	volatile unsigned int * pos_y = (unsigned int*) 0x000000d0; //input y position

    volatile unsigned int * beginpoint = (unsigned int*) 0x00000100; //input x position
	volatile unsigned int * endpoint = (unsigned int*) 0x000000f0; //input y position

    volatile unsigned int * distancehigh = (unsigned int*) 0x00000120; //high bit of score
	volatile unsigned int * distancelow = (unsigned int*) 0x00000110; //low bit of score

	* path1 = (unsigned int)0b00000000000000000000000000000000;
	* path2 = (unsigned int)0b00000000000000000000000000000000;

	* beginpoint=(unsigned int)0;
	* endpoint=(unsigned int)0;

	* distancehigh=(unsigned int)0;
	* distancelow=(unsigned int)0;

	int situation;
    int START;
    int END;
    int v = 0;
    char a[5];
    char b[5];
	printf("Please select a mode:(1)use console to enter. (2)use keyboard to enter. (3)use mouse\n");
	scanf("%d",&situation);
	printf("%d",situation);
	if(situation == 1){
		printf("Consule mode");
		while(1){
			int mode;
			if(v != 0){
				printf("Do you want to use again? press(1)-yes, press(2)-no");
				scanf("%d",&mode);
				* distancehigh = 0;
				* distancelow = 0;
			}
		v = 1;
		if(mode == 2){
			break;
		}
		* path1 = (unsigned int)0b00000000000000000000000000000000;
		* path2 = (unsigned int)0b00000000000000000000000000000000;
		int y = 1;
		while(y == 1){
		while(1){
		printf("please enter the start point\n");
		scanf("%s", a);
		END = strtoint(a);
		if(END != -1){
			break;
		}
		printf("INPUT WRONG!! AGAIN!!\n");
		}
		while(1){
		printf("please enter the end   point\n");
		scanf("%s", b);
		START = strtoint(b);
		if(START != -1){
			break;
		}
		printf("INPUT WRONG!! AGAIN!!\n");
		}
		if(END == START){
			printf("Please enter different place\n");
		}
		if(((END == 40)&&(START != 41))&&((END == 41)&&(START != 40))){
			printf("Taiwan and Mainland are not connected, but will soon be.\n");
		}
		y = 0;
		}
		*endpoint=END+1;
		*beginpoint=START+1;

		int i;    //计数来遍�?
		pOpen_list pO = (pOpen_list)malloc(sizeof(Open_list));
		 pO->next = NULL;
		 //pO = NULL;  千万不可以，NULL不可以访问的，除非pO赋��?
		 pOpen_list pC = (pOpen_list)malloc(sizeof(Open_list));
		 pC->next = NULL;

		 //创建起始节点并初始化，创建一个目标节�?
		 pOpen_list start = (pOpen_list)malloc(sizeof(Open_list));
		 start->next = NULL;
		pOpen_list end = (pOpen_list)malloc(sizeof(Open_list));
		end->next = NULL;

		 start->node.pFather = NULL;
		 start->node.N = START;
		 start->node.x = dis[START][0];
		 start->node.y = dis[START][1];
		 start->node.G = 0;
		 start->node.H = (my_abs(dis[START][0] - dis[END][0]) + my_abs(dis[START][0] - dis[END][0]))/SPEED;
		 start->node.F = start->node.G + start->node.H;

		end->node.pFather = NULL;
		 end->node.N = END;
		 end->node.x = dis[END][0];
		 end->node.y = dis[END][1];

		 //起始节点加入到open list
		 list_add_tail(pO, start);
		//************************************************************************//
		int cir = 1;
		while(cir)
		{
			//printf("-------------------begin while--------------\n");
			//寻找朢�小的F值节点，记为pCurrent（第丢�次循环也就是起点�?
			//pOpen_list pCurrent = (pOpen_list)malloc(sizeof(Open_list));
			//msg_open_list(pO);	//现在open list中有的节�?
			pOpen_list pCurrent = find_min_f(pO);  //有可能找不到

			//*********************************************************************//

			//把当前点从open list中移除（通过坐标），加入到close list，记为p
			pOpen_list p = list_delete_point(pO, pCurrent->node.N);
			list_add_tail(pC, p);
			//printf("core is (%d, %d)\n", p->node.x, p->node.y);

			//printf("now ,the open list is as follow\n");
			//msg_open_list(pC);
			//ptr(pC, path1, path2);
			* path1 = (unsigned int)0b00000000000000000000000000000000;
			* path2 = (unsigned int)0b00000000000000000000000000000000;
			//ptr(pC,path1,path2);

			//printf("------------------------begin for  for------------------\n");

			//*********************************************************************//

			//还要考虑是不是障碍物，有没有在close list或��open list,考虑是不是边�?
			//这块应该是遍历当前节�?p->node.x, p->node.y)的四周，并且都加入open list
			for (i = 0; i <= MAP_LENTH - 1; i++){
				if (i == p->node.N)
					continue;
				if (map[p->node.N][i] == X)
					continue;
				if (!(judge_node_exist(pC, i))){
					continue;
				}
				if (i == END){
					end->node.pFather = p;
					cir = 0;
					break;
				}
				if(judge_node_exist(pO, i)){
					pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
					pTmp->next =NULL;

					pTmp->node.pFather = p;  //父节点为当前节点

					pTmp->node.N = i;
					pTmp->node.x = dis[i][0];
					pTmp->node.y = dis[i][1];
					//节点G.H.F�?      G值��么算好(找到父节点和现在节点的坐标了)
					pTmp->node.G = p->node.F + map[p->node.N][i];   //初始节点到其实际距离
					pTmp->node.H = (my_abs(end->node.x - pTmp->node.x) + my_abs(end->node.y - pTmp->node.y))/SPEED;
					pTmp->node.F = pTmp->node.G + pTmp->node.H;

					//加入到open list
					list_add_tail(pO, pTmp);
					//msg_open_list(pO);

				}
				else{    //在open list中了
				//printf("---------------------in open list-------------------\n");
				//printf("(%d) in open list\n", p->node.N);
				//首先根据坐标找到他的指针
				//pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
				//pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
				pOpen_list pTmp = list_browse(pO, i);
				//定义好两个的G�?丢�个是原本的G值，丢�个是通过当前节点到的G�?
				//扢�以用核心节点的G值加上当前节点到核心节点的��?
				float currentG = p->node.F + map[p->node.N][i];
				float pastG = pTmp->node.G;

				//printf("currentG: %f      pastG: %f\n", currentG, pastG);
				if(currentG < pastG)    //当前更优
				{
					pTmp->node.pFather = p; //更换父节�?
					//注意，更改F值和G值一�?
					pTmp->node.G = currentG;
					pTmp->node.F = pTmp->node.G + pTmp->node.H;
				}
				}
				if(cir == 0)	//跳出外层的for循环
					break;
			}

		}
		printf_father_node(end,  path1,  path2, map, distancehigh, distancelow);

	}
	}
	if(situation == 1){
//	printf("%d\n",* beginpoint);
//	printf("%d",*endpoint);

//    printf("please enter the start point");
//    scanf("%d", &START);
//    printf("please enter the end   point");
//    scanf("%d", &END);





    //************************************************************************//

    //2.把起点放入open list

    //创建丢�个指针pO指向open list和一个指针pC指向close list，注意现在open list是pO指向的那块内�?




    //*path1 += 1;
    //*path2 += 2;
	printf("USB keyboard setup...\n\n");

	//----------------------------------------SIE1 initial---------------------------------------------------//
	USB_HOT_PLUG:
	UsbSoftReset();

	// STEP 1a:
	UsbWrite (HPI_SIE1_MSG_ADR, 0);
	UsbWrite (HOST1_STAT_REG, 0xFFFF);

	/* Set HUSB_pEOT time */
	UsbWrite(HUSB_pEOT, 600); // adjust the according to your USB device speed

	usb_ctl_val = SOFEOP1_TO_CPU_EN | RESUME1_TO_HPI_EN;// | SOFEOP1_TO_HPI_EN;
	UsbWrite(HPI_IRQ_ROUTING_REG, usb_ctl_val);

	intStat = A_CHG_IRQ_EN | SOF_EOP_IRQ_EN ;
	UsbWrite(HOST1_IRQ_EN_REG, intStat);
	// STEP 1a end

	// STEP 1b begin
	UsbWrite(COMM_R0,0x0000);//reset time
	UsbWrite(COMM_R1,0x0000);  //port number
	UsbWrite(COMM_R2,0x0000);  //r1
	UsbWrite(COMM_R3,0x0000);  //r1
	UsbWrite(COMM_R4,0x0000);  //r1
	UsbWrite(COMM_R5,0x0000);  //r1
	UsbWrite(COMM_R6,0x0000);  //r1
	UsbWrite(COMM_R7,0x0000);  //r1
	UsbWrite(COMM_R8,0x0000);  //r1
	UsbWrite(COMM_R9,0x0000);  //r1
	UsbWrite(COMM_R10,0x0000);  //r1
	UsbWrite(COMM_R11,0x0000);  //r1
	UsbWrite(COMM_R12,0x0000);  //r1
	UsbWrite(COMM_R13,0x0000);  //r1
	UsbWrite(COMM_INT_NUM,HUSB_SIE1_INIT_INT); //HUSB_SIE1_INIT_INT
	IO_write(HPI_MAILBOX,COMM_EXEC_INT);

	while (!(IO_read(HPI_STATUS) & 0xFFFF) )  //read sie1 msg register
	{
	}
	while (IO_read(HPI_MAILBOX) != COMM_ACK)
	{
		printf("[ERROR]:routine mailbox data is %x\n",IO_read(HPI_MAILBOX));
		goto USB_HOT_PLUG;
	}
	// STEP 1b end

	printf("STEP 1 Complete");
	// STEP 2 begin
	UsbWrite(COMM_INT_NUM,HUSB_RESET_INT); //husb reset
	UsbWrite(COMM_R0,0x003c);//reset time
	UsbWrite(COMM_R1,0x0000);  //port number
	UsbWrite(COMM_R2,0x0000);  //r1
	UsbWrite(COMM_R3,0x0000);  //r1
	UsbWrite(COMM_R4,0x0000);  //r1
	UsbWrite(COMM_R5,0x0000);  //r1
	UsbWrite(COMM_R6,0x0000);  //r1
	UsbWrite(COMM_R7,0x0000);  //r1
	UsbWrite(COMM_R8,0x0000);  //r1
	UsbWrite(COMM_R9,0x0000);  //r1
	UsbWrite(COMM_R10,0x0000);  //r1
	UsbWrite(COMM_R11,0x0000);  //r1
	UsbWrite(COMM_R12,0x0000);  //r1
	UsbWrite(COMM_R13,0x0000);  //r1

	IO_write(HPI_MAILBOX,COMM_EXEC_INT);

	while (IO_read(HPI_MAILBOX) != COMM_ACK)
	{
		printf("[ERROR]:routine mailbox data is %x\n",IO_read(HPI_MAILBOX));
		goto USB_HOT_PLUG;
	}
	// STEP 2 end

	ctl_reg = USB1_CTL_REG;
	no_device = (A_DP_STAT | A_DM_STAT);
	fs_device = A_DP_STAT;
	usb_ctl_val = UsbRead(ctl_reg);

	if (!(usb_ctl_val & no_device))
	{
		for(hot_plug_count = 0 ; hot_plug_count < 5 ; hot_plug_count++)
		{
			usleep(5*1000);
			usb_ctl_val = UsbRead(ctl_reg);
			if(usb_ctl_val & no_device) break;
		}
		if(!(usb_ctl_val & no_device))
		{
			printf("\n[INFO]: no device is present in SIE1!\n");
			printf("[INFO]: please insert a USB keyboard in SIE1!\n");
			while (!(usb_ctl_val & no_device))
			{
				usb_ctl_val = UsbRead(ctl_reg);
				if(usb_ctl_val & no_device)
					goto USB_HOT_PLUG;

				usleep(2000);
			}
		}
	}
	else
	{
		/* check for low speed or full speed by reading D+ and D- lines */
		if (usb_ctl_val & fs_device)
		{
			printf("[INFO]: full speed device\n");
		}
		else
		{
			printf("[INFO]: low speed device\n");
		}
	}



	// STEP 3 begin
	//------------------------------------------------------set address -----------------------------------------------------------------
	UsbSetAddress();

	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		UsbSetAddress();
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506); // i
	printf("[ENUM PROCESS]:step 3 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508); // n
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]:step 3 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03) // retries occurred
	{
		usb_ctl_val = UsbGetRetryCnt();

		goto USB_HOT_PLUG;
	}

	printf("------------[ENUM PROCESS]:set address done!---------------\n");

	// STEP 4 begin
	//-------------------------------get device descriptor-1 -----------------------------------//
	// TASK: Call the appropriate function for this step.
	UsbGetDeviceDesc1(); 	// Get Device Descriptor -1

	//usleep(10*1000);
	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		// TASK: Call the appropriate function again if it wasn't processed successfully.
		UsbGetDeviceDesc1();
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506);
	printf("[ENUM PROCESS]:step 4 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508);
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]:step 4 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03)
	{
		usb_ctl_val = UsbGetRetryCnt();

		printf("Encountered Fatal Error");
		goto USB_HOT_PLUG;
	}

	printf("---------------[ENUM PROCESS]:get device descriptor-1 done!-----------------\n");


	//--------------------------------get device descriptor-2---------------------------------------------//
	//get device descriptor
	// TASK: Call the appropriate function for this step.
	UsbGetDeviceDesc2(); 	// Get Device Descriptor -2

	//if no message
	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		//resend the get device descriptor
		//get device descriptor
		// TASK: Call the appropriate function again if it wasn't processed successfully.
		UsbGetDeviceDesc2();
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506);
	printf("[ENUM PROCESS]:step 4 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508);
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]:step 4 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03)
	{
		usb_ctl_val = UsbGetRetryCnt();

		printf("Encountered Fatal Error");
		goto USB_HOT_PLUG;
	}

	printf("------------[ENUM PROCESS]:get device descriptor-2 done!--------------\n");


	// STEP 5 begin
	//-----------------------------------get configuration descriptor -1 ----------------------------------//
	// TASK: Call the appropriate function for this step.
	UsbGetConfigDesc1(); 	// Get Configuration Descriptor -1

	//if no message
	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		//resend the get device descriptor
		//get device descriptor

		// TASK: Call the appropriate function again if it wasn't processed successfully.
		UsbGetConfigDesc1();
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506);
	printf("[ENUM PROCESS]:step 5 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508);
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]:step 5 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03)
	{
		usb_ctl_val = UsbGetRetryCnt();

		printf("Encountered Fatal Error");
		goto USB_HOT_PLUG;
	}
	printf("------------[ENUM PROCESS]:get configuration descriptor-1 pass------------\n");

	// STEP 6 begin
	//-----------------------------------get configuration descriptor-2------------------------------------//
	//get device descriptor
	// TASK: Call the appropriate function for this step.
	UsbGetConfigDesc2(); 	// Get Configuration Descriptor -2

	usleep(100*1000);
	//if no message
	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		// TASK: Call the appropriate function again if it wasn't processed successfully.
		UsbGetConfigDesc2();
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506);
	printf("[ENUM PROCESS]:step 6 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508);
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]:step 6 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03)
	{
		usb_ctl_val = UsbGetRetryCnt();

		printf("Encountered Fatal Error");
		goto USB_HOT_PLUG;
	}


	printf("-----------[ENUM PROCESS]:get configuration descriptor-2 done!------------\n");


	// ---------------------------------get device info---------------------------------------------//

	// TASK: Write the address to read from the memory for byte 7 of the interface descriptor to HPI_ADDR.
	IO_write(HPI_ADDR,0x056c);
	code = IO_read(HPI_DATA);
	code = code & 0x003;
	printf("\ncode = %x\n", code);

	if (code == 0x01)
	{
		printf("\n[INFO]:check TD rec data7 \n[INFO]:Keyboard Detected!!!\n\n");
	}
	else
	{
		printf("\n[INFO]:Keyboard Not Detected!!! \n\n");
	}

	// TASK: Write the address to read from the memory for the endpoint descriptor to HPI_ADDR.

	IO_write(HPI_ADDR,0x0576);
	IO_write(HPI_DATA,0x073F);
	IO_write(HPI_DATA,0x8105);
	IO_write(HPI_DATA,0x0003);
	IO_write(HPI_DATA,0x0008);
	IO_write(HPI_DATA,0xAC0A);
	UsbWrite(HUSB_SIE1_pCurrentTDPtr,0x0576); //HUSB_SIE1_pCurrentTDPtr

	//data_size = (IO_read(HPI_DATA)>>8)&0x0ff;
	//data_size = 0x08;//(IO_read(HPI_DATA))&0x0ff;
	//UsbPrintMem();
	IO_write(HPI_ADDR,0x057c);
	data_size = (IO_read(HPI_DATA))&0x0ff;
	printf("[ENUM PROCESS]:data packet size is %d\n",data_size);
	// STEP 7 begin
	//------------------------------------set configuration -----------------------------------------//
	// TASK: Call the appropriate function for this step.
	UsbSetConfig();		// Set Configuration

	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		// TASK: Call the appropriate function again if it wasn't processed successfully.
		UsbSetConfig();		// Set Configuration
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506);
	printf("[ENUM PROCESS]:step 7 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508);
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]:step 7 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03)
	{
		usb_ctl_val = UsbGetRetryCnt();

		printf("Encountered Fatal Error");
		goto USB_HOT_PLUG;
	}

	printf("------------[ENUM PROCESS]:set configuration done!-------------------\n");

	//----------------------------------------------class request out ------------------------------------------//
	// TASK: Call the appropriate function for this step.
	UsbClassRequest();

	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		// TASK: Call the appropriate function again if it wasn't processed successfully.
		UsbClassRequest();
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506);
	printf("[ENUM PROCESS]:step 8 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508);
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]:step 8 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03)
	{
		usb_ctl_val = UsbGetRetryCnt();

		printf("Encountered Fatal Error");
		goto USB_HOT_PLUG;
	}


	printf("------------[ENUM PROCESS]:class request out done!-------------------\n");

	// STEP 8 begin
	//----------------------------------get descriptor(class 0x21 = HID) request out --------------------------------//
	// TASK: Call the appropriate function for this step.
	UsbGetHidDesc();

	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		// TASK: Call the appropriate function again if it wasn't processed successfully.
		UsbGetHidDesc();
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506);
	printf("[ENUM PROCESS]:step 8 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508);
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]:step 8 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03)
	{
		usb_ctl_val = UsbGetRetryCnt();
		printf("Encountered Fatal Error");
		goto USB_HOT_PLUG;
	}

	printf("------------[ENUM PROCESS]:get descriptor (class 0x21) done!-------------------\n");

	// STEP 9 begin
	//-------------------------------get descriptor (class 0x22 = report)-------------------------------------------//
	// TASK: Call the appropriate function for this step.
	UsbGetReportDesc();
	//if no message
	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
	{
		// TASK: Call the appropriate function again if it wasn't processed successfully.
		UsbGetReportDesc();
		usleep(10*1000);
	}

	UsbWaitTDListDone();

	IO_write(HPI_ADDR,0x0506);
	printf("[ENUM PROCESS]: step 9 TD Status Byte is %x\n",IO_read(HPI_DATA));

	IO_write(HPI_ADDR,0x0508);
	usb_ctl_val = IO_read(HPI_DATA);
	printf("[ENUM PROCESS]: step 9 TD Control Byte is %x\n",usb_ctl_val);
	while (usb_ctl_val != 0x03)
	{
		usb_ctl_val = UsbGetRetryCnt();
		printf("Encountered Fatal Error");
		goto USB_HOT_PLUG;
	}

	printf("---------------[ENUM PROCESS]:get descriptor (class 0x22) done!----------------\n");



	//-----------------------------------get keycode value------------------------------------------------//
	usleep(10000);
    int x1_p;
    int y1_p;
    int x2_p;
    int y2_p;
    int bool = 0;
	while(1)
	{
		toggle++;
		IO_write(HPI_ADDR,0x0500); //the start address
		//data phase IN-1
		IO_write(HPI_DATA,0x051c); //500

		IO_write(HPI_DATA,0x000f & data_size);//2 data length

		IO_write(HPI_DATA,0x0291);//4 //endpoint 1
		if(toggle%2)
		{
			IO_write(HPI_DATA,0x0001);//6 //data 1
		}
		else
		{
			IO_write(HPI_DATA,0x0041);//6 //data 1
		}
		IO_write(HPI_DATA,0x0013);//8
		IO_write(HPI_DATA,0x0000);//a
		UsbWrite(HUSB_SIE1_pCurrentTDPtr,0x0500); //HUSB_SIE1_pCurrentTDPtr

		while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
		{
			IO_write(HPI_ADDR,0x0500); //the start address
			//data phase IN-1
			IO_write(HPI_DATA,0x051c); //500

			IO_write(HPI_DATA,0x000f & data_size);//2 data length

			IO_write(HPI_DATA,0x0291);//4 //endpoint 1
			if(toggle%2)
			{
				IO_write(HPI_DATA,0x0001);//6 //data 1
			}
			else
			{
				IO_write(HPI_DATA,0x0041);//6 //data 1
			}
			IO_write(HPI_DATA,0x0013);//8
			IO_write(HPI_DATA,0x0000);//
			UsbWrite(HUSB_SIE1_pCurrentTDPtr,0x0500); //HUSB_SIE1_pCurrentTDPtr
			usleep(10*1000);
		}//end while

		usb_ctl_val = UsbWaitTDListDone();

		// The first two keycodes are stored in 0x051E. Other keycodes are in
		// subsequent addresses.
		keycode = UsbRead(0x051e);
//		printf("\nfirst two keycode values are %04x\n",keycode);
		// We only need the first keycode, which is at the lower byte of keycode.
		// Send the keycode to hardware via PIO.
		*keycode_base = keycode & 0xff;
//		printf("X position is %d\n",*pos_x);
//		printf("Y position is %d\n",*pos_y);
		usleep(200);//usleep(5000);

	    if((*pos_x != 0)||(*pos_y != 0)){
	    	printf("X position is %d\n",*pos_x);
			printf("Y position is %d\n",*pos_y);
	    	if(bool == 0){
	    		* path1 = (unsigned int)0b00000000000000000000000000000000;
	    		* path2 = (unsigned int)0b00000000000000000000000000000000;
				* distancehigh = 0;
				* distancelow = 0;
			    x1_p = *pos_x;
			    y1_p = *pos_y;
			    bool = 1;
			    usleep(100000);
			    for(int i = 0;i<=45;i++){
			    	if (((int)dis[i][0] == x1_p)&&((int)dis[i][1] == y1_p))
			    	{
						END = i;
			    		printf("ori END is %d \n", END);
						*endpoint=END+1;
						*beginpoint = 0;
						//printf("END POINT IS %d\n",*endpoint);
						//break;
			    	}
			    }
	    	}
	    	else{
	    	    x2_p = *pos_x;
	    	    y2_p = *pos_y;
	    	    for(int i = 0;i<=45;i++){
	    	    	if (((int)dis[i][0] == x2_p)&&((int)dis[i][1] == y2_p))
	    	    	{
	    	    		START = i;
	    	    		printf("ori START is %d \n", START);
	    	    		*beginpoint=START+1;
	    	    		//printf("START POINT IS %d\n",*beginpoint);
	    	    		//break;
	    	    	}
	    	    }

	    	    int i;    //计数来遍�?
	    	    printf("go here");
	    	    pOpen_list pO = (pOpen_list)malloc(sizeof(Open_list));
	    	     pO->next = NULL;
	    	     //pO = NULL;  千万不可以，NULL不可以访问的，除非pO赋��?
	    	     pOpen_list pC = (pOpen_list)malloc(sizeof(Open_list));
	    	     pC->next = NULL;

	    	     //创建起始节点并初始化，创建一个目标节�?
	    	     pOpen_list start = (pOpen_list)malloc(sizeof(Open_list));
	    	     start->next = NULL;
	    	 	pOpen_list end = (pOpen_list)malloc(sizeof(Open_list));
	    	 	end->next = NULL;

	    	     start->node.pFather = NULL;
	    	     start->node.N = START;
	    	     start->node.x = dis[START][0];
	    	     start->node.y = dis[START][1];
	    	     start->node.G = 0;
	    	     start->node.H = (my_abs(dis[START][0] - dis[END][0]) + my_abs(dis[START][0] - dis[END][0]))/SPEED;
	    	     start->node.F = start->node.G + start->node.H;

	    	 	end->node.pFather = NULL;
	    	     end->node.N = END;
	    	     end->node.x = dis[END][0];
	    	     end->node.y = dis[END][1];

	    	     //起始节点加入到open list
	    	     list_add_tail(pO, start);
	    	    //************************************************************************//
	    		int cir = 1;
	    	    while(cir)
	    	    {
	    	        //printf("-------------------begin while--------------\n");
	    			//寻找朢�小的F值节点，记为pCurrent（第丢�次循环也就是起点�?
	    	        //pOpen_list pCurrent = (pOpen_list)malloc(sizeof(Open_list));
	    			//msg_open_list(pO);	//现在open list中有的节�?
	    			pOpen_list pCurrent = find_min_f(pO);  //有可能找不到

	    	        //*********************************************************************//

	    			//把当前点从open list中移除（通过坐标），加入到close list，记为p
	    			pOpen_list p = list_delete_point(pO, pCurrent->node.N);
	    			list_add_tail(pC, p);
	    			//printf("core is (%d, %d)\n", p->node.x, p->node.y);

	    			//printf("now ,the open list is as follow\n");
	    			//msg_open_list(pC);
	    	        //ptr(pC, path1, path2);
	    			* path1 = (unsigned int)0b00000000000000000000000000000000;
	    			* path2 = (unsigned int)0b00000000000000000000000000000000;
	    			//ptr(pC,path1,path2);

	    			//printf("------------------------begin for  for------------------\n");

	    	        //*********************************************************************//

	    	        //还要考虑是不是障碍物，有没有在close list或��open list,考虑是不是边�?
	    	        //这块应该是遍历当前节�?p->node.x, p->node.y)的四周，并且都加入open list
	    	        for (i = 0; i <= MAP_LENTH - 1; i++){
	    	            if (i == p->node.N)
	    	                continue;
	    	            if (map[p->node.N][i] == X)
	    	                continue;
	    	            if (!(judge_node_exist(pC, i))){
	    	                continue;
	    	            }
	    	            if (i == END){
	    	                end->node.pFather = p;
	    	                cir = 0;
	    	                break;
	    	            }
	    	            if(judge_node_exist(pO, i)){
	    	                pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
	    	                pTmp->next =NULL;

	    	                pTmp->node.pFather = p;  //父节点为当前节点

	    	                pTmp->node.N = i;
	    	                pTmp->node.x = dis[i][0];
	    	                pTmp->node.y = dis[i][1];
	    	                //节点G.H.F�?      G值��么算好(找到父节点和现在节点的坐标了)
	    	                pTmp->node.G = p->node.F + map[p->node.N][i];   //初始节点到其实际距离
	    	                pTmp->node.H = (my_abs(end->node.x - pTmp->node.x) + my_abs(end->node.y - pTmp->node.y))/SPEED;
	    	                pTmp->node.F = pTmp->node.G + pTmp->node.H;

	    	                //加入到open list
	    	                list_add_tail(pO, pTmp);
	    	                //msg_open_list(pO);

	    	            }
	    	            else{    //在open list中了
	    	            //printf("---------------------in open list-------------------\n");
	    	            //printf("(%d) in open list\n", p->node.N);
	    	            //首先根据坐标找到他的指针
	    	            //pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
	    	            //pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
	    	            pOpen_list pTmp = list_browse(pO, i);
	    	            //定义好两个的G�?丢�个是原本的G值，丢�个是通过当前节点到的G�?
	    	            //扢�以用核心节点的G值加上当前节点到核心节点的��?
	    	            float currentG = p->node.F + map[p->node.N][i];
	    	            float pastG = pTmp->node.G;

	    	            //printf("currentG: %f      pastG: %f\n", currentG, pastG);
	    	            if(currentG < pastG)    //当前更优
	    	            {
	    	                pTmp->node.pFather = p; //更换父节�?
	    	                //注意，更改F值和G值一�?
	    	                pTmp->node.G = currentG;
	    	                pTmp->node.F = pTmp->node.G + pTmp->node.H;
	    	            }
	    	            }
	    				if(cir == 0)	//跳出外层的for循环
	    					break;
	    	        }

	    	    }
	    	    printf_father_node(end,  path1,  path2, map, distancehigh, distancelow);

	    	    bool = 0;
	    	}
	    }




		usb_ctl_val = UsbRead(ctl_reg);

		if(!(usb_ctl_val & no_device))
		{
			//USB hot plug routine
			for(hot_plug_count = 0 ; hot_plug_count < 7 ; hot_plug_count++)
			{
				usleep(5*1000);
				usb_ctl_val = UsbRead(ctl_reg);
				if(usb_ctl_val & no_device) break;
			}
			if(!(usb_ctl_val & no_device))
			{
				printf("\n[INFO]: the keyboard has been removed!!! \n");
				printf("[INFO]: please insert again!!! \n");
			}
		}

		while (!(usb_ctl_val & no_device))
		{

			usb_ctl_val = UsbRead(ctl_reg);
			usleep(5*1000);
			usb_ctl_val = UsbRead(ctl_reg);
			usleep(5*1000);
			usb_ctl_val = UsbRead(ctl_reg);
			usleep(5*1000);

			if(usb_ctl_val & no_device)
				goto USB_HOT_PLUG;

			usleep(200);
		}

	}//end while

	return 0;
}
	else{
		usleep(10000);
	    int x1_p;
	    int y1_p;
	    int x2_p;
	    int y2_p;
	    int bool = 0;
	    while(1){
		 if((*pos_x != 0)||(*pos_y != 0)){
			    	printf("X position is %d\n",*pos_x);
					printf("Y position is %d\n",*pos_y);
			    	if(bool == 0){
			    		* path1 = (unsigned int)0b00000000000000000000000000000000;
			    		* path2 = (unsigned int)0b00000000000000000000000000000000;
						* distancehigh = 0;
						* distancelow = 0;
					    x1_p = *pos_x;
					    y1_p = *pos_y;
					    bool = 1;
					    usleep(100000);
					    for(int i = 0;i<=45;i++){
					    	if (((int)dis[i][0] == x1_p)&&((int)dis[i][1] == y1_p))
					    	{
								END = i;
					    		printf("ori END is %d \n", END);
								*endpoint=END+1;
								*beginpoint = 0;
								//printf("END POINT IS %d\n",*endpoint);
								//break;
					    	}
					    }
			    	}
			    	else{
			    	    x2_p = *pos_x;
			    	    y2_p = *pos_y;
			    	    for(int i = 0;i<=45;i++){
			    	    	if (((int)dis[i][0] == x2_p)&&((int)dis[i][1] == y2_p))
			    	    	{
			    	    		START = i;
			    	    		printf("ori START is %d \n", START);
			    	    		*beginpoint=START+1;
			    	    		//printf("START POINT IS %d\n",*beginpoint);
			    	    		//break;
			    	    	}
			    	    }

			    	    int i;    //计数来遍�?
			    	    printf("go here");
			    	    pOpen_list pO = (pOpen_list)malloc(sizeof(Open_list));
			    	     pO->next = NULL;
			    	     //pO = NULL;  千万不可以，NULL不可以访问的，除非pO赋��?
			    	     pOpen_list pC = (pOpen_list)malloc(sizeof(Open_list));
			    	     pC->next = NULL;

			    	     //创建起始节点并初始化，创建一个目标节�?
			    	     pOpen_list start = (pOpen_list)malloc(sizeof(Open_list));
			    	     start->next = NULL;
			    	 	pOpen_list end = (pOpen_list)malloc(sizeof(Open_list));
			    	 	end->next = NULL;

			    	     start->node.pFather = NULL;
			    	     start->node.N = START;
			    	     start->node.x = dis[START][0];
			    	     start->node.y = dis[START][1];
			    	     start->node.G = 0;
			    	     start->node.H = (my_abs(dis[START][0] - dis[END][0]) + my_abs(dis[START][0] - dis[END][0]))/SPEED;
			    	     start->node.F = start->node.G + start->node.H;

			    	 	end->node.pFather = NULL;
			    	     end->node.N = END;
			    	     end->node.x = dis[END][0];
			    	     end->node.y = dis[END][1];

			    	     //起始节点加入到open list
			    	     list_add_tail(pO, start);
			    	    //************************************************************************//
			    		int cir = 1;
			    	    while(cir)
			    	    {
			    	        //printf("-------------------begin while--------------\n");
			    			//寻找朢�小的F值节点，记为pCurrent（第丢�次循环也就是起点�?
			    	        //pOpen_list pCurrent = (pOpen_list)malloc(sizeof(Open_list));
			    			//msg_open_list(pO);	//现在open list中有的节�?
			    			pOpen_list pCurrent = find_min_f(pO);  //有可能找不到

			    	        //*********************************************************************//

			    			//把当前点从open list中移除（通过坐标），加入到close list，记为p
			    			pOpen_list p = list_delete_point(pO, pCurrent->node.N);
			    			list_add_tail(pC, p);
			    			//printf("core is (%d, %d)\n", p->node.x, p->node.y);

			    			//printf("now ,the open list is as follow\n");
			    			//msg_open_list(pC);
			    	        //ptr(pC, path1, path2);
			    			* path1 = (unsigned int)0b00000000000000000000000000000000;
			    			* path2 = (unsigned int)0b00000000000000000000000000000000;
			    			//ptr(pC,path1,path2);

			    			//printf("------------------------begin for  for------------------\n");

			    	        //*********************************************************************//

			    	        //还要考虑是不是障碍物，有没有在close list或��open list,考虑是不是边�?
			    	        //这块应该是遍历当前节�?p->node.x, p->node.y)的四周，并且都加入open list
			    	        for (i = 0; i <= MAP_LENTH - 1; i++){
			    	            if (i == p->node.N)
			    	                continue;
			    	            if (map[p->node.N][i] == X)
			    	                continue;
			    	            if (!(judge_node_exist(pC, i))){
			    	                continue;
			    	            }
			    	            if (i == END){
			    	                end->node.pFather = p;
			    	                cir = 0;
			    	                break;
			    	            }
			    	            if(judge_node_exist(pO, i)){
			    	                pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
			    	                pTmp->next =NULL;

			    	                pTmp->node.pFather = p;  //父节点为当前节点

			    	                pTmp->node.N = i;
			    	                pTmp->node.x = dis[i][0];
			    	                pTmp->node.y = dis[i][1];
			    	                //节点G.H.F�?      G值��么算好(找到父节点和现在节点的坐标了)
			    	                pTmp->node.G = p->node.F + map[p->node.N][i];   //初始节点到其实际距离
			    	                pTmp->node.H = (my_abs(end->node.x - pTmp->node.x) + my_abs(end->node.y - pTmp->node.y))/SPEED;
			    	                pTmp->node.F = pTmp->node.G + pTmp->node.H;

			    	                //加入到open list
			    	                list_add_tail(pO, pTmp);
			    	                //msg_open_list(pO);

			    	            }
			    	            else{    //在open list中了
			    	            //printf("---------------------in open list-------------------\n");
			    	            //printf("(%d) in open list\n", p->node.N);
			    	            //首先根据坐标找到他的指针
			    	            //pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
			    	            //pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
			    	            pOpen_list pTmp = list_browse(pO, i);
			    	            //定义好两个的G�?丢�个是原本的G值，丢�个是通过当前节点到的G�?
			    	            //扢�以用核心节点的G值加上当前节点到核心节点的��?
			    	            float currentG = p->node.F + map[p->node.N][i];
			    	            float pastG = pTmp->node.G;

			    	            //printf("currentG: %f      pastG: %f\n", currentG, pastG);
			    	            if(currentG < pastG)    //当前更优
			    	            {
			    	                pTmp->node.pFather = p; //更换父节�?
			    	                //注意，更改F值和G值一�?
			    	                pTmp->node.G = currentG;
			    	                pTmp->node.F = pTmp->node.G + pTmp->node.H;
			    	            }
			    	            }
			    				if(cir == 0)	//跳出外层的for循环
			    					break;
			    	        }

			    	    }
			    	    printf_father_node(end,  path1,  path2, map, distancehigh, distancelow);

			    	    bool = 0;
			    	}
	}

}
	}
}

//找到链表中最小的f值的函数,输入链表，返回最小f的节�?
pOpen_list find_min_f(pOpen_list my_list){
//定义丢�个临时变量tmpf为第二个节点的F值，挨个比下�?
int tmpf = my_list->next->node.F;
pOpen_list tmpp = my_list->next;

while(my_list->next != NULL)
{

    if(tmpf > my_list->next->node.F)
    {
        tmpf = my_list->next->node.F;
        tmpp = my_list->next;       //用一个循环就可以找到，注意！！！
    }
    my_list = my_list->next;
}

//找到了F值即为tmpf，��么找到对应的节点，为什么不跟着定义丢�个临时节点呢
return tmpp;
}



void list_add_tail(pOpen_list my_list, pOpen_list add_node)
{
pOpen_list tmp = my_list;
while (tmp->next != NULL)
{
	tmp = tmp->next;
}
tmp->next = add_node;
add_node->next = NULL;
}

//删除链表中某个节点，通过坐标删除,并且返回这个删除的节点指针，方便free
pOpen_list list_delete_point(pOpen_list my_list, int N)
{

while (my_list->next != NULL)
{
    if(my_list->next->node.N == N) //找到删除节点
    {
        pOpen_list tmp = my_list->next;
		//这个节点不是朢�后一个节�?
		if(my_list->next->next != NULL)
		{
			my_list->next = my_list->next->next;

			tmp->next = NULL;
		}
		else //朢�后一个节�?
		{
			my_list->next = NULL;

			tmp->next = NULL;
		}
        return tmp;
    }
    my_list = my_list->next;
}

return NULL;
}

float my_abs(float x)
{
if(x < 0)
{
    return (float)(-x);
}
else
{
   return (float)(x);
}
}

int judge_node_exist(pOpen_list mylist, int N)
{
while(mylist->next != NULL)
{
    if(mylist->next->node.N == N)	//在open list
    {
        return 0;
    }
    mylist = mylist->next;
}

return 1;
}

pOpen_list list_browse(pOpen_list my_list, int N)
{

while (my_list->next != NULL)
{
	if(my_list->next->node.N == N)
    {
        return my_list->next;
    }
    my_list = my_list->next;
}

return NULL;
}

//打印open list中的各个节点坐标以及F�?
void msg_open_list(pOpen_list my_list)
{
while(my_list->next != NULL)
{
    int n = my_list->next->node.N;

    float f = my_list->next->node.F;
    printf("is (%d).   F = %f\n", n, f);

    my_list = my_list->next;
}
}

void printf_father_node(pOpen_list my_list, int *p1, int *p2, int map[46][46], int *dis1, int *dis2)
{
	*p1 = 0b00000000000000000000000000000000;
	*p2 = 0b00000000000000000000000000000000;
	//int temp1 = 0b00000000000000000000000000000000;
	//int temp2 = 0b00000000000000000000000000000000;
	int distance = 0;
while(my_list->node.pFather != NULL)
{
	//printf("is(%d)\n", my_list->node.pFather->node.N);
	int n = my_list->node.pFather->node.N;
	int cur = my_list->node.N;
	int line = find_line(cur, n);
	distance += map[my_list->node.N][my_list->node.pFather->node.N];
	//printf("Line is %d \n",line);
	if(line <= 31){
		*p1 += (unsigned int)(pow(2,line)+0.01);
		//printf("p1 now is %d\n",*p1);
	}
	else{
		*p2 += (unsigned int)(pow(2,line-32)+0.01);
	}

	usleep(500000);
	my_list = my_list->node.pFather;
}
distance = (int) distance/60;
*dis1 = (int) distance/10;
*dis2 = distance%10;
//*p1 = temp1;
//*p2 = temp2;
}

void ptr(pOpen_list my_list, int *p1, int *p2)
{   *p1 = 0b00000000000000000000000000000000;
	*p2 = 0b00000000000000000000000000000000;
	int temp1 = 0b00000000000000000000000000000000;
	int temp2 = 0b00000000000000000000000000000000;
while(my_list->next != NULL)
{
    int n = my_list->next->node.N;
    int cur = my_list->node.N;
    int line = find_line(cur, n);
    printf("line number is %d \n",line);
	if(line <= 31){
		temp1 += (int)(pow(2,line)+0.01);
		//printf("The temp1 is %d",temp1);
		//printf("%d",*p1);
	}
	else{
		temp2 += (int)(pow(2,line-32)+0.01);
		//printf("%d",*p2);
	}
    my_list = my_list->next;
}
printf("In this round, path 1 is %d path 2 is %d \n",temp1, temp2);
*p1 = temp1;
*p2 = temp2;
}

int find_line(int p1, int p2){
int data[128][3] = {{0, 1, 0},
		{1, 0, 0},
		{1, 2, 1},
		{2, 1, 1},
		{2, 3, 2},
		{3, 2, 2},
		{3, 4, 3},
		{4, 3, 3},
		{4, 45, 4},
		{45, 4, 4},
		{5, 45, 5},
		{45, 5, 5},
		{5, 6, 6},
		{6, 5, 6},
		{5, 12, 7},
		{12, 5, 7},
		{7, 12, 8},
		{12, 7, 8},
		{7, 8, 9},
		{8, 7, 9},
		{8, 9, 10},
		{9, 8, 10},
		{9, 10, 11},
		{10, 9, 11},
		{10, 11, 12},
		{11, 10, 12},
		{11, 16, 13},
		{16, 11, 13},
		{11, 14, 14},
		{14, 11, 14},
		{16, 17, 15},
		{17, 16, 15},
		{15, 16, 16},
		{16, 15, 16},
		{15, 25, 17},
		{25, 15, 17},
		{15, 44, 18},
		{44, 15, 18},
		{18, 44, 19},
		{44, 18, 19},
		{19, 44, 20},
		{44, 19, 20},
		{19, 43, 21},
		{43, 19, 21},
		{25, 43, 22},
		{43, 25, 22},
		{14, 25, 23},
		{25, 14, 23},
		{8, 14, 24},
		{14, 8, 24},
		{13, 14, 25},
		{14, 13, 25},
		{14, 24, 26},
		{24, 14, 26},
		{24, 25, 27},
		{25, 24, 27},
		{25, 28, 28},
		{28, 25, 28},
		{13, 24, 29},
		{24, 13, 29},
		{8, 13, 30},
		{13, 8, 30},
		{12, 13, 31},
		{13, 12, 31},
		{5, 13, 32},
		{13, 5, 32},
		{13, 22, 33},
		{22, 13, 33},
		{22, 45, 34},
		{45, 22, 34},
		{22, 23, 35},
		{23, 22, 35},
		{3, 21, 36},
		{21, 3, 36},
		{0, 21, 37},
		{21, 0, 37},
		{20, 24, 38},
		{24, 20, 38},
		{29, 43, 39},
		{43, 29, 39},
		{19, 30, 40},
		{30, 19, 40},
		{29, 30, 41},
		{30, 29, 41},
		{28, 29, 42},
		{29, 28, 42},
		{27, 30, 43},
		{30, 27, 43},
		{27, 28, 44},
		{28, 27, 44},
		{20, 28, 45},
		{28, 20, 45},
		{20, 42, 46},
		{42, 20, 46},
		{20, 23, 47},
		{23, 20, 47},
		{23, 26, 48},
		{26, 23, 48},
		{21, 26, 49},
		{26, 21, 49},
		{26, 39, 50},
		{39, 26, 50},
		{39, 42, 51},
		{42, 39, 51},
		{31, 39, 52},
		{39, 31, 52},
		{38, 39, 53},
		{39, 38, 53},
		{31, 32, 54},
		{32, 31, 54},
		{37, 38, 55},
		{38, 37, 55},
		{36, 37, 56},
		{37, 36, 56},
		{35, 36, 57},
		{36, 35, 57},
		{33, 35, 58},
		{35, 33, 58},
		{34, 35, 59},
		{35, 34, 59},
		{27, 42, 60},
		{42, 27, 60},
		{8, 12, 61},
		{12, 8, 61},
		{3, 22, 62},
		{22, 3, 62},
		{40, 41, 63},
		{41, 40, 63}};
int linenum;

for(int i = 0;i <= 127;i++){
    if(p1 == data[i][0] && p2 == data[i][1]){
        linenum = data[i][2];
    }
}
return linenum;
}

int strtoint(char *a){
    int m = -1;
    if (strcmp(a,"HB")==0){m = 0;}
    if (strcmp(a,"CC")==0){m = 1;}
    if (strcmp(a,"SY")==0){m = 2;}
    if (strcmp(a,"BJ")==0){m = 3;}
    if (strcmp(a,"TJ")==0){m = 4;}
    if (strcmp(a,"JN")==0){m = 5;}
    if (strcmp(a,"QD")==0){m = 6;}
    if (strcmp(a,"SH")==0){m = 7;}
    if (strcmp(a,"HZ")==0){m = 8;}
    if (strcmp(a,"NB")==0){m = 9;}
    if (strcmp(a,"FZ")==0){m = 10;}
    if (strcmp(a,"XM")==0){m = 11;}
    if (strcmp(a,"NJ")==0){m = 12;}
    if (strcmp(a,"HF")==0){m = 13;}
    if (strcmp(a,"NC")==0){m = 14;}
    if (strcmp(a,"GZ")==0){m = 15;}
    if (strcmp(a,"SZ")==0){m = 16;}
    if (strcmp(a,"XG")==0){m = 17;}
    if (strcmp(a,"HK")==0){m = 18;}
    if (strcmp(a,"NN")==0){m = 19;}
    if (strcmp(a,"XA")==0){m = 20;}
    if (strcmp(a,"HHHT")==0){m = 21;}
    if (strcmp(a,"SJZ")==0){m = 22;}
    if (strcmp(a,"TY")==0){m = 23;}
    if (strcmp(a,"WH")==0){m = 24;}
    if (strcmp(a,"CS")==0){m = 25;}
    if (strcmp(a,"YC")==0){m = 26;}
    if (strcmp(a,"CD")==0){m = 27;}
    if (strcmp(a,"CQ")==0){m = 28;}
    if (strcmp(a,"GY")==0){m = 29;}
    if (strcmp(a,"KM")==0){m = 30;}
    if (strcmp(a,"XN")==0){m = 31;}
    if (strcmp(a,"LS")==0){m = 32;}
    if (strcmp(a,"WLMQ")==0){m = 33;}
    if (strcmp(a,"KS")==0){m = 34;}
    if (strcmp(a,"TLF")==0){m = 35;}
    if (strcmp(a,"HM")==0){m = 36;}
    if (strcmp(a,"DH")==0){m = 37;}
    if (strcmp(a,"JYG")==0){m = 38;}
    if (strcmp(a,"LZ")==0){m = 39;}
    if (strcmp(a,"TB")==0){m = 40;}
    if (strcmp(a,"GX")==0){m = 41;}
    if (strcmp(a,"BJ")==0){m = 42;}
    if (strcmp(a,"LZ")==0){m = 43;}
    if (strcmp(a,"HC")==0){m = 44;}
    if (strcmp(a,"DZ")==0){m = 45;}
    return m;
}

///*---------------------------------------------------------------------------
//  --      main.c                                                    	   --
//  --      Christine Chen                                                   --
//  --      Ref. DE2-115 Demonstrations by Terasic Technologies Inc.         --
//  --      Fall 2014                                                        --
//  --                                                                       --
//  --      For use with ECE 298 Experiment 7                                --
//  --      UIUC ECE Department                                              --
//  ---------------------------------------------------------------------------*/
//
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <io.h>
//#include <fcntl.h>
//
//#include "system.h"
//#include "alt_types.h"
//#include <unistd.h>  // usleep
//#include "sys/alt_irq.h"
//#include "io_handler.h"
//
//#include "cy7c67200.h"
//#include "usb.h"
//#include "lcp_cmd.h"
//#include "lcp_data.h"
//
//volatile unsigned int * Ptr = (unsigned int*) 0x00000040; //modified
//----------------------------------------------------------------------------------------//
//
//                                Main function
//
//----------------------------------------------------------------------------------------//
//int main(void)
//{
//	IO_init();
//
//	/*while(1)
//	{
//		IO_write(HPI_MAILBOX,COMM_EXEC_INT);
//		printf("[ERROR]:routine mailbox data is %x\n",IO_read(HPI_MAILBOX));
//		//UsbWrite(0xc008,0x000f);
//		//UsbRead(0xc008);
//		usleep(10*10000);
//	}*/
//
//	alt_u16 intStat;
//	alt_u16 usb_ctl_val;
//	static alt_u16 ctl_reg = 0;
//	static alt_u16 no_device = 0;
//	alt_u16 fs_device = 0;
//	int keycode = 0;
//	alt_u8 toggle = 0;
//	alt_u8 data_size;
//	alt_u8 hot_plug_count;
//	alt_u16 code;
//
//	volatile unsigned int * path1 = (unsigned int*) 0x000000c0;
//	volatile unsigned int * path2 = (unsigned int*) 0x00000010;
//	* path1 = 0b00100100000000000000000000000000;
//	* path2 = 0b00000000000000000000000000000000;
////	* path1 = 0b11111111111111111111111111111111;
////	* path2 = 0b11111111111111111111111111111111;
//	printf("USB keyboard setup...\n\n");
//
//	//----------------------------------------SIE1 initial---------------------------------------------------//
//	USB_HOT_PLUG:
//	UsbSoftReset();
//
//	// STEP 1a:
//	UsbWrite (HPI_SIE1_MSG_ADR, 0);
//	UsbWrite (HOST1_STAT_REG, 0xFFFF);
//
//	/* Set HUSB_pEOT time */
//	UsbWrite(HUSB_pEOT, 600); // adjust the according to your USB device speed
//
//	usb_ctl_val = SOFEOP1_TO_CPU_EN | RESUME1_TO_HPI_EN;// | SOFEOP1_TO_HPI_EN;
//	UsbWrite(HPI_IRQ_ROUTING_REG, usb_ctl_val);
//
//	intStat = A_CHG_IRQ_EN | SOF_EOP_IRQ_EN ;
//	UsbWrite(HOST1_IRQ_EN_REG, intStat);
//	// STEP 1a end
//
//	// STEP 1b begin
//	UsbWrite(COMM_R0,0x0000);//reset time
//	UsbWrite(COMM_R1,0x0000);  //port number
//	UsbWrite(COMM_R2,0x0000);  //r1
//	UsbWrite(COMM_R3,0x0000);  //r1
//	UsbWrite(COMM_R4,0x0000);  //r1
//	UsbWrite(COMM_R5,0x0000);  //r1
//	UsbWrite(COMM_R6,0x0000);  //r1
//	UsbWrite(COMM_R7,0x0000);  //r1
//	UsbWrite(COMM_R8,0x0000);  //r1
//	UsbWrite(COMM_R9,0x0000);  //r1
//	UsbWrite(COMM_R10,0x0000);  //r1
//	UsbWrite(COMM_R11,0x0000);  //r1
//	UsbWrite(COMM_R12,0x0000);  //r1
//	UsbWrite(COMM_R13,0x0000);  //r1
//	UsbWrite(COMM_INT_NUM,HUSB_SIE1_INIT_INT); //HUSB_SIE1_INIT_INT
//	IO_write(HPI_MAILBOX,COMM_EXEC_INT);
//
//	while (!(IO_read(HPI_STATUS) & 0xFFFF) )  //read sie1 msg register
//	{
//	}
//	while (IO_read(HPI_MAILBOX) != COMM_ACK)
//	{
//		printf("[ERROR]:routine mailbox data is %x\n",IO_read(HPI_MAILBOX));
//		goto USB_HOT_PLUG;
//	}
//	// STEP 1b end
//
//	printf("STEP 1 Complete");
//	// STEP 2 begin
//	UsbWrite(COMM_INT_NUM,HUSB_RESET_INT); //husb reset
//	UsbWrite(COMM_R0,0x003c);//reset time
//	UsbWrite(COMM_R1,0x0000);  //port number
//	UsbWrite(COMM_R2,0x0000);  //r1
//	UsbWrite(COMM_R3,0x0000);  //r1
//	UsbWrite(COMM_R4,0x0000);  //r1
//	UsbWrite(COMM_R5,0x0000);  //r1
//	UsbWrite(COMM_R6,0x0000);  //r1
//	UsbWrite(COMM_R7,0x0000);  //r1
//	UsbWrite(COMM_R8,0x0000);  //r1
//	UsbWrite(COMM_R9,0x0000);  //r1
//	UsbWrite(COMM_R10,0x0000);  //r1
//	UsbWrite(COMM_R11,0x0000);  //r1
//	UsbWrite(COMM_R12,0x0000);  //r1
//	UsbWrite(COMM_R13,0x0000);  //r1
//
//	IO_write(HPI_MAILBOX,COMM_EXEC_INT);
//
//	while (IO_read(HPI_MAILBOX) != COMM_ACK)
//	{
//		printf("[ERROR]:routine mailbox data is %x\n",IO_read(HPI_MAILBOX));
//		goto USB_HOT_PLUG;
//	}
//	// STEP 2 end
//
//	ctl_reg = USB1_CTL_REG;
//	no_device = (A_DP_STAT | A_DM_STAT);
//	fs_device = A_DP_STAT;
//	usb_ctl_val = UsbRead(ctl_reg);
//
//	if (!(usb_ctl_val & no_device))
//	{
//		for(hot_plug_count = 0 ; hot_plug_count < 5 ; hot_plug_count++)
//		{
//			usleep(5*1000);
//			usb_ctl_val = UsbRead(ctl_reg);
//			if(usb_ctl_val & no_device) break;
//		}
//		if(!(usb_ctl_val & no_device))
//		{
//			printf("\n[INFO]: no device is present in SIE1!\n");
//			printf("[INFO]: please insert a USB keyboard in SIE1!\n");
//			while (!(usb_ctl_val & no_device))
//			{
//				usb_ctl_val = UsbRead(ctl_reg);
//				if(usb_ctl_val & no_device)
//					goto USB_HOT_PLUG;
//
//				usleep(2000);
//			}
//		}
//	}
//	else
//	{
//		/* check for low speed or full speed by reading D+ and D- lines */
//		if (usb_ctl_val & fs_device)
//		{
//			printf("[INFO]: full speed device\n");
//		}
//		else
//		{
//			printf("[INFO]: low speed device\n");
//		}
//	}
//
//
//
//	// STEP 3 begin
//	//------------------------------------------------------set address -----------------------------------------------------------------
//	UsbSetAddress();
//
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		UsbSetAddress();
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506); // i
//	printf("[ENUM PROCESS]:step 3 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508); // n
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]:step 3 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03) // retries occurred
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//
//		goto USB_HOT_PLUG;
//	}
//
//	printf("------------[ENUM PROCESS]:set address done!---------------\n");
//
//	// STEP 4 begin
//	//-------------------------------get device descriptor-1 -----------------------------------//
//	// TASK: Call the appropriate function for this step.
//	UsbGetDeviceDesc1(); 	// Get Device Descriptor -1
//
//	//usleep(10*1000);
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		// TASK: Call the appropriate function again if it wasn't processed successfully.
//		UsbGetDeviceDesc1();
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506);
//	printf("[ENUM PROCESS]:step 4 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508);
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]:step 4 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03)
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//
//		printf("Encountered Fatal Error");
//		goto USB_HOT_PLUG;
//	}
//
//	printf("---------------[ENUM PROCESS]:get device descriptor-1 done!-----------------\n");
//
//
//	//--------------------------------get device descriptor-2---------------------------------------------//
//	//get device descriptor
//	// TASK: Call the appropriate function for this step.
//	UsbGetDeviceDesc2(); 	// Get Device Descriptor -2
//
//	//if no message
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		//resend the get device descriptor
//		//get device descriptor
//		// TASK: Call the appropriate function again if it wasn't processed successfully.
//		UsbGetDeviceDesc2();
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506);
//	printf("[ENUM PROCESS]:step 4 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508);
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]:step 4 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03)
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//
//		printf("Encountered Fatal Error");
//		goto USB_HOT_PLUG;
//	}
//
//	printf("------------[ENUM PROCESS]:get device descriptor-2 done!--------------\n");
//
//
//	// STEP 5 begin
//	//-----------------------------------get configuration descriptor -1 ----------------------------------//
//	// TASK: Call the appropriate function for this step.
//	UsbGetConfigDesc1(); 	// Get Configuration Descriptor -1
//
//	//if no message
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		//resend the get device descriptor
//		//get device descriptor
//
//		// TASK: Call the appropriate function again if it wasn't processed successfully.
//		UsbGetConfigDesc1();
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506);
//	printf("[ENUM PROCESS]:step 5 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508);
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]:step 5 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03)
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//
//		printf("Encountered Fatal Error");
//		goto USB_HOT_PLUG;
//	}
//	printf("------------[ENUM PROCESS]:get configuration descriptor-1 pass------------\n");
//
//	// STEP 6 begin
//	//-----------------------------------get configuration descriptor-2------------------------------------//
//	//get device descriptor
//	// TASK: Call the appropriate function for this step.
//	UsbGetConfigDesc2(); 	// Get Configuration Descriptor -2
//
//	usleep(100*1000);
//	//if no message
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		// TASK: Call the appropriate function again if it wasn't processed successfully.
//		UsbGetConfigDesc2();
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506);
//	printf("[ENUM PROCESS]:step 6 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508);
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]:step 6 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03)
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//
//		printf("Encountered Fatal Error");
//		goto USB_HOT_PLUG;
//	}
//
//
//	printf("-----------[ENUM PROCESS]:get configuration descriptor-2 done!------------\n");
//
//
//	// ---------------------------------get device info---------------------------------------------//
//
//	// TASK: Write the address to read from the memory for byte 7 of the interface descriptor to HPI_ADDR.
//	IO_write(HPI_ADDR,0x056c);
//	code = IO_read(HPI_DATA);
//	code = code & 0x003;
//	printf("\ncode = %x\n", code);
//
//	if (code == 0x01)
//	{
//		printf("\n[INFO]:check TD rec data7 \n[INFO]:Keyboard Detected!!!\n\n");
//	}
//	else
//	{
//		printf("\n[INFO]:Keyboard Not Detected!!! \n\n");
//	}
//
//	// TASK: Write the address to read from the memory for the endpoint descriptor to HPI_ADDR.
//
//	IO_write(HPI_ADDR,0x0576);
//	IO_write(HPI_DATA,0x073F);
//	IO_write(HPI_DATA,0x8105);
//	IO_write(HPI_DATA,0x0003);
//	IO_write(HPI_DATA,0x0008);
//	IO_write(HPI_DATA,0xAC0A);
//	UsbWrite(HUSB_SIE1_pCurrentTDPtr,0x0576); //HUSB_SIE1_pCurrentTDPtr
//
//	//data_size = (IO_read(HPI_DATA)>>8)&0x0ff;
//	//data_size = 0x08;//(IO_read(HPI_DATA))&0x0ff;
//	//UsbPrintMem();
//	IO_write(HPI_ADDR,0x057c);
//	data_size = (IO_read(HPI_DATA))&0x0ff;
//	printf("[ENUM PROCESS]:data packet size is %d\n",data_size);
//	// STEP 7 begin
//	//------------------------------------set configuration -----------------------------------------//
//	// TASK: Call the appropriate function for this step.
//	UsbSetConfig();		// Set Configuration
//
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		// TASK: Call the appropriate function again if it wasn't processed successfully.
//		UsbSetConfig();		// Set Configuration
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506);
//	printf("[ENUM PROCESS]:step 7 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508);
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]:step 7 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03)
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//
//		printf("Encountered Fatal Error");
//		goto USB_HOT_PLUG;
//	}
//
//	printf("------------[ENUM PROCESS]:set configuration done!-------------------\n");
//
//	//----------------------------------------------class request out ------------------------------------------//
//	// TASK: Call the appropriate function for this step.
//	UsbClassRequest();
//
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		// TASK: Call the appropriate function again if it wasn't processed successfully.
//		UsbClassRequest();
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506);
//	printf("[ENUM PROCESS]:step 8 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508);
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]:step 8 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03)
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//
//		printf("Encountered Fatal Error");
//		goto USB_HOT_PLUG;
//	}
//
//
//	printf("------------[ENUM PROCESS]:class request out done!-------------------\n");
//
//	// STEP 8 begin
//	//----------------------------------get descriptor(class 0x21 = HID) request out --------------------------------//
//	// TASK: Call the appropriate function for this step.
//	UsbGetHidDesc();
//
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		// TASK: Call the appropriate function again if it wasn't processed successfully.
//		UsbGetHidDesc();
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506);
//	printf("[ENUM PROCESS]:step 8 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508);
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]:step 8 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03)
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//		printf("Encountered Fatal Error");
//		goto USB_HOT_PLUG;
//	}
//
//	printf("------------[ENUM PROCESS]:get descriptor (class 0x21) done!-------------------\n");
//
//	// STEP 9 begin
//	//-------------------------------get descriptor (class 0x22 = report)-------------------------------------------//
//	// TASK: Call the appropriate function for this step.
//	UsbGetReportDesc();
//	//if no message
//	while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//	{
//		// TASK: Call the appropriate function again if it wasn't processed successfully.
//		UsbGetReportDesc();
//		usleep(10*1000);
//	}
//
//	UsbWaitTDListDone();
//
//	IO_write(HPI_ADDR,0x0506);
//	printf("[ENUM PROCESS]: step 9 TD Status Byte is %x\n",IO_read(HPI_DATA));
//
//	IO_write(HPI_ADDR,0x0508);
//	usb_ctl_val = IO_read(HPI_DATA);
//	printf("[ENUM PROCESS]: step 9 TD Control Byte is %x\n",usb_ctl_val);
//	while (usb_ctl_val != 0x03)
//	{
//		usb_ctl_val = UsbGetRetryCnt();
//		printf("Encountered Fatal Error");
//		goto USB_HOT_PLUG;
//	}
//
//	printf("---------------[ENUM PROCESS]:get descriptor (class 0x22) done!----------------\n");
//
//
//
//	//-----------------------------------get keycode value------------------------------------------------//
//	usleep(10000);
//	while(1)
//	{
//		toggle++;
//		IO_write(HPI_ADDR,0x0500); //the start address
//		//data phase IN-1
//		IO_write(HPI_DATA,0x051c); //500
//
//		IO_write(HPI_DATA,0x000f & data_size);//2 data length
//
//		IO_write(HPI_DATA,0x0291);//4 //endpoint 1
//		if(toggle%2)
//		{
//			IO_write(HPI_DATA,0x0001);//6 //data 1
//		}
//		else
//		{
//			IO_write(HPI_DATA,0x0041);//6 //data 1
//		}
//		IO_write(HPI_DATA,0x0013);//8
//		IO_write(HPI_DATA,0x0000);//a
//		UsbWrite(HUSB_SIE1_pCurrentTDPtr,0x0500); //HUSB_SIE1_pCurrentTDPtr
//
//		while (!(IO_read(HPI_STATUS) & HPI_STATUS_SIE1msg_FLAG) )  //read sie1 msg register
//		{
//			IO_write(HPI_ADDR,0x0500); //the start address
//			//data phase IN-1
//			IO_write(HPI_DATA,0x051c); //500
//
//			IO_write(HPI_DATA,0x000f & data_size);//2 data length
//
//			IO_write(HPI_DATA,0x0291);//4 //endpoint 1
//			if(toggle%2)
//			{
//				IO_write(HPI_DATA,0x0001);//6 //data 1
//			}
//			else
//			{
//				IO_write(HPI_DATA,0x0041);//6 //data 1
//			}
//			IO_write(HPI_DATA,0x0013);//8
//			IO_write(HPI_DATA,0x0000);//
//			UsbWrite(HUSB_SIE1_pCurrentTDPtr,0x0500); //HUSB_SIE1_pCurrentTDPtr
//			usleep(10*1000);
//		}//end while
//
//		usb_ctl_val = UsbWaitTDListDone();
//
//		// The first two keycodes are stored in 0x051E. Other keycodes are in
//		// subsequent addresses.
//		keycode = UsbRead(0x051e);
//		printf("\nfirst two keycode values are %04x\n",keycode);
//		// We only need the first keycode, which is at the lower byte of keycode.
//		// Send the keycode to hardware via PIO.
//		*keycode_base = keycode & 0xff;
//
//		usleep(200);//usleep(5000);
//		usb_ctl_val = UsbRead(ctl_reg);
//
//		if(!(usb_ctl_val & no_device))
//		{
//			//USB hot plug routine
//			for(hot_plug_count = 0 ; hot_plug_count < 7 ; hot_plug_count++)
//			{
//				usleep(5*1000);
//				usb_ctl_val = UsbRead(ctl_reg);
//				if(usb_ctl_val & no_device) break;
//			}
//			if(!(usb_ctl_val & no_device))
//			{
//				printf("\n[INFO]: the keyboard has been removed!!! \n");
//				printf("[INFO]: please insert again!!! \n");
//			}
//		}
//
//		while (!(usb_ctl_val & no_device))
//		{
//
//			usb_ctl_val = UsbRead(ctl_reg);
//			usleep(5*1000);
//			usb_ctl_val = UsbRead(ctl_reg);
//			usleep(5*1000);
//			usb_ctl_val = UsbRead(ctl_reg);
//			usleep(5*1000);
//
//			if(usb_ctl_val & no_device)
//				goto USB_HOT_PLUG;
//
//			usleep(200);
//		}
//
//	}//end while
//
//	return 0;
//}


