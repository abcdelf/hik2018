/**	@file		matchstatus.h
 *	@note		gongchengdaxue.
 *	@brief		get the match statsu
 *
 *	@author		huangjingwei
 *	@date		2018/5/24
 *	@note		历史记录：
 *	@note		V1.0.0  
 *	@warning	
 */
#ifndef MATCHSTATUS_H
#define MATCHSTATUS_H

#include "include/CmdParse.h"
#include "include/mapCreate.h"
#include <vector>
#include <map>

using namespace std;

class MATCHSTATUS
{
public:
  MATCHSTATUS();
  MATCHSTATUS(MAP_CREATE* mmap,MAP_INFO *pstMap);
  void JudWauvSta(int plane_num,int goods_no);//good_num=-1表示没有给无人机安排取货物认为
  void set_newmatch(MATCH_STATUS * pstMatch);
  void auv_goods();
  int which_goods(int plane_num);
  int which_enemy(int plane_num);
  void search_enemy(void);
  void wePlaneHomeInit(void);
  int getHomeX(void)
  {
    return mapStartX;
  };
  int getHomeY(void)
  {
    return mapStartY;
  };
  void clearPlaneGoodsNum(int planeNum)
  {
    if((mpstMatch->astWeUav[planeNum].nGoodsNo ==-1)&&(mauvstate[planeNum] == SEARCH))
      plane_goods[mpstMatch->astWeUav[planeNum].nNO]=-1;
  }
public:
  enum auvstate
  {
    BACK_SEARCH=0,//向上运动到最低限制高度
    SEARCH=1,     //向货物起始点搜索路径
    TO_GET=2,     //向下去获取货物
    BACK_TRANS=3, //获取到货物并向上运动到最低限飞高度
    TRANS=4,      //向货物目标点搜索路径
    TO_PUT=5,     //到达货物目标点并向下去放置货物
    TO_EnemyUav=6,//追踪敌方无人机
    PLANE_INIT=7, //初始化刚出生的无人机
  };
  vector<int> mauvstate;
  map<int,int> plane_goods;    //auv_no,good_no
  MATCH_STATUS * mpstMatch;
  MAP_INFO *mpstMap;
  MAP_CREATE *mymap;

private:
  int mhlow;
  int minLoadWeight;
  


  int enemyNum;
  int enemyIdNum[1000];

  int isJustStart;

  int mapStartX;
  int mapStartY;
};



#endif