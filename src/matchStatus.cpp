/**	@file		matchstatus.cpp
 *	@note		gongchengdaxue.
 *	@brief		get the match statsu
 *
 *	@author		huangjingwei
 *	@date		2018/5/24
 *	@note		历史记录：
 *	@note		V1.0.0  
 *	@warning	
 */
#include "include/matchStatus.h"

#include <iostream>

MATCHSTATUS::MATCHSTATUS()
{
 }
 
MATCHSTATUS::MATCHSTATUS(MAP* mmap,MAP_INFO *pstMap):\
                          mymap(mmap),mpstMap(pstMap)
{
  map<int, int>::iterator it;

  mhlow = pstMap->nHLow;

  minLoadWeight = mymap->getPlaneWeight(0);

  isJustStart = -1;//第一次运行，还没有初始化本起飞位置
  mapStartX = -1;
  mapStartY = -1;

  for(int i=0;i<MAX_UAV_NUM;i++)
  {
    plane_goods.insert(make_pair(i, -1));

  }
}

void MATCHSTATUS::wePlaneHomeInit(void)//isJustStart = 0;
{
  int initState=0;
  if(isJustStart == -1)
  {
    if(mpstMatch->nUavWeNum >=2 )
    {
      if( (mpstMatch->astWeUav[0].nX== mpstMatch->astWeUav[1].nX)&&\
          (mpstMatch->astWeUav[0].nY== mpstMatch->astWeUav[1].nY)&&\
          (mpstMatch->astWeUav[0].nZ== mpstMatch->astWeUav[1].nZ)&&\
          (UAV_CRASH != mpstMatch->astWeUav[0].nStatus) && (mpstMatch->astWeUav[1].nStatus!=UAV_CRASH))
      {
        initState=1;
      }
    }
    if(initState==1)
    {
      isJustStart = 0;
      mapStartX = mpstMatch->astWeUav[0].nX;
      mapStartY = mpstMatch->astWeUav[0].nY;
      cout<<"mapStartX="<<mapStartX<<" ; mapStartY="<<mapStartY<<endl;
    }
  }
}

void MATCHSTATUS::JudWauvSta(int plane_num,int goods_no)//good_num=-1表示没有给无人机安排取货物认为
{
  int goods_num;

  // if(mpstMatch->astWeUav[plane_num].nX == mapStartX&&\
  //    mpstMatch->astWeUav[plane_num].nY == mapStartY)
  // {
  //   if(mpstMatch->astWeUav[plane_num].nZ < mhlow)
  //   {
  //     cout<<"inHome"<<endl;
  //     mauvstate[plane_num]= PLANE_INIT;//飞机位置需要初始化
  //     return;
  //   }
  // }
  for(goods_num=0;goods_num< mpstMatch->nGoodsNum;goods_num++)
  {
    if(goods_no==mpstMatch->astGoods[goods_num].nNO)
      break;	  
  }
  if(mpstMatch->astWeUav[plane_num].nGoodsNo!=-1)//无人机载货
  {
    if(mpstMatch->astWeUav[plane_num].nZ>=mhlow)//无人机大于最低高度限制    
    {
      mauvstate[plane_num]=TRANS;//去搜索货物目标点
      if(mpstMatch->astWeUav[plane_num].nX==mpstMatch->astGoods[goods_num].nEndX&&mpstMatch->astWeUav[plane_num].nY==mpstMatch->astGoods[goods_num].nEndY)
      {
        mauvstate[plane_num]=TO_PUT;//到达货物目标点去放货	
        if(mpstMatch->astWeUav[plane_num].nZ==0)
          plane_goods[mpstMatch->astWeUav[plane_num].nNO]=-1;
      }
    }
    else
    {
      if(mpstMatch->astGoods[goods_num].nState==1&&mpstMatch->astWeUav[plane_num].nX==mpstMatch->astGoods[goods_num].nStartX&&mpstMatch->astWeUav[plane_num].nY==mpstMatch->astGoods[goods_num].nStartY)//nState表示被拾起
	      mauvstate[plane_num]=BACK_TRANS;//从货物起始点向上运动到最低限度
      if(mpstMatch->astGoods[goods_num].nState==1&&mpstMatch->astWeUav[plane_num].nX==mpstMatch->astGoods[goods_num].nEndX&&mpstMatch->astWeUav[plane_num].nY==mpstMatch->astGoods[goods_num].nEndY)//nState表示被拾起
      {
        mauvstate[plane_num]=TO_PUT;	//到达货物目标点去放货
        if(mpstMatch->astWeUav[plane_num].nZ==0)
          plane_goods[mpstMatch->astWeUav[plane_num].nNO]=-1;
      }
    }
  }
  else//无人机空载
  {
    if(mpstMatch->astWeUav[plane_num].nZ>=mhlow)  
    {

      if(mpstMatch->astWeUav[plane_num].nLoadWeight == minLoadWeight)//如果飞机是最低载重量的飞机，则作为攻击机器使用
      {
        mauvstate[plane_num] = TO_EnemyUav;//搜索攻击敌方无人机
      }else{
        mauvstate[plane_num] = SEARCH;//搜索货物目标点
      }
      
      if(mpstMatch->astWeUav[plane_num].nX==mpstMatch->astGoods[goods_num].nStartX&&mpstMatch->astWeUav[plane_num].nY==mpstMatch->astGoods[goods_num].nStartY)
      {
	      mauvstate[plane_num]=TO_GET;
      }   
    }
    else
    {
      if(mpstMatch->astGoods[goods_num].nState==0&&mpstMatch->astWeUav[plane_num].nX==mpstMatch->astGoods[goods_num].nStartX&&mpstMatch->astWeUav[plane_num].nY==mpstMatch->astGoods[goods_num].nStartY)//nState表示未被拾起
      {
	      mauvstate[plane_num]=TO_GET;
      }
      else
      	mauvstate[plane_num]=BACK_SEARCH;	

      if(mpstMatch->astWeUav[plane_num].nX != mapStartX&&\
         mpstMatch->astWeUav[plane_num].nY != mapStartY)//飞机不在坐标原点上
      {
        if(mpstMatch->astWeUav[plane_num].nLoadWeight == minLoadWeight)//如果飞机是最低载重量的飞机，则作为攻击机器使用
        {
          mauvstate[plane_num] = TO_EnemyUav;//搜索攻击敌方无人机
        }
      }


    }
  }
}

