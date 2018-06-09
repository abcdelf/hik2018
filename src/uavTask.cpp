#include "uavTask.h"

bool sortFromMinToMax(const UAV &a, const UAV &b){//小于比较
    return a.nZ < b.nZ;
}
bool sortFromMaxToMin(const UAV &a, const UAV &b){//大于比较
    return a.nZ > b.nZ;
}



UAV_TASK::UAV_TASK(MAP_CREATE *m_map_create, FLAY_PLANE* pstFlayPlane, pathSearch* pPathSearch ):m_mapCreate(m_map_create),m_pstFlayPlane(pstFlayPlane),m_PathSearch(pPathSearch)
{
	weHomeX = m_map_create->getUavWeHome().first;
	weHomeY = m_map_create->getUavWeHome().second;

    minFlyHeight = m_map_create->getMinFlyHeight();
    MaxFlyHeight = m_map_create->getMaxFlyHeight();

    mapXsize    = m_map_create->getMapXsize() - 1;
    mapYsize    = m_map_create->getMapYsize() - 1;
    mapZsize    = m_map_create->getMapZsize() - 1;

    m_cheapestUavPrice = m_map_create->getPlaneUavPrice(m_map_create->getMinPlaneWeight());

    isJustStart = -1;
    enemyUavHomeX = -1;
    enemyUavHomeY = -1;
    uavOutHomeQueue.clear();
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
        //cout<<"Task uavID not found"<<endl;
        return -1;
	}
	else	  
	{
		return 1;
	}
}

void UAV_TASK::findEnemyHome(MATCH_STATUS * pstMatch)//isJustStart = 0;
{
  int initState=0;
  if(isJustStart == -1)
  {
    if(pstMatch->nUavEnemyNum >=2 )
    {
      if( (pstMatch->astEnemyUav[0].nX== pstMatch->astEnemyUav[1].nX)&&\
          (pstMatch->astEnemyUav[0].nY== pstMatch->astEnemyUav[1].nY)&&\
          (pstMatch->astEnemyUav[0].nZ== pstMatch->astEnemyUav[1].nZ)&&\
          (UAV_CRASH != pstMatch->astEnemyUav[0].nStatus) && (pstMatch->astEnemyUav[1].nStatus!=UAV_CRASH))
      {
        initState=1;
      }
    }
    if(initState==1)
    {
        isJustStart = 0;
        enemyUavHomeX = pstMatch->astEnemyUav[0].nX;
        enemyUavHomeY = pstMatch->astEnemyUav[0].nY;

        cout<<"enemyHomeX="<<enemyUavHomeX<<" ; enemyHomeY="<<enemyUavHomeY<<endl;

    }else
    {
        enemyUavHomeX = -1;
        enemyUavHomeY = -1;
    }
  }
}

void UAV_TASK::updateUavStatus(MATCH_STATUS * pstMatch)
{

    findEnemyHome(pstMatch);

	m_weUavID.clear();
	m_weUav.clear();
	m_uavPlanID.clear();
    m_weUavNextPiont.clear();//保存我方无人机的要走的位置，初始化为当前坐标
    m_weUavNowPiont.clear();

    uavCoord_t coordTemp;
	for(int i=0;i<pstMatch->nUavWeNum;i++)
	{
		if(pstMatch->astWeUav[i].nStatus != UAV_CRASH)//飞机有效
		{
			m_weUav.insert(pair<int,UAV> (i,pstMatch->astWeUav[i]));
			m_weUavID.insert(pair<int,UAV> (pstMatch->astWeUav[i].nNO,pstMatch->astWeUav[i]));
			m_uavPlanID.insert(pair<int, int>(pstMatch->astWeUav[i].nNO, i));

            coordTemp.x = pstMatch->astWeUav[i].nX;
            coordTemp.y = pstMatch->astWeUav[i].nY;
            coordTemp.z = pstMatch->astWeUav[i].nZ;
            m_weUavNextPiont.insert(pair<int, uavCoord_t>(pstMatch->astWeUav[i].nNO, coordTemp));
            m_weUavNowPiont.insert(pair<int, uavCoord_t>(pstMatch->astWeUav[i].nNO, coordTemp));
		}else{
			this->clearUavTaskWithID(pstMatch->astWeUav[i].nNO);

		}
			
	}
	


    m_enemyUavID.clear();
    m_enemyUavNowPiont.clear();
	for(int i=0;i<pstMatch->nUavEnemyNum;i++)
	{
        m_enemyUavID.insert(pair<int,UAV> (pstMatch->astEnemyUav[i].nNO,pstMatch->astEnemyUav[i]));

        coordTemp.x = pstMatch->astEnemyUav[i].nX;
        coordTemp.y = pstMatch->astEnemyUav[i].nY;
        coordTemp.z = pstMatch->astEnemyUav[i].nZ;
        m_enemyUavNowPiont.insert(pair<int, uavCoord_t>(pstMatch->astEnemyUav[i].nNO, coordTemp));
	}
	
    m_Goods.clear();
	for(int i=0;i<pstMatch->nGoodsNum;i++)
	{
        m_Goods.insert(pair<int,GOODS> (pstMatch->astGoods[i].nNO,pstMatch->astGoods[i]));
	}	

    m_weMoney = pstMatch->nWeValue;//更新我方的money
    m_enemyUavNum = pstMatch->nUavEnemyNum;//更新敌方无人机数量
    m_runTime = pstMatch->nTime;
/*
//    cout<<"WeUavNUM="<<m_weUav.size()<<"; enemyUavNum="<<m_enemyUavID.size()<<\
//          "; goodsNum="<<m_Goods.size()<<"; m_weMoney="<<m_weMoney<<"; m_enemyUavNum="<<m_enemyUavNum<<\
//          "; m_runTime="<<m_runTime<<endl;
*/
}

