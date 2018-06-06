#ifndef __UAVTASK_H_
#define __UAVTASK_H_

#include "CmdParse.h"
#include "matchState.h"
#include "mapCreate.h"
#include <map>
using namespace std;

enum{
    UAV_TASK_IDEL=1,//空闲状态下，涉及到充电，地址分配，任务分配
    UAV_TASK_GOODS,
    UAV_TASK_TRACK,
};

enum{
    UAV_STATE_CHARGE=1,//空闲状态下，涉及到充电，地址分配，任务分配
    UAV_STATE_RAND,
    UAV_STATE_TRACK,
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
    uavCoord_t nowLocation;
    uavCoord_t nextLocation;



}uavTask_t;

class UAV_TASK{

  public:

    UAV_TASK(MAP_CREATE *m_map_create, FLAY_PLANE* pstFlayPlane);
    uavTask_t getUavTaskWithID(int uavID);
    void setUavTaskWithID(int uavID, uavTask_t uavTask);
    void clearUavTaskWithID(int uavID);
    int isUavTaskAvailbal(int uavID);



    void updateUavStatus(MATCH_STATUS * pstMatch);
    void uavTaskProcess(MATCH_STATUS * pstMatch);
  private:
    MAP_CREATE *m_mapCreate;
    FLAY_PLANE *m_pstFlayPlane;

    void uavTaskAllot(int uavID, UAV uavStatus);
    void uavTaskInIDEL(int uavID, UAV uavStatus, uavTask_t uavTask);
    void uavChargeProcess(int uavID, UAV uavStatus);



    map<int, UAV> m_weUavID;
    map<int, int> m_uavPlanID;
    map<int, UAV> m_weUav;
    map<int, UAV> enemyUav;
    map<int, GOODS> Goods;
    
    map<int , uavTask_t> m_uavTask;//link the uav ID with uav Task

    int weHomeX;
    int weHomeY;


    int isUavInHome(int uavX, int uavY)
    {
        if(uavX == weHomeX && uavY == weHomeY)
        {
            return 1;
        }
        return -1;
    };

};


#endif