void MATCHSTATUS::set_newmatch(MATCH_STATUS * pstMatch)
{
  mpstMatch=pstMatch;
  for(int i=0;i<pstMatch->nUavWeNum;i++)
    mauvstate.push_back(-1);
};
  
void MATCHSTATUS::auv_goods()
{
  for(int i=0;i<mpstMatch->nUavWeNum;i++)
  {
    float  bestPercentWorth = 0;
    int best_dis=1000;
    int best_goosnum=0;
    if(mpstMatch->nGoodsNum==0)        //当没有货物时全部归0
    {
      plane_goods[mpstMatch->astWeUav[i].nNO]=-1;
      continue;
    }
    if(mpstMatch->astWeUav[i].nStatus==UAV_CRASH)   //无人机坠毁
    {
      plane_goods[mpstMatch->astWeUav[i].nNO]=-1;
      continue;
    } 
    if(mauvstate[i]!=SEARCH)                     //只在搜索状态时更新
      continue;
    if(mauvstate[i]==TO_EnemyUav)//如果是攻击机器，则不作为载货机器使用
      continue;

    for(int j=0;j< mpstMatch->nGoodsNum;j++)
    {
      
      if(!mpstMatch->astGoods[j].nState&&plane_goods[mpstMatch->astWeUav[i].nNO]!=-1)      //如果货物j不可被捡起，但是设置i飞机去捡起，就让i飞机放弃j货物
	      plane_goods[mpstMatch->astWeUav[i].nNO]=-1;

      int delect_flag=0;
      for(map<int,int>::iterator it=plane_goods.begin();it!=plane_goods.end();it++)          //如果货物已经分配给自己队伍飞机，就不再进行分配
      {
        if(it->first>mpstMatch->nUavWeNum)                  //plane_goods初始化时较大，提前结束
          break;
        if(it->second==mpstMatch->astGoods[j].nNO)
        {
          delect_flag=1;
          break;
        }
      }	   
      if(delect_flag)
	      continue;

      if(mpstMatch->astGoods[j].nState)      //status为0表示货物正常且可以被拾起
	      continue;
      if(mpstMatch->astWeUav[i].nLoadWeight< mpstMatch->astGoods[j].nWeight)
	      continue;
      int distance_get=abs((mpstMatch->astWeUav[i].nX-mpstMatch->astGoods[j].nStartX))+abs((mpstMatch->astWeUav[i].nY-mpstMatch->astGoods[j].nStartY))+mpstMatch->astWeUav[i].nZ;
      int distance_put=abs((mpstMatch->astGoods[j].nEndX-mpstMatch->astGoods[j].nStartX))+abs((mpstMatch->astGoods[j].nEndY-mpstMatch->astGoods[j].nStartY));
      int left_time=mpstMatch->astGoods[j].nRemainTime+mpstMatch->astGoods[j].nStartTime-mpstMatch->nTime;

      if(distance_get<left_time)
      {
        float goodsWorth      = mpstMatch->astGoods[j].nValue;
        //float goodsWeight     = mpstMatch->astGoods[j].nWeight;
        float distance        = distance_get+distance_put;
        float planeLoadWeight = mpstMatch->astWeUav[i].nLoadWeight;
        float percentWorth    = (float)(goodsWorth/distance/planeLoadWeight);
        //printf("percentWorth=%f\n",percentWorth);

        if(percentWorth > bestPercentWorth)
        {
          bestPercentWorth = percentWorth;
          best_goosnum=j;	  
        }

      } 
    }   

    if(bestPercentWorth==0&&best_goosnum==0)
      continue;
    plane_goods[mpstMatch->astWeUav[i].nNO] = mpstMatch->astGoods[best_goosnum].nNO;
    printf("%dplane,bestPercentWorth%f,best_goosnum:%d,\n",i,bestPercentWorth,best_goosnum);
  }
}
  
