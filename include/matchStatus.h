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
#include <vector>
#include <map>

using namespace std;

class MATCHSTATUS
{
public:
  MATCHSTATUS();
  MATCHSTATUS(MAP_INFO *pstMap);
  void JudWauvSta(int plane_num,int goods_no);//good_num=-1表示没有给无人机安排取货物认为
  void set_newmatch(MATCH_STATUS * pstMatch);
  void auv_goods();
  int which_goods(int plane_num);

public:
  enum auvstate
  {
    BACK_SEARCH=0,
    SEARCH=1,
    TO_GET=2,
    BACK_TRANS=3,
    TRANS=4,
    TO_PUT=5
  };
  vector<int> mauvstate;
  map<int,int> plane_goods;    //auv_no,good_no
  MATCH_STATUS * mpstMatch;
  MAP_INFO *mpstMap;
  private:
  int mhlow;
};



#endif