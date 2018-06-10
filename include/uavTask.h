#ifndef __UAVTASK_H_
#define __UAVTASK_H_

#include "CmdParse.h"
#include "matchState.h"
#include "mapCreate.h"
#include <map>
#include <math.h>
#include <queue>
#include <assert.h>
#include <cstring>

#include "search.h"

using namespace std;

enum{
    UAV_TASK_IDEL=1,//空闲状态下，涉及到充电，地址分配，任务分配
    UAV_TASK_GOODS,
    UAV_TASK_TRACK,
};

enum{
    UAV_STATE_CHARGE=1,//空闲状态下，涉及到充电，地址分配，任务分配
    UAV_STATE_BACK_CHARGE,
    UAV_STATE_RAND,
    UAV_STATE_TRACK,
    UAV_GOOD_TO_GET,
    UAV_GOOD_TO_PUT,

    UAV_HAS_RAND_ROAD,
    UAV_NO_RAND_ROAD,
};

enum{
    UAV_NOT_IN_QUEUE=1,
    UAV_IN_OUT_QUEUE,
    UAV_IN_BACK_QUEUE,

};

typedef struct{
    int x;
    int y;
    int z;
}uavCoord_t;

typedef struct {
    int taskState;//
    int taskClass;
    int enemyNo;
    int goodsNo;
    int uavHomeStatus;
    uavCoord_t nowLocation;
    uavCoord_t nextLocation;
    uavCoord_t goalLocation;
    int taskInIdelTime;


}uavTask_t;


//typedef pair<int, UAV> PAIR_UAV;



class UAV_TASK{

  public:

    UAV_TASK(MAP_CREATE *m_map_create, FLAY_PLANE* pstFlayPlane, pathSearch* pPathSearch );
    uavTask_t getUavTaskWithID(int uavID);
    void setUavTaskWithID(int uavID, uavTask_t uavTask);
    void clearUavTaskWithID(int uavID);
    int isUavTaskAvailbal(int uavID);
    void findEnemyHome(MATCH_STATUS *pstMatch);


    void updateUavStatus(MATCH_STATUS * pstMatch);
    void uavTaskProcess(MATCH_STATUS * pstMatch);
  private:
    MAP_CREATE *m_mapCreate;
    FLAY_PLANE *m_pstFlayPlane;
    pathSearch* m_PathSearch;

    void uavRun(int uavID, UAV uavStatus);
    void uavTaskInGoods(int uavID, UAV uavStatus);
    void uavTaskAssignGoods(int uavID, UAV uavStatus);
    void uavTaskAssign(int uavID, UAV uavStatus);
    void uavTaskAllot(int uavID, UAV uavStatus);
    void uavTaskInIDEL(int uavID, UAV uavStatus);
    void uavChargeProcess(int uavID, UAV uavStatus);

    void uavTaskTrackEnemy(int uavID, UAV uavStatus);

    void uavPurchase(void);

    map<int, UAV> m_weUavID;
    map<int, int> m_uavPlanID;
    map<int, UAV> m_weUav;

    map<int, UAV> m_enemyUavID;
    map<int, int> m_uavTrackID;

    map<int, GOODS> m_Goods;
    map<int, int>   m_uavGoodsID;

    map<int , uavTask_t> m_uavTask;//link the uav ID with uav Task

    map<int , uavCoord_t> m_weUavNextPiont;
    map<int , uavCoord_t> m_weUavNowPiont;

    map<int , uavCoord_t> m_enemyUavNowPiont;

    map<float, int> goodsCost;

    vector<UAV> uavOutHomeQueue;
    vector<UAV> uavBackHomeQueue;

    vector<pair<int, int>> weUavObstaclePos;


    int m_weMoney;
    int m_enemyMoney;

    int m_enemyUavNum;
    int m_weUavNum;
    UAV_PRICE m_cheapestUavPrice;

    int m_runTime;

    int weHomeX;
    int weHomeY;

    int mapXsize;
    int mapYsize;
    int mapZsize;

    int minFlyHeight;
    int MaxFlyHeight;

    int enemyUavHomeX;
    int enemyUavHomeY;

    int isJustStart;
    #define random(x) (rand()%x)

    int isUavInHome(int uavX, int uavY)
    {
        if((uavX == weHomeX) && (uavY == weHomeY))
        {
            return 1;
        }
        return 0;
    };

    int isUavInEnemyHome(int uavX, int uavY)
    {
        if((uavX == enemyUavHomeX) && (uavY == enemyUavHomeY))
        {
            return 1;
        }
        return 0;
    };

};


#endif

