/**	@file		uavset.h
 *	@note		gongchengdaxue.
 *	@brief		set the uavstep
 *
 *	@author		huangjingwei
 *	@date		2018/5/24
 *	@note		历史记录：
 *	@note		V1.0.0  
 *	@warning	
 */
#ifndef UAVSET_H
#define UAVSET_H

#include "include/CmdParse.h"
#include <vector>
#include "include/SquareGraph.h"
#include "include/mapCreate.h"
#include "include/search.h"

using namespace std;

class PLANE
{
public:
  PLANE();
  PLANE(MAP* map,FLAY_PLANE* pstFlayPlane, pathSearch* roadSearch);
  void plane_up(int plane_num,int high,int flag);  //flag:0=back_search,flag=1:back_trans
  void plane_get(int plane_num,int high,int goods_no);
  pair<int, int> plane_search(int plane_num,int goods_no, vector<pair<int, int>> obstaclePos);
  pair<int, int> plane_tran(int plane_num,int goods_no, vector<pair<int, int>> obstaclePos);
  void plane_put(int plane_num,int high);
  void set_newmatch(MATCH_STATUS * pstMatch);
  void printgoodnum(int goo_num);
  FLAY_PLANE* renew();


public:

  vector<int> PlanSetate;
  MAP* mmap;
  FLAY_PLANE* mpstFlayPlane;
  MATCH_STATUS* mpstMatch;
  pathSearch* mpathSearch;
  

  
};

#endif