#ifndef MATCHSTATE_H
#define MATCHSTATE_H

#include "CmdParse.h"
#include <map>
#include <iostream>

using namespace std;



class MATCHSTATE
{
public:
    MATCHSTATE();
    void renewMatchstate(MATCH_STATUS * pstMatch);
    UAV pickWeUavFromID(int weUavId);
    UAV pickWeUavFromNum(int num);
    UAV pickEnemyUavFromID(int enemyUavId);
    GOODS pickGoodsFromID(int GoodsId);
    void findUavEnemyHome(void);

    int  getCurrentTime()
    {
        return mpstMatch->nTime;
    }
    int getWeValue()
    {
        return mpstMatch->nWeValue;
    }
    int getEnemyValue()
    {
        return mpstMatch->nWeValue;
    }
    pair<int,int> getUavEnemyHome()
    {
        return uavEnemyHome;
    }
	int getWeUavNum(void)
    {
        return m_weUav.size();
    }
private:
	MATCH_STATUS * mpstMatch;	
	map<int,UAV> m_weUavID;
    map<int,UAV> m_weUav;
	map<int,UAV> enemyUav;
	map<int,GOODS> Goods;
	int isJustStart;
	pair<int,int> uavEnemyHome;




};



#endif