void UAV_TASK::uavRun(int uavID, UAV uavStatus)
{
    m_uavTask[uavID].nextLocation.x=uavStatus.nX;
    m_uavTask[uavID].nextLocation.y=uavStatus.nY;
    m_uavTask[uavID].nextLocation.z=uavStatus.nZ;

    m_uavTask[uavID].nowLocation.x=uavStatus.nX;
    m_uavTask[uavID].nowLocation.y=uavStatus.nY;
    m_uavTask[uavID].nowLocation.z=uavStatus.nZ;

    vector<pair<int, int>> weUavObstaclePos;
    weUavObstaclePos.clear();

    int DontMoveStatus=0;
    int needMoveAwayEnwmy=0;

    int uavWeIDTemp =0;
    uavCoord_t uavWeTempCoord;
    int weUavDisX=0;
    int weUavDisY=0;


    int enemyUavIDTemp =0;
    uavCoord_t enemyUavTempCoord;
    int weUavToEnemyDisX=0;
    int weUavToEnemyDisY=0;

//    if(m_uavTask[uavID].taskClass != UAV_TASK_TRACK)// 普通模式下，避开敌方无人机，避开我方无人机
//    {


//    }else//攻击机模式下，不避开敌方无人机，需要避开敌方低价值无人机,避开我方无人机
//    {

//    }


    for(map<int,uavCoord_t>::iterator it= m_enemyUavNowPiont.begin(); it!= m_enemyUavNowPiont.end(); it++)//扁历存在的ID
    {
        enemyUavIDTemp      = it->first;
        enemyUavTempCoord   = it->second;

        if(abs(enemyUavTempCoord.z - uavStatus.nZ) < 1)//敌方飞机和我方无人机在上下两层高度
        {
            weUavToEnemyDisX = abs(enemyUavTempCoord.x - uavStatus.nX);
            weUavToEnemyDisY = abs(enemyUavTempCoord.y - uavStatus.nY);
            if(weUavToEnemyDisX<3&&weUavToEnemyDisY<3)//2*2
            {
                int goodsValue =0;
                GOODS goodsStatusTemp;
                if(uavStatus.nGoodsNo!=-1)
                {
                   goodsStatusTemp = m_Goods[uavStatus.nGoodsNo];
                   goodsValue = goodsStatusTemp.nValue;
                }
                int weUavNowValue = uavStatus.nLoadWeight;//*0.6 + uavStatus.remainElectricity*0.01 + goodsValue*0.3;

                goodsValue =0;
                if(m_enemyUavID[enemyUavIDTemp].nGoodsNo!=-1)
                {
                   goodsStatusTemp = m_Goods[m_enemyUavID[enemyUavIDTemp].nGoodsNo];
                   goodsValue = goodsStatusTemp.nValue;
                }
                int enemyUavNowValue = m_enemyUavID[enemyUavIDTemp].nLoadWeight;//*0.6 + m_enemyUavID[enemyUavIDTemp].remainElectricity*0.01 + goodsValue*0.3;
                if(weUavNowValue > enemyUavNowValue)//我方无人机价值大于敌方无人机，注意避让敌方无人机
                {
                     //cout<<"our Uav piont =:("<<uavStatus.nX<<" ,"<<uavStatus.nY<<") "<<endl;
                    // cout<<"add enemey piont =:"<<endl;
                     needMoveAwayEnwmy = 1;
//                    for(int m=-1; m<2; m++)
//                    {
//                        for(int n=-1; n<2; n++)
//                        {
//                            weUavObstaclePos.push_back(make_pair(enemyUavTempCoord.x+n,enemyUavTempCoord.y+m));
//                      //      cout<<"("<<enemyUavTempCoord.x+n<<" ,"<<enemyUavTempCoord.y+m<<") ";
//                        }
//                     //   cout<<endl;
//                    }
//                    weUavObstaclePos.push_back(make_pair(enemyUavTempCoord.x,enemyUavTempCoord.y));
//                    weUavObstaclePos.push_back(make_pair(enemyUavTempCoord.x,enemyUavTempCoord.y));
                }
            }

        }

    }
    //我方无人机障碍,避开我方无人机

    for(map<int,uavCoord_t>::iterator it= m_weUavNextPiont.begin(); it!= m_weUavNextPiont.end(); it++)//扁历存在的ID
    {
        uavWeIDTemp      = it->first;
        uavWeTempCoord   = it->second;
        if(uavWeIDTemp != uavID)
        {
            if(uavWeTempCoord.z == uavStatus.nZ)
            {
                weUavDisX =abs(uavWeTempCoord.x - uavStatus.nX);
                weUavDisY =abs(uavWeTempCoord.y - uavStatus.nY);

                if(weUavDisX<3&&weUavDisY<3)//2*2以内
                {
                    //todo ... 此处斜线判断，放到最后数据校验
                    if(m_weUavNowPiont[uavWeIDTemp].x == uavStatus.nX || m_weUavNowPiont[uavWeIDTemp].y == uavStatus.nX)//无人机相临
                    {
                        weUavDisX =abs(uavWeTempCoord.x - m_weUavNowPiont[uavWeIDTemp].x);//该无人机有无走斜线
                        weUavDisY =abs(uavWeTempCoord.y - m_weUavNowPiont[uavWeIDTemp].y);
                        if(weUavDisX==1&&weUavDisY==1)//该无人机走的斜线
                        {
                            needMoveAwayEnwmy=1;
//                            //cout<<"uav xiexian ----+++"<<endl;
//                            if(uavStatus.nX == m_weUavNowPiont[uavWeIDTemp].x && abs(uavStatus.nY - m_weUavNowPiont[uavWeIDTemp].y)==1)
//                            {
//                            //    cout<<"uav xiexian xxxxxx = "<<uavID<<" ; "<<endl;
//                                DontMoveStatus = 1;
//                               //weUavObstaclePos.push_back(make_pair(uavWeTempCoord.x,m_weUavNowPiont[uavWeIDTemp].y));
//                            }
//                            if(uavStatus.nY == m_weUavNowPiont[uavWeIDTemp].y && abs(uavStatus.nX - m_weUavNowPiont[uavWeIDTemp].x)==1)
//                            {
//                             //   cout<<"uav xiexian yyyyy"<<uavID<<" ; "<<endl;
//                                // weUavObstaclePos.push_back(make_pair(m_weUavNowPiont[uavWeIDTemp].x,uavWeTempCoord.y));
//                                DontMoveStatus = 1;

//                            }
                        }
                    }

                    weUavObstaclePos.push_back(make_pair(uavWeTempCoord.x,uavWeTempCoord.y));
                }
            }
/*
//            if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)//不在家的位置上// 放在最后的策略调整上//需要测试，什么原因导致任务分配失败
//            {
//                if(m_uavTask[uavID].taskClass == UAV_TASK_TRACK)
//                {
//                    if(uavWeTempCoord.z<minFlyHeight+1)
//                    {
//                        if(m_uavTask[uavID].enemyNo != -1)
//                        {
//                            if(m_uavTask[uavWeIDTemp].goalLocation.x == m_uavTask[uavID].goalLocation.x &&\
//                               m_uavTask[uavWeIDTemp].goalLocation.y == m_uavTask[uavID].goalLocation.y)//目标相同
//                            {
//                                weUavObstaclePos.push_back(make_pair(uavWeTempCoord.x,uavWeTempCoord.y));
//                            }
//                        }
//                    }
//                }
//            }

//            else if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)//不在家的位置上// 放在最后的策略调整上
//            {
//                if(uavWeTempCoord.x == uavStatus.nX && uavWeTempCoord.y == uavStatus.nY)//不是同一高度，但是在同一点，需要判断能否上下运动
//                {
//                    if(abs(uavWeTempCoord.z - uavStatus.nZ)<2)//飞机上下相邻，不能上下走
//                    {
//                        isZCanMoveStatus=1;

//                        if(uavStatus.nZ<minFlyHeight&&uavStatus.nZ<uavWeTempCoord.z)//当前飞机低于攻击机器的高度，并且小于最低能飞高度
//                        {
//                            if(m_uavTask[uavWeIDTemp].goalLocation.x == m_uavTask[uavID].goalLocation.x &&\
//                                    m_uavTask[uavWeIDTemp].goalLocation.y == m_uavTask[uavID].goalLocation.y)//目标相同
//                            if(m_uavTask[uavWeIDTemp].taskClass == UAV_TASK_TRACK)
//                            {
//                                if(m_uavTask[uavID].nextLocation.z < MaxFlyHeight)
//                                m_uavTask[uavWeIDTemp].nextLocation.z++;
//                                m_weUavNextPiont[uavWeIDTemp] = m_uavTask[uavWeIDTemp].nextLocation;//更新我方无人机下一步的位置

//                            }
//                        }
//                    }
//                }
//            }*/

        }
    }
    //todo ...
    //添加我方无人机障碍点


    if(uavStatus.nZ==0&& isUavInHome(uavStatus.nX,uavStatus.nY)==1)//当前飞机在家
    {
        if(m_uavTask[uavID].taskState != UAV_STATE_CHARGE && m_uavTask[uavID].taskState != UAV_STATE_BACK_CHARGE )
        {
            if(m_uavTask[uavID].uavHomeStatus == UAV_NOT_IN_QUEUE)
            {
                if(uavBackHomeQueue.empty())
                {
                    m_uavTask[uavID].uavHomeStatus = UAV_IN_OUT_QUEUE;
                    //cout<<"add to uavOutHomeQueue=:"<<uavStatus.nNO<<endl;
                    uavOutHomeQueue.push_back(uavStatus);
                }
            }
        }
    }else if(m_uavTask[uavID].uavHomeStatus == UAV_IN_OUT_QUEUE)
    {
        if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)
        {
            m_uavTask[uavID].uavHomeStatus = UAV_NOT_IN_QUEUE;
            vector<UAV>::iterator it;

            for(it=uavOutHomeQueue.begin();it!=uavOutHomeQueue.end();it++)
            {
                UAV uavTemp = *it;
                if(uavTemp.nNO == uavID)
                {
                    //cout<<"erase ok uav ID ="<<uavTemp.nNO <<endl;
                    uavOutHomeQueue.erase(it);
                    break;
                }
            }
        }
    }

    if(m_uavTask[uavID].taskState == UAV_STATE_BACK_CHARGE)//当前飞机需要回家充电
    {
        if(isUavInHome(uavStatus.nX,uavStatus.nY)==1 && uavStatus.nZ>0)//
        {
            if(m_uavTask[uavID].uavHomeStatus == UAV_NOT_IN_QUEUE)
            {
                if(uavOutHomeQueue.empty())//||!uavBackHomeQueue.empty()
                {
                    m_uavTask[uavID].uavHomeStatus = UAV_IN_BACK_QUEUE;//飞机到达home点
                   // cout<<"add to uavBackHomeQueue _-------------------=:"<<uavStatus.nNO<<endl;
                    uavBackHomeQueue.push_back(uavStatus);
                }
            }
        }else if(isUavInHome(uavStatus.nX,uavStatus.nY)==1 && uavStatus.nZ==0)
        {
            m_uavTask[uavID].taskState = UAV_STATE_CHARGE;

            if(m_uavTask[uavID].uavHomeStatus == UAV_IN_BACK_QUEUE)
            {
                m_uavTask[uavID].uavHomeStatus = UAV_NOT_IN_QUEUE;
                vector<UAV>::iterator it;

                for(it=uavBackHomeQueue.begin();it!=uavBackHomeQueue.end();it++)
                {
                    UAV uavTemp = *it;
                    if(uavTemp.nNO == uavID)
                    {
                      //  cout<<"erase ok uav ID ="<<uavTemp.nNO <<endl;
                        uavBackHomeQueue.erase(it);
                        break;
                    }
                }
            }
        }else if(DontMoveStatus == 0)
        {
            UAV uavStatusTemp;
            int needMinHeight = minFlyHeight;

            if(!uavBackHomeQueue.empty())
            {
                sort(uavBackHomeQueue.begin(),uavBackHomeQueue.end(),sortFromMaxToMin);//按Z从大到小排序
                uavStatusTemp = uavBackHomeQueue[0];//得到当前队列中高度最高的那个元素
                needMinHeight = uavStatusTemp.nZ;//m_weUavID[uavStatusTemp.nNO].nZ+1;
            }

            int heightStatus=0;

            vector<pair<int, int>> obstaclePos;
            if(uavStatus.nZ>minFlyHeight)//jisuan lujing
            {
                int weUavDisX=abs(uavStatus.nX - weHomeX);
                int weUavDisY=abs(uavStatus.nY - weHomeY);

                if(weUavDisX<=1&&weUavDisY<=1)
                {
                    if(uavStatus.nZ < needMinHeight)//小于最低高度，需要上升
                    {
                        if(m_uavTask[uavID].nextLocation.z < MaxFlyHeight)
                        {
                          //  cout<<"uav ID="<<uavID<<" ;no road find, the uav will fly up"<<endl;
                          m_uavTask[uavID].nextLocation.z ++;
                          heightStatus = 1;
                        }
                    }
                }
                if(heightStatus==0)
                {
                    if(needMoveAwayEnwmy==0)
                    {
                        vector<Node> path = m_PathSearch->createGraph(make_pair(uavStatus.nX,uavStatus.nY),\
                                                                      make_pair(m_uavTask[uavID].goalLocation.x,m_uavTask[uavID].goalLocation.y),\
                                                                      uavStatus.nZ,weUavObstaclePos);


                        if(path.size()>1)
                        {
                          auto p=path.begin();

                          p++;
                          Node node = *p;

                          m_uavTask[uavID].nextLocation.x = node.x;
                          m_uavTask[uavID].nextLocation.y = node.y;

                        }else if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)
                        {
                            if(m_uavTask[uavID].nextLocation.z < MaxFlyHeight)
                            {
                              //  cout<<"uav ID="<<uavID<<" ;no road find, the uav will fly up"<<endl;
                              m_uavTask[uavID].nextLocation.z ++;

                            }
                        }
                    }else if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)
                    {
                        if(m_uavTask[uavID].nextLocation.z < MaxFlyHeight)
                        {
                          //  cout<<"uav ID="<<uavID<<" ;no road find, the uav will fly up"<<endl;
                          m_uavTask[uavID].nextLocation.z ++;

                        }
                    }

                }

            }else{
                if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)
                {
                    if(m_uavTask[uavID].nextLocation.z < MaxFlyHeight)
                    {
                      m_uavTask[uavID].nextLocation.z ++;
                     //  cout<<"Z="<<m_uavTask[uavID].nextLocation.z<<endl;
                    }
                }
            }
        }

    }else if(DontMoveStatus == 0)
    {


        if(m_uavTask[uavID].goalLocation.x == uavStatus.nX && \
                m_uavTask[uavID].goalLocation.y == uavStatus.nY )
        {
            //cout<<"uav ID="<<uavID<<" ;goal Z="<<m_uavTask[uavID].goalLocation.z<<" ; now Z="<<uavStatus.nZ<<endl;
            if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)
            {
                if(uavStatus.nZ != m_uavTask[uavID].goalLocation.z)
                {
                    if(uavStatus.nZ > m_uavTask[uavID].goalLocation.z)
                    {
                        if(m_uavTask[uavID].nextLocation.z>=1)
                            m_uavTask[uavID].nextLocation.z--;
                    }
                    if(uavStatus.nZ < m_uavTask[uavID].goalLocation.z)
                    {
                        if(m_uavTask[uavID].nextLocation.z<MaxFlyHeight)
                            m_uavTask[uavID].nextLocation.z++;
                    }
                }
            }

        }else{
            vector<pair<int, int>> obstaclePos;
            if(uavStatus.nZ>=minFlyHeight)//jisuan lujing
            {
                if(needMoveAwayEnwmy==0)
                {

                    vector<Node> path = m_PathSearch->createGraph(make_pair(uavStatus.nX,uavStatus.nY),\
                                                                  make_pair(m_uavTask[uavID].goalLocation.x,m_uavTask[uavID].goalLocation.y),\
                                                                  uavStatus.nZ,weUavObstaclePos);


                    if(path.size()>1)
                    {
                      auto p=path.begin();

                      p++;
                      Node node = *p;

                      m_uavTask[uavID].nextLocation.x = node.x;
                      m_uavTask[uavID].nextLocation.y = node.y;

                    }else if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)
                    {
                        if(m_uavTask[uavID].nextLocation.z < MaxFlyHeight)
                        {
                           // cout<<"uav ID="<<uavID<<" ;no road find, the uav will fly up"<<endl;
                          m_uavTask[uavID].nextLocation.z ++;

                        }
                    }
                }else if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)
                {
                    if(m_uavTask[uavID].nextLocation.z < MaxFlyHeight)
                    {
                      //  cout<<"uav ID="<<uavID<<" ;no road find, the uav will fly up"<<endl;
                      m_uavTask[uavID].nextLocation.z ++;

                    }
                }

            }else{
                if(isUavInHome(uavStatus.nX,uavStatus.nY)!=1)
                {
                    if(m_uavTask[uavID].nextLocation.z < MaxFlyHeight)
                    {
                      m_uavTask[uavID].nextLocation.z ++;
                     //  cout<<"Z="<<m_uavTask[uavID].nextLocation.z<<endl;
                    }
                }
            }
        }
    }

    m_weUavNextPiont[uavID] = m_uavTask[uavID].nextLocation;//更新我方无人机下一步的位置
}

