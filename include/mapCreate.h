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
#include <map>

using namespace std;

typedef struct{
    int  nPrice;
    char nType[20];
    int  nCapacity;
    int  nChargePerTime;
} uavWeight_t;

class MAP_CREATE
{
public:
    MAP_CREATE();
    MAP_CREATE(MAP_INFO *pstMap);

    int get_mappoint (int x,int y, int z){
        return map[z][y][x];
    }

    int getMapXsize(void){
        return map_x;
    }

    int getMapYsize(void){
        return map_y;
    }

    int getMapZsize(void){
        return map_z;
    }

    int getMinFlyHeight(void){
        return flyMinHeight;
    }

    int getMaxFlyHeight(void){
        return flyMaxHeight;
    }

    pair<int,int> getUavWeHome(void){
        return uavWeHone;
    }

    int getMinPlaneWeight(void){
        return mUavLoadWeight[0];
    }

    int getMaxPlaneWeight(void){
        return mUavLoadWeight[mnUavPriceNum-1];
    }

    int getPlaneWeight(int num){
    if(num < mnUavPriceNum)
        return mUavLoadWeight[num];
    else return -1;
    }
    int getPlaneTypeNum(void)
    {
        return mnUavPriceNum;
    }
    UAV_PRICE getPlaneUavPrice(int weight)
    {
        UAV_PRICE uavPriceTemp = {-1};

        it = m_uavPrice.find(weight);
        if(it != m_uavPrice.end())
            return m_uavPrice[weight];//返回

        return uavPriceTemp;
    }

private:
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
    
    std::map<int , UAV_PRICE> m_uavPrice;//保存重量对应的UAC_PRICE结构值
    std::map<int , UAV_PRICE>::iterator it;
    //
  

  

};

#endif
