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

    planeWeightNum = mpstMap->nUavPriceNum;

    for(int i=0;i< planeWeightNum;i++)
    {
        planeWeight[i] = mpstMap->astUavPrice[i].nLoadWeight;

        printf("i=%d;value=%5d;weight=%5d\n",i,mpstMap->astUavPrice[i].nValue,mpstMap->astUavPrice[i].nLoadWeight);
    }

    int n = planeWeightNum;
    for (int i = 0; i<n - 1; i++) 
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            //如果前面的数比后面大，进行交换
            if (planeWeight[j] > planeWeight[j + 1]) {
                int temp = planeWeight[j]; 
                planeWeight[j] = planeWeight[j + 1]; 
                planeWeight[j + 1] = temp;
            }
        }
    }  

    for(int i=0;i<n;i++)
    {
        printf("weight=%d\n",planeWeight[i]);
    }

    for(int i=0;i< mpstMap->nUavPriceNum; i++)                                              
    {
        for(int j=0;j<mpstMap->nUavPriceNum; j++)
        {
            if(mpstMap->astUavPrice[j].nLoadWeight == getPlaneWeight(i)) 
            {
                planePrice[i] = mpstMap->astUavPrice[j].nValue;
            }  
        }  
    }
    for(int i=0;i<n;i++)
    {
        printf("planePrice=%d\n",planePrice[i]);
    }

    
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