void UAV_TASK::uavTaskInIDEL(int uavID, UAV uavStatus)
{

    //空闲任务下，没有分配到任务，判断电量是否充满
    if(uavStatus.remainElectricity >= m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity)//have full power
    {//飞机满电，要么出去取货，要么安排随机地址
        m_uavTask[uavID].taskState = UAV_STATE_RAND;
    }

    if(m_uavTask[uavID].taskState != UAV_STATE_CHARGE && m_uavTask[uavID].taskState != UAV_STATE_BACK_CHARGE)
    {//飞机不是满电，且没有收到任务，进入充电模式
     //todo... 加入空闲时间判断，一段时间内没有收到任务，也回家

        float uavRemainStep = (float)uavStatus.remainElectricity/(float)uavStatus.nLoadWeight;
        if(uavRemainStep < (minFlyHeight*2))//电量不够支撑一上一下，进入充电模式
            m_uavTask[uavID].taskState = UAV_STATE_BACK_CHARGE;

        if(uavStatus.remainElectricity < m_mapCreate->getPlaneUavPrice(uavStatus.nLoadWeight).capacity/2)
        {
            m_uavTask[uavID].taskInIdelTime++;//时间判断，超时
            if(m_uavTask[uavID].taskInIdelTime>minFlyHeight)
            {
                m_uavTask[uavID].taskInIdelTime =0;
                m_uavTask[uavID].taskState = UAV_STATE_BACK_CHARGE;
            }
        }
    }

	if(m_uavTask[uavID].taskState == UAV_STATE_CHARGE)
	{
        m_uavTask[uavID].goalLocation.x = weHomeX;//目标点的坐标
        m_uavTask[uavID].goalLocation.y = weHomeY;
        m_uavTask[uavID].goalLocation.z = 0;

	}
    if(m_uavTask[uavID].taskState == UAV_STATE_BACK_CHARGE)
    {
        m_uavTask[uavID].goalLocation.x = weHomeX;//目标点的坐标
        m_uavTask[uavID].goalLocation.y = weHomeY;
        m_uavTask[uavID].goalLocation.z = minFlyHeight+1;

    }

    if(m_uavTask[uavID].taskState == UAV_STATE_RAND)//分配随机位置
    {//todo... 随机位置分配，需要优化,
        //todo ... 6.8:需要判断随机位置是否已经分配给其他飞机了
        //if(m_uavTask[uavID].randLocationState == UAV_NO_RAND_ROAD)
        if(isUavInHome(uavStatus.nX, uavStatus.nY) == 1)
        {
            int randX=0;
            int randY=0;
            int randZ=minFlyHeight;

            do
            {
                srand((int)m_runTime);
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
            }while(m_mapCreate->get_mappoint(randX,randY,randZ)==1);

        }else
        {
            if(uavStatus.nZ<minFlyHeight)
                m_uavTask[uavID].goalLocation.z = minFlyHeight - 1;
        }
        // cout<<"Rand Location, uav ID="<<uavID<<"; rand :x="<<m_uavTask[uavID].goalLocation.x<<";y="<<m_uavTask[uavID].goalLocation.y<<endl;

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

            //cout<<"goodsID="<<m_uavTask[uavID].goodsNo<<"; goods Start :x="<<m_uavTask[uavID].goalLocation.x<<";y="<<m_uavTask[uavID].goalLocation.y<<endl;
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
           // cout<<"goods End :x="<<m_uavTask[uavID].goalLocation.x<<";y="<<m_uavTask[uavID].goalLocation.y<<endl;
        }
    }

}



