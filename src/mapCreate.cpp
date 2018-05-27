/**	@file		map.cpp
 *	@note		gongchengdaxue.
 *	@brief		build the map
 *
 *	@author		huangjingwei
 *	@date		2018/5/24
 *	@note		历史记录：
 *	@note		V1.0.0  
 *	@warning	
 */
#include "include/mapCreate.h"

using namespace std;

MAP::MAP()
{
  
}

MAP::MAP(MAP_INFO *pstMap):mpstMap(pstMap)
{
    map_x=mpstMap->nMapX;
    map_y=mpstMap->nMapY;
    map_z=mpstMap->nMapZ;  
    
    flyMaxHeight =  mpstMap->nHHigh;
    flyMinHeight =  mpstMap->nHLow;

    map.resize(map_z);  
    for (int i = 0; i < map_z; ++i) {  
        map[i].resize(map_y);  
  
        for (int j = 0; j < map_y; ++j)  
            map[i][j].resize(map_x);  
    }  

    for (int z=0;z<map_z;z++)
    {
      for(int y=0;y<map_y;y++)
      {
        for(int x=0;x<map_x;x++)
        {
            map[z][y][x]=0;
        }
      }
    }
}

  
  
void MAP::map_build()
{
    //选出建筑的最大高度
   // int highest=0;
   // for(int build_num=0;build_num<mpstMap->nBuildingNum;build_num++)
   // if(mpstMap->astBuilding[build_num].nH>highest)
   //     highest=mpstMap->astBuilding[build_num].nH;
    //在0～highest之间存在建筑
    for(int z=0;z<=flyMaxHeight;z++)
    {
      for(int build_num=0;build_num<mpstMap->nBuildingNum;build_num++)
      {
        if(z>mpstMap->astBuilding[build_num].nH)
            continue;
        for(int x=mpstMap->astBuilding[build_num].nX;x<mpstMap->astBuilding[build_num].nX+mpstMap->astBuilding[build_num].nL;x++)
        {
            for(int y=mpstMap->astBuilding[build_num].nY;y<mpstMap->astBuilding[build_num].nY+mpstMap->astBuilding[build_num].nW;y++)
            {
                map[z][y][x]=1;//有墙是1
            }
        }
      }
    }
}

int MAP::get_mappoint (int x,int y, int z)
{
    return map[z][y][x];
}
  
int MAP::getMapXsize(void)
{
   return map_x;
}

int MAP::getMapYsize(void)
{
   return map_y;
}

int MAP::getMapZsize(void)
{
    return map_z;
}

int MAP::getMinFlyHeight(void)
{
    return flyMinHeight;
}  

int MAP::getMaxFlyHeight(void)
{
    return flyMaxHeight;
}  


