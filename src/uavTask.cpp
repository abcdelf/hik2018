#include "uavTask.h"


UAV_TASK::UAV_TASK(MAP_CREATE *m_map_create, FLAY_PLANE* pstFlayPlane):m_mapCreate(m_map_create),m_pstFlayPlane(pstFlayPlane)
{
	weHomeX = m_map_create->getUavWeHome().first;
	weHomeY = m_map_create->getUavWeHome().second;

    minFlyHeight = m_map_create->getMinFlyHeight();
    MaxFlyHeight = m_map_create->getMaxFlyHeight();
}


uavTask_t UAV_TASK::getUavTaskWithID(int uavID)
{
    return m_uavTask[uavID];
}

void UAV_TASK::setUavTaskWithID(int uavID, uavTask_t uavTask)
{
    m_uavTask.insert(pair<int, uavTask_t>(uavID,uavTask));
}

void UAV_TASK::clearUavTaskWithID(int uavID)
{
    map<int,uavTask_t>::iterator it;
	it=m_uavTask.find(uavID);	
	if(it==m_uavTask.end())
	{
		cout<<"Task uavID not found"<<endl;
	}
	else	  
	{
		m_uavTask.erase(it);
	}    
}

int UAV_TASK::isUavTaskAvailbal(int uavID)
{
    map<int,uavTask_t>::iterator it;
	it=m_uavTask.find(uavID);	
	if(it==m_uavTask.end())
	{
		cout<<"Task uavID not found"<<endl;
        return -1;
	}
	else	  
	{
		return 1;
	}
}

void UAV_TASK::updateUavStatus(MATCH_STATUS * pstMatch)
{
	m_weUavID.clear();
	m_weUav.clear();
	m_uavPlanID.clear();

	for(int i=0;i<pstMatch->nUavWeNum;i++)
	{
		if(pstMatch->astWeUav[i].nStatus != UAV_CRASH)//飞机有效
		{
			m_weUav.insert(pair<int,UAV> (i,pstMatch->astWeUav[i]));
			m_weUavID.insert(pair<int,UAV> (pstMatch->astWeUav[i].nNO,pstMatch->astWeUav[i]));
			m_uavPlanID.insert(pair<int, int>(pstMatch->astWeUav[i].nNO, i));

		}else{
			this->clearUavTaskWithID(pstMatch->astWeUav[i].nNO);

		}
			
	}
	
    cout<<"WeUavNUM="<<m_weUav.size()<<endl;

    m_enemyUavID.clear();
	for(int i=0;i<pstMatch->nUavEnemyNum;i++)
	{
        m_enemyUavID.insert(pair<int,UAV> (pstMatch->astEnemyUav[i].nNO,pstMatch->astEnemyUav[i]));
	}
	
    m_Goods.clear();
	for(int i=0;i<pstMatch->nGoodsNum;i++)
	{
        m_Goods.insert(pair<int,GOODS> (pstMatch->astGoods[i].nNO,pstMatch->astGoods[i]));
	}	

}
void UAV_TASK::uavTaskInIDEL(int uavID, UAV uavStatus, uavTask_t uavTask)
{
	 
	if(m_uavTask[uavID].taskState == UAV_STATE_CHARGE)
	{

	}
	if(m_uavTask[uavID].taskState == UAV_STATE_RAND)//在随机位置之前，需要运行分配算法
	{
        if(uavStatus.nZ<m_mapCreate->getMinFlyHeight())//在最低高度以下
		{

			int nextZ = m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nZ+1;
			UAV         m_UavStatusTemp;
			int         m_UavIDTemp;
			for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID 
			{
				m_UavStatusTemp = it->second;
				m_UavIDTemp 	= it->first;
				if(m_UavIDTemp != uavID)
				{
					if(m_UavStatusTemp.nX == uavStatus.nX && m_UavStatusTemp.nY == uavStatus.nY )
					{
						if(nextZ == m_pstFlayPlane->astUav[m_uavPlanID[m_UavIDTemp]].nZ)
						{
							if(nextZ>=1)
								nextZ--;
							break;
						}
					}
				}
				

			}
			m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nZ = nextZ;
        }else{//达到飞行高度
            //根据目标点，运行路径规划
            //m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nX++;
            //m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nY++;
		}
	}




}

void UAV_TASK::uavChargeProcess(int uavID, UAV uavStatus)//无人机充电
{
    if( isUavInHome(uavStatus.nX,uavStatus.nY)&&\
        m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nZ == 0&&\
            m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nGoodsNo == -1)//飞机在停机坪上,飞机没有载货，并且下一步不打算飞走
	{
		uavStatus.remainElectricity += m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).charge;
		if(uavStatus.remainElectricity >m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity)
		{
			// m_uavTask[uavID].taskState = UAV_STATE_RAND;
			uavStatus.remainElectricity = m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity;
		}	
	}
	m_pstFlayPlane->astUav[m_uavPlanID[uavID]].remainElectricity = uavStatus.remainElectricity ;
}

void UAV_TASK::uavTaskAssignGoods(int uavID, UAV uavStatus)
{
    map<float, int> goodsCost;
    goodsCost.clear();

    if(m_uavTask[uavID].taskClass == UAV_TASK_IDEL)//空闲任务下，规划任务
    {
        int goodsId=0;
        GOODS goodsStatus;
        int uavDistanceToGoods=0;
        for(map<int,GOODS>::iterator it= m_Goods.begin(); it!= m_Goods.end(); it++)
        {
            goodsId = it->first;
            goodsStatus = it->second;
            if(goodsStatus.nWeight < uavStatus.nLoadWeight && goodsStatus.nState == 0)//0表示货物正常可以被捡起来
            {
                map<int, int>::iterator its;
                its = m_uavGoodsID.find(goodsId);
                if(its == m_uavGoodsID.end())//当前货物ID没有和我方飞机绑定
                {
                    //
                    uavDistanceToGoods = abs(uavStatus.nZ - minFlyHeight)+minFlyHeight+ abs(goodsStatus.nStartX - uavStatus.nX) + abs(goodsStatus.nStartY - uavStatus.nY);
                }
            }
        }
    }
}