void UAV_TASK::uavTaskAssignGoods(int uavID, UAV uavStatus)
{
    map<float, int> goodsCost;
    goodsCost.clear();

    if(uavStatus.nZ >= minFlyHeight-1)
    {
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
                //cout<<"uavID:"<<uavID<<"; find the goods to get=:"<<uavGoodsId<<endl;
                m_uavGoodsID.insert(pair<int,int>(uavGoodsId,uavID));
                m_uavTask[uavID].taskClass = UAV_TASK_GOODS;
                m_uavTask[uavID].taskState = UAV_GOOD_TO_GET;
                m_uavTask[uavID].goodsNo   = uavGoodsId;
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
                            m_uavTask[uavID].taskState = UAV_STATE_RAND;
                            m_uavTask[uavID].enemyNo   = uavEnemyId;
                            break;
                        }
                    }
                }
            }
        }//优先攻击机器规划完成
        if(m_uavTask[uavID].taskClass == UAV_TASK_IDEL)//空闲任务下，规划任务
        {
            if(m_uavTask[uavID].uavHomeStatus != UAV_IN_BACK_QUEUE)// 飞机不在回家队列中
                uavTaskAssignGoods(uavID,uavStatus);//调度取货算法，任务切换；如果不满足条件，继续执行IDEL任务
        }
    }//空闲任务
    else if(m_uavTask[uavID].taskClass == UAV_TASK_GOODS)//飞机的当前任务为取货状态
    {//todo ... 判断当前货物是否有效，是否有敌方飞机去取货,如果到达取货点判断，在取货点以下的敌方飞机是否比我方值钱

        if(m_uavTask[uavID].taskState == UAV_GOOD_TO_GET)//本机器的状态为取货中，还未取到货
        {
            map<int,GOODS>::iterator its;
            its=m_Goods.find(m_uavTask[uavID].goodsNo);//判断当前ID 号对应的货物是否存在

            if(its==m_Goods.end())//货物不存在
            {
               // cout<<"goods is noon"<<endl;
                m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
                m_uavTask[uavID].taskState = UAV_STATE_RAND;
                uavTaskAssignGoods(uavID,uavStatus);//调度取货算法，任务切换；如果不满足条件，继续执行IDEL任务
            }else{//当前ID号存在

                GOODS goodsStatus = m_Goods[m_uavTask[uavID].goodsNo];
             //   cout<<"goodsStatus="<<goodsStatus.nState;

                if(goodsStatus.nState !=0|| goodsStatus.nLeftTime == 0)//当前货物已经被取走,或者剩余时间为0
                {
             //       cout<<"goods is noon"<<endl;

                    int isTrack=0;
                    if(uavStatus.nX == goodsStatus.nStartX && uavStatus.nY == goodsStatus.nStartY)
                    {
                        if(uavStatus.nZ <= minFlyHeight)
                        {
                            for(map<int,uavCoord_t>::iterator it= m_enemyUavNowPiont.begin(); it!= m_enemyUavNowPiont.end(); it++)//扁历存在的ID
                            {
                                int enemyUavIDTemp      = it->first;
                                uavCoord_t enemyUavTempCoord   = it->second;
                                if((enemyUavTempCoord.x == goodsStatus.nStartX)&&(enemyUavTempCoord.y == goodsStatus.nStartY))
                                {
                                    if(enemyUavTempCoord.z<uavStatus.nZ)//敌方飞机在我方下方
                                    {
                                        map<int,int>::iterator its;
                                        its=m_uavTrackID.find(enemyUavIDTemp);//判断敌方飞机是否根我方攻击飞机关联上了

                                        if(its==m_uavTrackID.end())//如果没有关联上，
                                        {
                                            isTrack=1;
                                        }

                                        break;


                                    }
                                }
                            }
                        }
                    }

                    if(isTrack==0)
                    {
                        m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
                        m_uavTask[uavID].taskState = UAV_STATE_RAND;
                        uavTaskAssignGoods(uavID,uavStatus);//调度取货算法，任务切换；如果不满足条件，继续执行IDEL任务
                    }


                }else if(goodsStatus.nState ==0){//当前货物还在，
                    //todo... 需要判断货物处有无敌方无人机，如果敌方无人机正在去取货，调整相应的策略
                    for(map<int,uavCoord_t>::iterator it= m_enemyUavNowPiont.begin(); it!= m_enemyUavNowPiont.end(); it++)//扁历存在的ID
                    {
                        int enemyUavIDTemp      = it->first;
                        uavCoord_t enemyUavTempCoord   = it->second;
                        if((enemyUavTempCoord.x == goodsStatus.nStartX)&&(enemyUavTempCoord.y == goodsStatus.nStartY))//敌方无人机去取货了
                        {
                            if(enemyUavTempCoord.z<minFlyHeight)
                            {
                                if((uavStatus.nZ > enemyUavTempCoord.z )&& (uavStatus.nLoadWeight>m_enemyUavID[enemyUavIDTemp].nLoadWeight)\
                                        &&(uavStatus.nZ> minFlyHeight/2))
                                {
                                    m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
                                    m_uavTask[uavID].taskState = UAV_STATE_RAND;
                                    uavTaskAssignGoods(uavID,uavStatus);//调度取货算法，任务切换；如果不满足条件，继续执行IDEL任务
                                    break;
                                }
                            }
                        }
                    }
                }
            }

        }
    }
    else if(m_uavTask[uavID].taskClass == UAV_TASK_TRACK)//飞机的当前任务为攻击状态
    {//todo ...
        //同上，使用敌方无人机ID时候，需要判断是否存在

    }
}


