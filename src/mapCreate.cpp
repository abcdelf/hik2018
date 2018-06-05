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
#include <string.h>


using namespace std;

MAP::MAP()
{
  
}

MAP::MAP(MAP_INFO *pstMap):mpstMap(pstMap)
{
	//地图大小
	map_x=mpstMap->nMapX;
	map_y=mpstMap->nMapY;
	map_z=mpstMap->nMapZ;  
    
	//飞行的高度限制
	flyMaxHeight =  mpstMap->nHHigh;
	flyMinHeight =  mpstMap->nHLow;
    
	//我方停机坪位置
	uavWeHone=make_pair(mpstMap->nParkingX,mpstMap->nParkingY);
       
	//可购买的飞机的种类数
	mnUavPriceNum = mpstMap->nUavPriceNum;

	for(int i=0;i< mnUavPriceNum;i++)
	{
		mUavLoadWeight[i] = mpstMap->astUavPrice[i].nLoadWeight;
	}
	
	//对可购买的飞机的种类按载重从小到达排列
	int n = mnUavPriceNum;
	for (int i = 0; i<n - 1; i++) 
	{
		for (int j = 0; j < n - i - 1; j++)
		{
			//如果前面的数比后面大，进行交换
			if (mUavLoadWeight[j] > mUavLoadWeight[j + 1]) {
			int temp = mUavLoadWeight[j]; 
			mUavLoadWeight[j] = mUavLoadWeight[j + 1]; 
			mUavLoadWeight[j + 1] = temp;
			}
		}
	}  
       
        //用顺序载重更新顺序价值
	for(int i=0;i< mpstMap->nUavPriceNum; i++)                                              
	{
		for(int j=0;j<mpstMap->nUavPriceNum; j++)
		{
			if(mpstMap->astUavPrice[j].nLoadWeight == mUavLoadWeight[i]) 
			{
				mUavPrice[i] = mpstMap->astUavPrice[j].nValue;
				break;
			}  
		}  
	}
	
	//用顺序载重更新顺序飞机名称
	for(int i=0;i< mpstMap->nUavPriceNum; i++)                                              
	{
		for(int j=0;j<mpstMap->nUavPriceNum; j++)
		{
			if(mpstMap->astUavPrice[j].nLoadWeight == mUavLoadWeight[i]) 
			{
				strcpy(mPurchaseType[i],mpstMap->astUavPrice[j].szType);
				break;
			}  
		}  
	}

	//初始化地图的每个点为0
	map.resize(map_z);  
	for (int i = 0; i < map_z; ++i) {  
		map[i].resize(map_y);  
		for (int j = 0; j < map_y; ++j)  
			map[i][j].resize(map_x);  
	}  
	for (int z=0;z<map_z;z++){
		for(int y=0;y<map_y;y++) {
			for(int x=0;x<map_x;x++){
				map[z][y][x]=0;
			}
		}
	 }
	 for(int z=0;z<=flyMaxHeight;z++)
	{
		for(int fog_num=0;fog_num<mpstMap->nFogNum;fog_num++)
		{
			if(z<mpstMap->astFog[fog_num].nB)
				continue;
			if(z>mpstMap->astFog[fog_num].nT)
				continue;
			for(int x=mpstMap->astFog[fog_num].nX;x<mpstMap->astFog[fog_num].nX+mpstMap->astFog[fog_num].nL;x++)
			{
				for(int y=mpstMap->astFog[fog_num].nY;y<mpstMap->astFog[fog_num].nY+mpstMap->astFog[fog_num].nW;y++)
				{
					map[z][y][x]=2;//雾是2
				}
			}
		 }
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





