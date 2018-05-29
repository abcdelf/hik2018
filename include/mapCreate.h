/**	@file		map.h
 *	@note		gongchengdaxue.
 *	@brief		build the map
 *
 *	@author		huangjingwei
 *	@date		2018/5/24
 *	@note		历史记录：
 *	@note		V1.0.0  
 *	@warning	
 */
#ifndef MAP_H
#define MAP_H

#include "include/CmdParse.h"
#include <vector>


class MAP
{
public:
  MAP();
  MAP(MAP_INFO *pstMap);
  void map_build();
  int get_mappoint (int x,int y, int z);
  int getMapXsize(void);
  int getMapYsize(void);
  int getMapZsize(void);
  int getMinFlyHeight(void);
  int getMaxFlyHeight(void);
  int getMinPlaneWeight(void)
  {
    return planeWeight[0];
  }
  int getMaxPlaneWeight(void)
  {
    return planeWeight[planeWeightNum-1];
  }
  int getPlaneWeight(int num)
  {
    if(num < planeWeightNum)
      return planeWeight[num];
    else return -1;
  }
  int getPlanePrice(int num)
  {
    if(num < planeWeightNum)
      return planePrice[num];
    else return -1;
  }

public:
  MAP_INFO *mpstMap;

private:
  int flyMaxHeight;
  int flyMinHeight;
  int map_x;
  int map_y;
  int map_z;
  std::vector<std::vector<std::vector<unsigned char> > > map;
  int planeWeight[100];
  int planeWeightNum;
  int planeNum[100];
  int planePrice[100];

  

};

#endif