void UAV_TASK::uavTaskTrackEnemy(int uavID, UAV uavStatus)//计算被跟踪无人机的目标点
{
    int enemyIdTemp = m_uavTask[uavID].enemyNo;

    if(enemyIdTemp >=0)
    {
        UAV enemyUavStatus = m_enemyUavID[enemyIdTemp];

        if(enemyUavStatus.nGoodsNo == -1)//敌方无人机没有取货
        {
            if(enemyUavStatus.nStatus!=UAV_CRASH)
            {
                if(enemyUavStatus.nZ !=-1)
                {
                    m_uavTask[uavID].goalLocation.x = enemyUavStatus.nX;//
                    m_uavTask[uavID].goalLocation.y = enemyUavStatus.nY;
                    m_uavTask[uavID].goalLocation.z = enemyUavStatus.nZ;
                }
            }
        }else//敌方无人机已经取到货物
        {
            if(enemyUavStatus.nZ !=-1)//敌方无人机数据有效
            {
                if(enemyUavStatus.nZ < minFlyHeight)//
                {
                    if(uavStatus.nX == enemyUavStatus.nX && uavStatus.nY == enemyUavStatus.nY)
                    {
                        m_uavTask[uavID].goalLocation.x = enemyUavStatus.nX;//
                        m_uavTask[uavID].goalLocation.y = enemyUavStatus.nY;
                        m_uavTask[uavID].goalLocation.z = enemyUavStatus.nZ;
                    }
                }else{//敌方无人机，水平方向已经可以移动
                    int enemyGoodsNo = enemyUavStatus.nGoodsNo;
                    GOODS enemyGoodsStatus = m_Goods[enemyGoodsNo];

                    m_uavTask[uavID].goalLocation.x = enemyGoodsStatus.nEndX;//
                    m_uavTask[uavID].goalLocation.y = enemyGoodsStatus.nEndY;
                    m_uavTask[uavID].goalLocation.z = 1;
                }
            }else if(enemyUavStatus.nGoodsNo == -1)
            {
                int enemyGoodsNo = enemyUavStatus.nGoodsNo;

                map<int,GOODS>::iterator its;
                its=m_Goods.find(enemyGoodsNo);//判断当前ID 号对应的货物是否存在

                if(its!=m_Goods.end())//货物存在
                {
                    GOODS enemyGoodsStatus = m_Goods[enemyGoodsNo];

                    m_uavTask[uavID].goalLocation.x = enemyGoodsStatus.nEndX;//
                    m_uavTask[uavID].goalLocation.y = enemyGoodsStatus.nEndY;
                    m_uavTask[uavID].goalLocation.z = 1;
                }
            }
        }

    }/*else
    {
        m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
        m_uavTask[uavID].taskState = UAV_STATE_RAND;
    }*/
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
        m_UavTaskTemp.uavHomeStatus = UAV_NOT_IN_QUEUE;
        m_UavTaskTemp.randLocationState = UAV_NO_RAND_ROAD;

        if(enemyUavHomeX!=-1)
            m_UavTaskTemp.goalLocation.x      =  enemyUavHomeX;
        else
            m_UavTaskTemp.goalLocation.x      =  mapXsize/3;
        if(enemyUavHomeY!=-1)
            m_UavTaskTemp.goalLocation.y      =  enemyUavHomeY;
        else
            m_UavTaskTemp.goalLocation.y      =  mapYsize/3;

        m_UavTaskTemp.goalLocation.z      =  minFlyHeight;

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
            uavTaskTrackEnemy(uavID,uavStatus);
			break;
		
		default:

            m_uavTask[uavID].taskClass = UAV_TASK_IDEL;
            m_uavTask[uavID].taskState = UAV_STATE_CHARGE;
            m_uavTask[uavID].enemyNo   = -1;
            m_uavTask[uavID].goodsNo   = -1;
            m_uavTask[uavID].taskInIdelTime = 0;
            m_uavTask[uavID].uavHomeStatus = UAV_NOT_IN_QUEUE;
            m_uavTask[uavID].randLocationState = UAV_NO_RAND_ROAD;
            cout<< "there has none TASK CLASS, maybe has problem!!! ="<<m_uavTask[uavID].taskClass<<" ;----"<<endl;
			break;
	}

    uavRun(uavID,uavStatus);
    //uavChargeProcess(uavID,uavStatus);

	
}


