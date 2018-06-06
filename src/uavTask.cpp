#include "uavTask.h"


UAV_TASK::UAV_TASK(MAP_CREATE *m_map_create, FLAY_PLANE* pstFlayPlane):m_mapCreate(m_map_create),m_pstFlayPlane(pstFlayPlane)
{
	weHomeX = m_map_create->getUavWeHome().first;
	weHomeY = m_map_create->getUavWeHome().second;

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
	
	cout<<"WeUavNUM"<<m_weUav.size()<<endl;

	enemyUav.clear();
	for(int i=0;i<pstMatch->nUavEnemyNum;i++)
	{
		enemyUav.insert(pair<int,UAV> (pstMatch->astEnemyUav[i].nNO,pstMatch->astEnemyUav[i]));
	}
	
	Goods.clear();
	for(int i=0;i<pstMatch->nGoodsNum;i++)
	{
		Goods.insert(pair<int,GOODS> (pstMatch->astGoods[i].nNO,pstMatch->astGoods[i]));
	}	

}
void UAV_TASK::uavTaskInIDEL(int uavID, UAV uavStatus, uavTask_t uavTask)
{
	 
	if(m_uavTask[uavID].taskState == UAV_STATE_CHARGE)
	{
		// if(uavStatus.nZ == 0 && isUavInHome(uavStatus.nX,uavStatus.nY))//飞机在停机坪上
		// {

		// 	m_uavTask[uavID].taskState = UAV_STATE_CHARGE;

		// 	uavStatus.remainElectricity += m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).charge;
		// 	if(uavStatus.remainElectricity >m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity)
		// 	{
		// 		m_uavTask[uavID].taskState = UAV_STATE_RAND;
		// 		uavStatus.remainElectricity = m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity;
		// 	}
				
		// }
	}
	if(m_uavTask[uavID].taskState == UAV_STATE_RAND)//在随机位置之前，需要运行分配算法
	{
		if(uavStatus.nZ<m_mapCreate->getMinFlyHeight())
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
		}else{
			m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nX++;
			m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nY++;
		}
	}




}

void UAV_TASK::uavChargeProcess(int uavID, UAV uavStatus)
{

	if( uavStatus.nZ == 0 && isUavInHome(uavStatus.nX,uavStatus.nY)&&\
		m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nZ == 0)//飞机在停机坪上
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
void UAV_TASK::uavTaskAllot(int uavID, UAV uavStatus)
{
	uavTask_t   m_UavTaskTemp;
	if(this->isUavTaskAvailbal(uavID)!=1)//不存在uav task，飞机首次出现，初始化
	{
		m_UavTaskTemp.taskClass = UAV_TASK_IDEL;
		m_UavTaskTemp.taskState = UAV_STATE_CHARGE;

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
    UAV         uavStatusTemp;
	int         uavIDTemp;
	

	this->updateUavStatus(pstMatch);

	for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID 
	{
		uavStatusTemp = it->second;
		uavIDTemp 	= it->first;

		m_pstFlayPlane->astUav[m_uavPlanID[uavIDTemp]] = uavStatusTemp;
		uavTaskAllot(uavIDTemp,uavStatusTemp);

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








