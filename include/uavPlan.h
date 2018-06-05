#ifndef UAVPLAN_H
#define UAVPLAN_H
#include "CmdParse.h"
#include "Node.h"
#include <vector>
using namespace std;


typedef struct  _DESTINATION_
{
    int X;
    int Y;
    int Z;
}DESTINATION;

typedef struct _SUAVPLAN_
{
    int     nNO;
    DESTINATION goalAddress;
    UAV_STATUS  nStatus;
    int     nGoodsNo;
    vector<Node> path;
}SUAVPLAN;


class UAVPLAN
{
public:
    UAVPLAN();
    void setUavPlan(int uav_id,DESTINATION gold_address,UAV_STATUS n_status,int goods_no,vector<Node> path);
    SUAVPLAN getUavPlan(int uav_id);
private:
    map<int,SUAVPLAN> mUavPlan;
};


#endif