void UAV_TASK::uavChargeProcess(int uavID, UAV uavStatus)//无人机充电
{
    if( isUavInHome(uavStatus.nX,uavStatus.nY)==1 &&\
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

void UAV_TASK::uavPurchase(void)
{
    if(m_weMoney > m_cheapestUavPrice.nValue )
    {
        m_pstFlayPlane->nPurchaseNum = 1;
        strcpy(m_pstFlayPlane->szPurchaseType[0],m_cheapestUavPrice.szType);
    }
}


void UAV_TASK::uavTaskProcess(MATCH_STATUS * pstMatch)
{
    int trackCycle=1;

    updateUavStatus(pstMatch);//更新飞行器状态


    while(trackCycle ==1)
    {
        trackCycle=0;
        for(map<int,int>::iterator it= m_uavTrackID.begin(); it!= m_uavTrackID.end(); it++)//判断跟踪列表中，敌方无人机的ID 号是否存在
        {
            int uavEnemyId = it->first;
            int uavWeIdTemp    = it->second;

            map<int,UAV>::iterator its,weIts;
            its=m_enemyUavID.find(uavEnemyId);//判断列表中的敌方飞机的ID是否存在
            if(its==m_enemyUavID.end())//敌方无人机已经坠毁
            {
                trackCycle = 1;
                //释放我方跟踪无人机
                weIts = m_weUavID.find(uavWeIdTemp);
                if(weIts!=m_weUavID.end()) //我方无人机存在，清除任务
                {
                    m_uavTask[uavWeIdTemp].taskClass = UAV_TASK_IDEL;
                    m_uavTask[uavWeIdTemp].enemyNo   = -1;
                }

                m_uavTrackID.erase(it);
                break;
            }else//敌方无人机存在，需要判断我方无人机是否存在
            {
                weIts = m_weUavID.find(uavWeIdTemp);
                if(weIts==m_weUavID.end()) //我方无人机不存在，清除关联
                {
                    trackCycle = 1;
                    m_uavTrackID.erase(it);
                    break;
                }
            }
        }
    }


	for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID 
	{

        int uavIdTemp = it->first;
        UAV uavStatusTemp = it->second;

        m_pstFlayPlane->astUav[m_uavPlanID[uavIdTemp]] = uavStatusTemp;

        uavTaskAllot(uavIdTemp,uavStatusTemp);


	}


    vector<UAV>::iterator it;
    int uavOutHomeFlyMaxHeight = MaxFlyHeight;


    cout<<"uavOutHomeQueue="<<uavOutHomeQueue.size()<<" ;uavBackHomeQueue="<<uavBackHomeQueue.size()<<" ; "<<endl;
//    if(!uavOutHomeQueue.empty() && !uavBackHomeQueue.empty())
//    {
//        //todo ... when go in this funtion , show that there maybe happened some problems
//        UAV uavStatusTemp;
//        sort(uavBackHomeQueue.begin(),uavBackHomeQueue.end(),sortFromMaxToMin);//按Z从大到小排序

//        uavStatusTemp = uavOutHomeQueue[0];//出家无人机的最高高度
//        int uavOutHeight = uavStatusTemp.nZ;

//        uavStatusTemp = *prev(uavBackHomeQueue.end());
//        int uavInHeight = uavStatusTemp.nZ;

//        for(it=uavBackHomeQueue.begin();it!=uavBackHomeQueue.end();it++)
//        {

//            uavStatusTemp = *it;

//            map<int,UAV>::iterator its;
//            its=m_weUavID.find(uavStatusTemp.nNO);
//            if(its==m_weUavID.end())
//            {
//                uavBackHomeQueue.erase(it);
//                //cout<<"  Clear Uav in Home BACK queue------";
//                break;
//            }else{
//                if(m_weUavID[uavStatusTemp.nNO].nZ <MaxFlyHeight)
//                {
//                    m_uavTask[uavStatusTemp.nNO].nextLocation.z++;
//                    cout<<"  ID="<<uavStatusTemp.nNO<<"; Now Z:="<<m_weUavID[uavStatusTemp.nNO].nZ <<"; Next Z="<<m_uavTask[uavStatusTemp.nNO].nextLocation.z;
//                    cout<<"  here back uav fly up ok";
//    //                m_pstFlayPlane->astUav[m_uavPlanID[uavStatusTemp.nNO]].nZ++ ;
//                }else{
//                    break;
//                }

//                if(m_weUavID[uavStatusTemp.nNO].nZ == 0)
//                {
//                   break;
//                }

//            }
//        }
//        uavStatusTemp = *prev(uavBackHomeQueue.end());
//        uavOutHomeFlyMaxHeight = m_uavTask[uavStatusTemp.nNO].nextLocation.z-1;

//    }//else

    uavOutHomeFlyMaxHeight = minFlyHeight;
    {
        if(!uavOutHomeQueue.empty())//出家队列,优先出家
        {

            for(it=uavOutHomeQueue.begin();it!=uavOutHomeQueue.end();it++)
            {
                UAV uavStatusTemp;
                uavStatusTemp = *it;

                map<int,UAV>::iterator its;
                its=m_weUavID.find(uavStatusTemp.nNO);
                if(its==m_weUavID.end())
                {
                    uavOutHomeQueue.erase(it);
                    cout<<"Clear Uav in Home ------"<<endl;
                    break;
                }else{

                    if(m_weUavID[uavStatusTemp.nNO].nZ < uavOutHomeFlyMaxHeight)
                    {
                        m_uavTask[uavStatusTemp.nNO].nextLocation.z++;
                    //    cout<<"ID="<<uavStatusTemp.nNO<<"; Now Z:="<<m_weUavID[uavStatusTemp.nNO].nZ <<"; Next Z="<<m_uavTask[uavStatusTemp.nNO].nextLocation.z<<endl;

                    }else{
                        //增加判断当前点是否要离开，离开的话，让下点上升 ok
                        if(m_uavTask[uavStatusTemp.nNO].nextLocation.x == weHomeX && m_uavTask[uavStatusTemp.nNO].nextLocation.y ==weHomeY)
                            break;
                    }

                    if(m_weUavID[uavStatusTemp.nNO].nZ == 0)
                    {
                       break;
                    }
                }
            }
        }else if(!uavBackHomeQueue.empty())//进家队列
        {
            sort(uavBackHomeQueue.begin(),uavBackHomeQueue.end(),sortFromMinToMax);//整理，按Z从小到大排序

            for(it=uavBackHomeQueue.begin();it!=uavBackHomeQueue.end();it++)
            {
                UAV uavStatusTemp;
                uavStatusTemp = *it;

                map<int,UAV>::iterator its;
                its=m_weUavID.find(uavStatusTemp.nNO);
                if(its==m_weUavID.end())
                {
                    uavBackHomeQueue.erase(it);

                    break;
                }else{

                    //cout<<"uav ID in uavBackHomeQueue:= "<<uavStatusTemp.nNO<<" ;"<<"m_weUav.nZ= "<<m_weUavID[uavStatusTemp.nNO].nZ<<" ;";

                    if(m_weUavID[uavStatusTemp.nNO].nZ >0)
                    {
                        m_uavTask[uavStatusTemp.nNO].nextLocation.z--;

                    }else{
                        m_uavTask[uavStatusTemp.nNO].taskState = UAV_STATE_CHARGE;

                        if(m_uavTask[uavStatusTemp.nNO].uavHomeStatus == UAV_IN_BACK_QUEUE)
                        {
                            m_uavTask[uavStatusTemp.nNO].uavHomeStatus = UAV_NOT_IN_QUEUE;
                            vector<UAV>::iterator it;

                            for(it=uavBackHomeQueue.begin();it!=uavBackHomeQueue.end();it++)
                            {
                                UAV uavTemp = *it;
                                if(uavTemp.nNO == uavStatusTemp.nNO)
                                {
                                  //  cout<<"erase ok uav ID ="<<uavTemp.nNO <<endl;
                                    uavBackHomeQueue.erase(it);
                                    break;
                                }
                            }
                        }

                        break;
                    }

                    if(m_weUavID[uavStatusTemp.nNO].nZ == 0)
                    {
                      // break;
                    }
                }
            }
        }
    }

    for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID
    {
        int uavIdNow     = it->first;
        //UAV uavStatusNow = it->second;

        uavTask_t uavTaskNow = m_uavTask[uavIdNow];

        if(uavTaskNow.taskClass == UAV_TASK_TRACK)
        {
            if((uavTaskNow.enemyNo != -1)&& (uavTaskNow.nowLocation.x != weHomeX)&&(uavTaskNow.nowLocation.y != weHomeY))
            {
                if(uavTaskNow.nextLocation.z<uavTaskNow.nowLocation.z)//biaoshi move down
                {
                    for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID
                    {
                        int uavIdCycle     = it->first;
                        //UAV uavStatusCyCle = it->second;
                        uavTask_t uavTaskCycle = m_uavTask[uavIdCycle];
                        if(uavIdCycle!=uavIdNow)
                        {
                            if((uavTaskNow.nextLocation.x == uavTaskCycle.nextLocation.x)&& (uavTaskNow.nextLocation.y == uavTaskCycle.nextLocation.y))
                            {
                                if(uavTaskNow.nowLocation.z > uavTaskCycle.nowLocation.z )
                                {
                                    if(uavTaskNow.nextLocation.z <= uavTaskCycle.nextLocation.z)
                                    {
                                        if(uavTaskNow.nowLocation.z<MaxFlyHeight)
                                        {
                                            m_uavTask[uavIdNow].nextLocation.z = uavTaskNow.nowLocation.z+1;
                                        }else
                                            m_uavTask[uavIdNow].nextLocation.z = uavTaskNow.nowLocation.z;
                                        m_uavTask[uavIdNow].nextLocation.x = uavTaskNow.nowLocation.x;
                                        m_uavTask[uavIdNow].nextLocation.y = uavTaskNow.nowLocation.y;

                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for(map<int,UAV>::iterator it= m_weUavID.begin(); it!= m_weUavID.end(); it++)//扁历存在的ID
    {
        int uavIdTemp     = it->first;
        UAV uavStatusTemp = it->second;

        if(m_uavTask[uavIdTemp].nextLocation.z != uavStatusTemp.nZ)
        {
            if(uavStatusTemp.nZ < minFlyHeight)
            {
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nX = uavStatusTemp.nX;
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nY = uavStatusTemp.nY;
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nZ = m_uavTask[uavIdTemp].nextLocation.z;
            }else if(m_uavTask[uavIdTemp].nextLocation.x != uavStatusTemp.nX || m_uavTask[uavIdTemp].nextLocation.y != uavStatusTemp.nY)
            {
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nX = m_uavTask[uavIdTemp].nextLocation.x;
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nY = m_uavTask[uavIdTemp].nextLocation.y;
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nZ = uavStatusTemp.nZ;
            }else
            {
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nX = m_uavTask[uavIdTemp].nextLocation.x;
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nY = m_uavTask[uavIdTemp].nextLocation.y;
                m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nZ = m_uavTask[uavIdTemp].nextLocation.z;
            }

        }else{
            m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nX = m_uavTask[uavIdTemp].nextLocation.x;
            m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nY = m_uavTask[uavIdTemp].nextLocation.y;
            m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nZ = m_uavTask[uavIdTemp].nextLocation.z;
        }

        uavChargeProcess(it->first,it->second);


        if(m_uavTask[uavIdTemp].taskClass == UAV_TASK_GOODS)
        {
            GOODS goodsStatus = m_Goods[m_uavTask[uavIdTemp].goodsNo];
            if(m_uavTask[uavIdTemp].taskState == UAV_GOOD_TO_GET)
            {
                if(goodsStatus.nStartX == uavStatusTemp.nX && goodsStatus.nStartY == uavStatusTemp.nY&&\
                        uavStatusTemp.nZ == 1 &&m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nZ==0)
                {
                    m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nGoodsNo = m_uavTask[uavIdTemp].goodsNo;
                    m_pstFlayPlane->astUav[m_uavPlanID[it->first]].remainElectricity = uavStatusTemp.remainElectricity - goodsStatus.nWeight;
                    m_uavTask[uavIdTemp].taskState = UAV_GOOD_TO_PUT;

                   // cout<<"uavId="<<uavIdTemp<<" ;remainElectricity"<<m_pstFlayPlane->astUav[m_uavPlanID[it->first]].remainElectricity<<endl;
                }

            }else if(m_uavTask[uavIdTemp].taskState == UAV_GOOD_TO_PUT)
            {
                if(goodsStatus.nEndX == uavStatusTemp.nX && goodsStatus.nEndY == uavStatusTemp.nY&&\
                        uavStatusTemp.nZ == 0 &&m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nZ==1)
                {

                }
                if(uavStatusTemp.nGoodsNo!=-1)
                    m_pstFlayPlane->astUav[m_uavPlanID[it->first]].remainElectricity = uavStatusTemp.remainElectricity - goodsStatus.nWeight;
                //cout<<"uavId="<<uavIdTemp<<" ;remainElectricity"<<m_pstFlayPlane->astUav[m_uavPlanID[it->first]].remainElectricity<<endl;
               // cout<<"next Z="<<m_pstFlayPlane->astUav[m_uavPlanID[it->first]].nZ<<" ;uavStatusTemp.nZ="<< uavStatusTemp.nZ<<endl;
            }
        }

        if(m_pstFlayPlane->astUav[m_uavPlanID[it->first]].remainElectricity<0)//临界植判断
            m_pstFlayPlane->astUav[m_uavPlanID[it->first]].remainElectricity=0;
        //cout<<"uavId="<<uavIdTemp<<" ;remainElectricity"<<m_pstFlayPlane->astUav[m_uavPlanID[it->first]].remainElectricity<<endl;



    }



    uavPurchase();

    //todo ... 在发送给服务器数据之前，做最后的数据校验，判断数据是否越界、不符合规范




    //cout<<"end ok"<<endl;
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