int MATCHSTATUS::which_goods(int plane_num)
{
  //printf(" plane_goods[mpstMatch->astWeUav[%d].nNO]:%d\n", mpstMatch->astWeUav[plane_num].nNO,plane_goods[mpstMatch->astWeUav[plane_num].nNO]);
  return plane_goods[mpstMatch->astWeUav[plane_num].nNO];
}
void MATCHSTATUS::search_enemy(void)
{

    int enemyWeight[100]={0};
    
    for(int i=0; i<mpstMatch->nUavEnemyNum; i++)
    {
       enemyWeight[i] = mpstMatch->astEnemyUav[i].nLoadWeight;
    }
    
    int n = mpstMatch->nUavEnemyNum;
    for (int i = 0; i<n - 1; i++) 
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            //如果前面的数比后面xiao，进行交换
          if (enemyWeight[j] < enemyWeight[j + 1]) {
              int temp = enemyWeight[j]; 
              enemyWeight[j] = enemyWeight[j + 1]; 
              enemyWeight[j + 1] = temp;
          }
        }
    }  
    // for(int i=0;i<n;i++)
    // {
    //     printf("enemyWeight=%d\n",enemyWeight[i]);
    // }

    enemyIdNum[0]=-1;
    int ourTracePlane=0;
    for(int ii=0;ii< mpstMatch->nUavWeNum; ii++)
    {
      if(mpstMatch->astWeUav[ii].nLoadWeight == mymap->getMinPlaneWeight())
      {
        ourTracePlane++;
      }
    }

    int j=0;
    if(mpstMatch->nUavEnemyNum >=2)
    {
      for(int i=0 ; i <mpstMatch->nUavEnemyNum; i++)
      {
        if(mpstMatch->astEnemyUav[i].nLoadWeight >= enemyWeight[ourTracePlane])
        {
          enemyIdNum[j]=i;
          j++;
        }
      }
    }else if(mpstMatch->nUavEnemyNum >=1){
      enemyIdNum[j] = 0;
      j++;
    }
    enemyIdNum[j] = -1;
    enemyNum = j;


}
int MATCHSTATUS::which_enemy(int plane_num)
{
//   cout<<"******************"<<endl;
//   cout<< "enemyNum="<<enemyNum <<"; planeNum="<<plane_num<<endl;
//   cout<< "enemyIdNum[plane_num]="<<enemyIdNum[plane_num] <<endl;
  if(plane_num < enemyNum)
  {
    if(enemyIdNum[plane_num] !=-1)
    {
      if(enemyIdNum[plane_num]< mpstMatch->nUavEnemyNum)
        return enemyIdNum[plane_num];
    }

  }else{

  }

  return 0;
}