void UAV_TASK::uavTaskAssign(int uavID, UAV uavStatus)
{

    if(m_uavTask[uavID].taskClass == UAV_TASK_IDEL)//空闲任务下，规划任务
    {
        if(uavStatus.nLoadWeight == m_mapCreate->getMinPlaneWeight())//最小载重的飞机
        {
            int enemyUavWeightTemp=0;
            for(map<int,UAV>::iterator it= m_enemyUavID.begin(); it!= m_enemyUavID.end(); it++)
            {
                int uavEnemyId = it->first;
                UAV uavEnemyStatus = it->second;

                map<int,int>::iterator its;
                its=m_uavTrackID.find(uavEnemyId);//判断敌方飞机是否根我方攻击飞机关联上了

                if(its==m_uavTrackID.end())//如果没有关联上，找出未关联的飞机的最大重量
                {
                    if(uavEnemyStatus.nLoadWeight>enemyUavWeightTemp)
                    {
                        enemyUavWeightTemp = uavEnemyStatus.nLoadWeight;
                    }
                }
            }
            if(enemyUavWeightTemp != 0)
            {
                for(map<int,UAV>::iterator it= m_enemyUavID.begin(); it!= m_enemyUavID.end(); it++)
                {
                    int uavEnemyId = it->first;
                    UAV uavEnemyStatus = it->second;

                    map<int,int>::iterator its;
                    its=m_uavTrackID.find(uavEnemyId);//判断敌方飞机是否根我方攻击飞机关联上了

                    if(its==m_uavTrackID.end())//如果没有关联上，找出未关联的飞机的最大重量
                    {
                        if(uavEnemyStatus.nLoadWeight == enemyUavWeightTemp)//找到一个
                        {
                            cout<<"Find enemy uavID to track, UAV weight = "<<enemyUavWeightTemp<<"; UAV ID= "<<uavEnemyId<<endl;
                            m_uavTrackID.insert(pair<int, int>(uavEnemyId,uavID));//将敌方我方飞机ID关联上
                            m_uavTask[uavID].taskClass = UAV_TASK_TRACK;
                            m_uavTask[uavID].enemyNo   = uavEnemyId;
                        }
                    }
                }
            }
        }//
        if(m_uavTask[uavID].taskClass == UAV_TASK_IDEL)//空闲任务下，规划任务
        {

            if(uavStatus.remainElectricity >=m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity)//have full power
            {//飞机满电，要么出去取货，要么安排随机地址

            }
        }


    }
}

void UAV_TASK::uavTaskAllot(int uavID, UAV uavStatus)
{
	uavTask_t   m_UavTaskTemp;
	if(this->isUavTaskAvailbal(uavID)!=1)//不存在uav task，飞机首次出现，初始化
	{
		m_UavTaskTemp.taskClass = UAV_TASK_IDEL;
		m_UavTaskTemp.taskState = UAV_STATE_CHARGE;
        m_UavTaskTemp.enemyNo   = -1;
        m_UavTaskTemp.goodsNo   = -1;
		this->setUavTaskWithID(uavID,m_UavTaskTemp);
	}

	//need todo ...

	if(uavStatus.nLoadWeight == m_mapCreate->getMinPlaneWeight())
	{
		m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
		m_uavTask[uavID].taskState = UAV_STATE_RAND;		
	}
	if(uavStatus.remainElectricity >=m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity)//have full power
 	{
		m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
		m_uavTask[uavID].taskState = UAV_STATE_RAND;
		m_uavTask[uavID].nextLocation.x = uavID+1;
		m_uavTask[uavID].nextLocation.y = 8;
		m_uavTask[uavID].nextLocation.z = 8;

	}
	//在运行任务之前，需要运行分配算法
	switch(m_uavTask[uavID].taskClass)
	{
		case UAV_TASK_IDEL:
			uavTaskInIDEL(uavID,uavStatus,m_UavTaskTemp);
			break;
		case UAV_TASK_GOODS:

			break;
		case UAV_TASK_TRACK:

			break;
		
		default:
			cout<< "there has none TASK CLASS, maybe has problem!!!"<<endl;
			break;
	}

	uavChargeProcess(uavID,uavStatus);
		
	
}

void UAV_TASK::uavTaskProcess(MATCH_STATUS * pstMatch)
{
	int         uavIDTemp;
	

    this->updateUavStatus(pstMatch);//更新飞行器状态



	for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID 
	{

        m_pstFlayPlane->astUav[m_uavPlanID[it->first]] = it->second;
        uavTaskAllot(it->first,it->second);

	}

	for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID 
	{
		uavIDTemp 	= it->first;
		
		cout<<uavIDTemp<<":="<<m_uavTask[uavIDTemp].taskState<<"; ";
	}
	cout<<endl;

	for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID 
	{
		uavIDTemp 	= it->first;
		cout<<uavIDTemp<<":="<<m_pstFlayPlane->astUav[m_uavPlanID[uavIDTemp]].nZ<<"; ";
	}
	cout<<endl;

}








