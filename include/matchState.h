#ifndef MATCHSTATE_H
#define MATCHSTATE_H

#include "include/CmdParse.h"
#include <map>
using namespace std;



class MATCHSTATE
{
public:
    MATCHSTATE();
    void renewMatchstate(MATCH_STATUS * pstMatch);
    UAV pickWeUavFromID(int weUavId);
    UAV pickEnemyUavFromID(int enemyUavId);
    GOODS pickGoodsFromID(int GoodsId);
    void findUavEnemyHome(void);

    inline int  getCurrentTime()
    {
        return mpstMatch->nTime;
    }
    inline int getWeValue()
    {
        return mpstMatch->nWeValue;
    }
    inline int getEnemyValue()
    {
        return mpstMatch->nWeValue;
    }
    inline pair<int,int> getUavEnemyHome()
    {
        return uavEnemyHome;
    }
	
private:
	MATCH_STATUS * mpstMatch;	
	map<int,UAV> weUav;
	map<int,UAV> enemyUav;
	map<int,GOODS> Goods;
	int isJustStart;
	pair<int,int> uavEnemyHome;



};



#endif
