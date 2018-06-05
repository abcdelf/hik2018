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
using namespace std;

class MAP
{
public:
    MAP();
    MAP(MAP_INFO *pstMap);

    inline  int get_mappoint (int x,int y, int z){
        return map[z][y][x];
    }

    inline int getMapXsize(void){
        return map_x;
    }

    inline int getMapYsize(void){
        return map_y;
    }

    inline int getMapZsize(void){
        return map_z;
    }

    inline int getMinFlyHeight(void){
        return flyMinHeight;
    }

    inline int getMaxFlyHeight(void){
        return flyMaxHeight;
    }

    inline pair<int,int> getUavWeHome(void){
        return uavWeHone;
    }

    inline int getMinPlaneWeight(void){
        return mUavLoadWeight[0];
    }

    inline int getMaxPlaneWeight(void){
        return mUavLoadWeight[mnUavPriceNum-1];
    }

    inline int getPlaneWeight(int num){
    if(num < mnUavPriceNum)
        return mUavLoadWeight[num];
    else return -1;
    }

    inline int getPlanePrice(int num){
    if(num < mnUavPriceNum)
        return mUavPrice[num];
    else return -1;
    }

    inline char*  getPlaneSort(int num){
    if(num < mnUavPriceNum)
        return mPurchaseType[num];
    else return  nullptr;
    }

public:
    MAP_INFO *mpstMap;

private:
  
    int flyMaxHeight;
    int flyMinHeight;

    int map_x;
    int map_y;
    int map_z;

    pair<int,int>  uavWeHone;
    std::vector<std::vector<std::vector<unsigned char> > > map;

    int mnUavPriceNum;
    int mUavLoadWeight[MAX_UAV_PRICE_NUM];
    int mUavPrice[MAX_UAV_PRICE_NUM];
    char mPurchaseType[MAX_UAV_PRICE_NUM][8];

  

  

};

#endif
