#include "uavPlan.h"
#include "iostream"

//contriction function
UAVPLAN::UAVPLAN()
{
  
}

/**
 * @brief 设置飞行计划
 */
 void UAVPLAN::setUavPlan(int uav_id,DESTINATION gold_address,UAV_STATUS n_status,int goods_no,vector<Node> path)
 {
    SUAVPLAN uavPlanTem;
    uavPlanTem.nNO=uav_id;
    uavPlanTem.goalAddress.X=gold_address.X;
    uavPlanTem.goalAddress.Y=gold_address.Y;
    uavPlanTem.goalAddress.Z=gold_address.Z;
    uavPlanTem.nStatus=n_status;
    uavPlanTem.nGoodsNo=goods_no;
    uavPlanTem.path.assign(path.begin(),path.end());

    //mUavPlan.insert(pair<int,SUAVPLAN>(uav_id,uavPlanTem));//insert操作：键值相同时前面覆盖后面
    mUavPlan[uav_id]=uavPlanTem;//下标操作：键值相同时后面覆盖前面
}


/**
 * @brief 获取uav_id的飞机的飞行计划
 */
SUAVPLAN UAVPLAN::getUavPlan(int uav_id)
{
    map<int,SUAVPLAN>::iterator it;
    it=mUavPlan.find(uav_id);
    if(it==mUavPlan.end())
    {
        cout<<"goodsId not found"<<endl;
    }
    else
    {
        cout<<"nNO "<<it->second.nNO<<" found"<<endl;
        return it->second;
    }
}

