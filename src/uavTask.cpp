#include "uavTask.h"


UAV_TASK::UAV_TASK(MAP_CREATE *m_map_create, FLAY_PLANE* pstFlayPlane):m_mapCreate(m_map_create),m_pstFlayPlane(pstFlayPlane)
{
	weHomeX = m_map_create->getUavWeHome().first;
	weHomeY = m_map_create->getUavWeHome().second;

    minFlyHeight = m_map_create->getMinFlyHeight();
    MaxFlyHeight = m_map_create->getMaxFlyHeight();

    mapXsize    = m_map_create->getMapXsize() - 1;
    mapYsize    = m_map_create->getMapYsize() - 1;
    mapZsize    = m_map_create->getMapZsize() - 1;
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
        //cout<<"Task uavID not found"<<endl;
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

    cout<<"WeUavNUM="<<m_weUav.size()<<"; enemyUavNum="<<m_enemyUavID.size()<<"; goodsNum="<<m_Goods.size()<<endl;
}

void UAV_TASK::uavRun(int uavID, UAV uavStatus)
{
    if(m_uavTask[uavID].taskClass == UAV_TASK_IDEL)//
    {
        if(m_uavTask[uavID].taskState == UAV_STATE_CHARGE)
        {

        }
    }
}

void UAV_TASK::uavTaskInIDEL(int uavID, UAV uavStatus)
{

    //空闲任务下，没有分配到任务，判断电量是否充满
    if(uavStatus.remainElectricity >= m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity)//have full power
    {//飞机满电，要么出去取货，要么安排随机地址
        m_uavTask[uavID].taskState = UAV_STATE_RAND;
    }

    if(m_uavTask[uavID].taskState != UAV_STATE_CHARGE)
    {//飞机不是满电，且没有收到任务，进入充电模式
     //todo... 加入空闲时间判断，一段时间内没有收到任务，也回家

        float uavRemainStep = (float)uavStatus.remainElectricity/(float)uavStatus.nLoadWeight;
        if(uavRemainStep < (minFlyHeight*2))//电量不够支撑一上一下，进入充电模式
            m_uavTask[uavID].taskState = UAV_STATE_CHARGE;

        if(uavStatus.remainElectricity < m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity/2)
        {
            m_uavTask[uavID].taskInIdelTime++;//时间判断，超时
            if(m_uavTask[uavID].taskInIdelTime>3)
            {
                m_uavTask[uavID].taskInIdelTime =0;
                m_uavTask[uavID].taskState = UAV_STATE_CHARGE;
            }
        }
    }

	if(m_uavTask[uavID].taskState == UAV_STATE_CHARGE)
	{
        m_uavTask[uavID].goalLocation.x = weHomeX;//目标点的坐标
        m_uavTask[uavID].goalLocation.y = weHomeY;
        m_uavTask[uavID].goalLocation.z = 0;

	}
    if(m_uavTask[uavID].taskState == UAV_STATE_RAND)//分配随机位置
    {//todo... 随机位置分配，需要优化
        int randX=0;
        int randY=0;
        int randZ=minFlyHeight;

        srand((int)uavID);
        randX = random(mapXsize/3);
        randY = random(mapYsize/3);
        int groupCoord = uavID%4;
        switch(groupCoord)
        {
        case 0:

            break;
        case 1:
            randX = randX + mapXsize/2;
            break;
        case 2:
            randY = randY + mapYsize/2;
            break;
        case 3:
            randX = randX + mapXsize/2;
            randY = randY + mapYsize/2;
            break;
        }

        if(randX<0)randX=0;
        if(randY<0)randY=0;
        if(randX>mapXsize) randX = mapXsize;
        if(randY>mapYsize) randY = mapYsize;

        if(m_mapCreate->get_mappoint(randX,randY,randZ)!=1)
        {
            m_uavTask[uavID].goalLocation.x = randX;//目标点的坐标
            m_uavTask[uavID].goalLocation.y = randY;
            m_uavTask[uavID].goalLocation.z = randZ;
        }else{
            while(m_mapCreate->get_mappoint(randX,randY,randZ)==1)
            {
                if(randZ<MaxFlyHeight)
                    randZ++;
                else{
                    if(randX<mapXsize)
                        randX++;
                    else if(randY>0) randY--;
                       else{

                        break;
                    }
                }
            }
            if(randX<0)randX=0;
            if(randY<0)randY=0;
            if(randX>mapXsize) randX = mapXsize;
            if(randY>mapYsize) randY = mapYsize;

            m_uavTask[uavID].goalLocation.x = randX;//目标点的坐标
            m_uavTask[uavID].goalLocation.y = randY;
            m_uavTask[uavID].goalLocation.z = randZ;
        }
         cout<<"Rand Location, uav ID="<<uavID<<"; rand :x="<<m_uavTask[uavID].goalLocation.x<<";y="<<m_uavTask[uavID].goalLocation.y<<endl;

//        if(uavStatus.nZ<m_mapCreate->getMaxFlyHeight())//在最低高度以下
//        {

//            int nextZ = m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nZ+1;
//            UAV         m_UavStatusTemp;
//            int         m_UavIDTemp;
//            for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID
//            {
//                m_UavStatusTemp = it->second;
//                m_UavIDTemp 	= it->first;
//                if(m_UavIDTemp != uavID)
//                {
//                    if(m_UavStatusTemp.nX == uavStatus.nX && m_UavStatusTemp.nY == uavStatus.nY )
//                    {
//                        if(nextZ == m_pstFlayPlane->astUav[m_uavPlanID[m_UavIDTemp]].nZ)
//                        {
//                            if(nextZ>=1)
//                                nextZ--;
//                            break;
//                        }
//                    }
//                }
				

//            }
//            m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nZ = nextZ;
//        }else{//达到飞行高度
//            //根据目标点，运行路径规划
//            //cout<<"maxHeightFly"<<m_mapCreate->getMaxFlyHeight()<<" ;MYnZ="<<m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nZ<<endl;
//            if(m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nX<mapXsize)
//                m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nX++;
//            if(m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nY<mapYsize)
//                m_pstFlayPlane->astUav[m_uavPlanID[uavID]].nY++;
//        }
	}
}
void UAV_TASK::uavTaskInGoods(int uavID, UAV uavStatus)
{
    if(m_uavTask[uavID].taskState == UAV_GOOD_TO_GET)//本机器的状态为取货中，还未取到货
    {
        if(uavStatus.nGoodsNo != -1)
        {
            m_uavTask[uavID].taskState = UAV_GOOD_TO_PUT;
        }else
        {
            GOODS goodsStatus = m_Goods[m_uavTask[uavID].goodsNo];


            m_uavTask[uavID].goalLocation.x = goodsStatus.nStartX;//
            m_uavTask[uavID].goalLocation.y = goodsStatus.nStartY;
            m_uavTask[uavID].goalLocation.z = 0;

            cout<<"goodsID="<<m_uavTask[uavID].goodsNo<<"; goods Start :x="<<m_uavTask[uavID].goalLocation.x<<";y="<<m_uavTask[uavID].goalLocation.y<<endl;
        }


    }
    if(m_uavTask[uavID].taskState == UAV_GOOD_TO_PUT)
    {
        if(uavStatus.nGoodsNo == -1)//放货完成
        {
            m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
            m_uavTask[uavID].taskState = UAV_STATE_RAND;
            uavTaskInIDEL(uavID, uavStatus);//调度空闲任务
        }else
        {
            GOODS goodsStatus = m_Goods[m_uavTask[uavID].goodsNo];


            m_uavTask[uavID].goalLocation.x = goodsStatus.nEndX;//
            m_uavTask[uavID].goalLocation.y = goodsStatus.nEndY;
            m_uavTask[uavID].goalLocation.z = 0;
            cout<<"goods End :x="<<m_uavTask[uavID].goalLocation.x<<";y="<<m_uavTask[uavID].goalLocation.y<<endl;
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
        float uavDisToGetGoods=0;
        float uavDisToPutGoods=0;

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
                    uavDisToGetGoods = abs(uavStatus.nZ - minFlyHeight)+minFlyHeight+ \
                                       abs(goodsStatus.nStartX - uavStatus.nX) + abs(goodsStatus.nStartY - uavStatus.nY);

                    int left_time = goodsStatus.nLeftTime;
                    if(left_time>uavDisToGetGoods)//剩余时间足够UAV去捡起来货物
                    {
//                        uavDisToPutGoods = 2*minFlyHeight + pow(pow(abs(goodsStatus.nEndX - goodsStatus.nStartX), 2) + pow(abs(goodsStatus.nEndY - goodsStatus.nStartY),2),0.5);
                        uavDisToPutGoods = 2*minFlyHeight + abs(goodsStatus.nEndX - goodsStatus.nStartX) + abs(goodsStatus.nEndY - goodsStatus.nStartY);
                        float needPower = uavDisToPutGoods * goodsStatus.nWeight;
                        if(needPower < uavStatus.remainElectricity)//无人机电量足够使用
                        {
                            float goodsWorth      = goodsStatus.nValue;
                            float distance        = uavDisToGetGoods + uavDisToPutGoods;
                            float planeLoadWeight = uavStatus.nLoadWeight;
                            float percentWorth    = (float)(goodsWorth/distance/planeLoadWeight);
                            goodsCost.insert(pair<float, int>(percentWorth, goodsId));//遍历得到所有可以取货的价值表
                        }
                    }
                }
            }
        }
        if(goodsCost.empty())
        {

        }else{
            float goodsValuePerDis=0;
            for(map<float, int>::iterator it= goodsCost.begin(); it!= goodsCost.end(); it++)
            {
                 if(goodsValuePerDis < it->first)
                 {
                     goodsValuePerDis = it->first;//find max value
                 }
            }
            int uavGoodsId = goodsCost[goodsValuePerDis];
            cout<<"uavID:"<<uavID<<"; find the goods to get=:"<<uavGoodsId<<endl;
            m_uavGoodsID.insert(pair<int,int>(uavGoodsId,uavID));
            m_uavTask[uavID].taskClass = UAV_TASK_GOODS;
            m_uavTask[uavID].taskState = UAV_GOOD_TO_GET;
            m_uavTask[uavID].goodsNo   = uavGoodsId;
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
        }//优先攻击机器规划完成
        if(m_uavTask[uavID].taskClass == UAV_TASK_IDEL)//空闲任务下，规划任务
        {
            uavTaskAssignGoods(uavID,uavStatus);//调度取货算法，任务切换；如果不满足条件，继续执行IDEL任务
        }
    }//空闲任务
    else if(m_uavTask[uavID].taskClass == UAV_TASK_GOODS)//飞机的当前任务为取货状态
    {//todo ... 判断当前货物是否有效，是否有敌方飞机去取货

        if(m_uavTask[uavID].taskState == UAV_GOOD_TO_GET)//本机器的状态为取货中，还未取到货
        {
            map<int,GOODS>::iterator its;
            its=m_Goods.find(m_uavTask[uavID].goodsNo);//判断当前ID 号对应的货物是否存在

            if(its==m_Goods.end())//货物不存在
            {
                cout<<"goods is noon"<<endl;
                m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
                m_uavTask[uavID].taskState = UAV_STATE_RAND;
                uavTaskAssignGoods(uavID,uavStatus);//调度取货算法，任务切换；如果不满足条件，继续执行IDEL任务
            }else{//当前ID号存在

                GOODS goodsStatus = m_Goods[m_uavTask[uavID].goodsNo];
                cout<<"goodsStatus="<<goodsStatus.nState;

                if(goodsStatus.nState !=0|| goodsStatus.nLeftTime == 0)//当前货物已经被取走,或者剩余时间为0
                {
                    cout<<"goods is noon"<<endl;
                    m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
                    m_uavTask[uavID].taskState = UAV_STATE_RAND;
                    uavTaskAssignGoods(uavID,uavStatus);//调度取货算法，任务切换；如果不满足条件，继续执行IDEL任务
                }else if(goodsStatus.nState ==0){//当前货物还在，
                    //todo... 需要判断货物处有无敌方无人机，如果敌方无人机正在去取货，调整相应的策略

                }
            }

        }
    }
    else if(m_uavTask[uavID].taskClass == UAV_TASK_TRACK)//飞机的当前任务为攻击状态
    {//todo ...
        //同上，使用敌方无人机ID时候，需要判断是否存在
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
        m_UavTaskTemp.taskInIdelTime = 0;
		this->setUavTaskWithID(uavID,m_UavTaskTemp);
	}

	//need todo ...

    uavTaskAssign(uavID, uavStatus);

	//在运行任务之前，需要运行分配算法
	switch(m_uavTask[uavID].taskClass)
	{
		case UAV_TASK_IDEL:
            uavTaskInIDEL(uavID,uavStatus);
			break;
		case UAV_TASK_GOODS:
            uavTaskInGoods(uavID,uavStatus);
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
//	int         uavIDTemp;
	

    this->updateUavStatus(pstMatch);//更新飞行器状态



	for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID 
	{

        m_pstFlayPlane->astUav[m_uavPlanID[it->first]] = it->second;
        uavTaskAllot(it->first,it->second);

	}

//	for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID
//	{
//		uavIDTemp 	= it->first;
		
//		cout<<uavIDTemp<<":="<<m_uavTask[uavIDTemp].taskState<<"; ";
//	}
//	cout<<endl;

//	for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID
//	{
//		uavIDTemp 	= it->first;
//		cout<<uavIDTemp<<":="<<m_pstFlayPlane->astUav[m_uavPlanID[uavIDTemp]].nZ<<"; ";
//	}
//	cout<<endl;

}








