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
 
MATCHSTATUS::MATCHSTATUS(MAP* mmap,MAP_INFO *pstMap):mymap(mmap),mpstMap(pstMap)
{
  map<int, int>::iterator it;

  mhlow = pstMap->nHLow;

  minLoadWeight = mymap->getPlaneWeight(0);

  for(int i=0;i<MAX_UAV_NUM;i++)
  {
    plane_goods.insert(make_pair(i, -1));

  }
}

void MATCHSTATUS::JudWauvSta(int plane_num,int goods_no)//good_num=-1表示没有给无人机安排取货物认为
{
  int goods_num;
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
    if(mpstMatch->astWeUav[i].nStatus)   //无人机坠毁
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
    plane_goods[mpstMatch->astWeUav[i].nNO]=mpstMatch->astGoods[best_goosnum].nNO;
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
    int enemyId[100]={0};

    for(int i=0; i<mpstMatch->nUavEnemyNum; i++)
    {
      int temp = mpstMatch->astEnemyUav[i].nLoadWeight;
      if(temp == 100)
      {
        enemyId[0]=i;

      }else if(temp == 50)
      {
        enemyId[1]=i;
      }
    }
    for(int j=0, i=0; i<mpstMatch->nUavWeNum; i++)
    {
      int temp = mpstMatch->astWeUav[i].nLoadWeight;
      if(temp==20)
      {
        enemyIdNum[i] = j;//enemyId[j];
        j++;
      }
    }

}
int MATCHSTATUS::which_enemy(int plane_num)
{
  int enemyIdTemp=0;

  if(plane_num<1000)
  {
    if(enemyIdNum[plane_num]< mpstMatch->nEnemyValue)
      enemyIdTemp = enemyIdNum[plane_num];
  }
   
  return enemyIdTemp;
}


