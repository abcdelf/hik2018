#include "matchState.h"


using namespace std;

MATCHSTATE::MATCHSTATE()
{
  isJustStart=-1;
}

/**
 * @brief 更新3个map
 */
void MATCHSTATE::renewMatchstate(MATCH_STATUS * pstMatch)
{
	mpstMatch=pstMatch;
	
	m_weUavID.clear();
	m_weUav.clear();

	for(int i=0;i<mpstMatch->nUavWeNum;i++)
	{
		if(mpstMatch->astWeUav[i].nStatus != UAV_CRASH)//飞机有效
		{
			m_weUav.insert(pair<int,UAV> (i,mpstMatch->astWeUav[i]));
			m_weUavID.insert(pair<int,UAV> (mpstMatch->astWeUav[i].nNO,mpstMatch->astWeUav[i]));
		}
			
	}
	
    //cout<<"WeUavNUM"<<m_weUav.size()<<endl;

	enemyUav.clear();
	for(int i=0;i<mpstMatch->nUavEnemyNum;i++)
	{
		enemyUav.insert(pair<int,UAV> (mpstMatch->astEnemyUav[i].nNO,mpstMatch->astEnemyUav[i]));
	}
	
	Goods.clear();
	for(int i=0;i<mpstMatch->nGoodsNum;i++)
	{
		Goods.insert(pair<int,GOODS> (mpstMatch->astGoods[i].nNO,mpstMatch->astGoods[i]));
	}	  
}

/**
 * @brief 获取weUavId的我方飞机的结构体
 */
UAV MATCHSTATE::pickWeUavFromID(int weUavId)
{
	map<int,UAV>::iterator it;
	it = m_weUavID.find(weUavId);	
	if(it== m_weUavID.end())
	{
		cout<<"weUavId not found"<<endl;
	}
	else	  
	{
	    cout<<"nNO "<<it->second.nNO<<" found"<<endl; 
		return it->second;
	}
}

/**
 * @brief 获取weUavId的我方飞机的结构体
 */
UAV MATCHSTATE::pickWeUavFromNum(int num)
{
	map<int,UAV>::iterator it;
	it = m_weUav.find(num);	
	if(it== m_weUav.end())
	{
		cout<<"weUavId not found"<<endl;
	}
	else	  
	{
	    cout<<"nNO "<<it->second.nNO<<" found"<<endl; 
		return it->second;
	}
}

/**
 * @brief 获取enemyUavId的敌方飞机的结构体
 */
UAV MATCHSTATE::pickEnemyUavFromID(int enemyUavId)
{
	map<int,UAV>::iterator it;
	it=enemyUav.find(enemyUavId);	
	if(it==enemyUav.end())
	{
		cout<<"enemyUavId not found"<<endl;
	}
	else	  
	{
	        cout<<"nNO "<<it->second.nNO<<" found"<<endl; 
		return it->second;
	}
}

/**
 * @brief 获取goodno为goodsId的货物的结构体
 */
GOODS  MATCHSTATE::pickGoodsFromID(int goodsId)
{
	map<int,GOODS>::iterator it;
	it=Goods.find(goodsId);	
	if(it==Goods.end())
	{
		cout<<"goodsId not found"<<endl;
	}
	else	  
	{
	        cout<<"nNO "<<it->second.nNO<<" found"<<endl; 
		return it->second;
	}
}

/**
 * @brief 找到敌方飞机的老巢
 */
void MATCHSTATE::findUavEnemyHome(void)//isJustStart = 0;
{
  int initState=0;
  if(isJustStart == -1)
  {
    if(mpstMatch->nUavEnemyNum >=2 )
    {
      if( (mpstMatch->astEnemyUav[0].nX== mpstMatch->astEnemyUav[1].nX)&&\
          (mpstMatch->astEnemyUav[0].nY== mpstMatch->astEnemyUav[1].nY)&&\
          (mpstMatch->astEnemyUav[0].nZ== mpstMatch->astEnemyUav[1].nZ)&&\
          (UAV_CRASH != mpstMatch->astEnemyUav[0].nStatus) && (mpstMatch->astEnemyUav[1].nStatus!=UAV_CRASH))
      {
        initState=1;
      }
    }
    if(initState==1)
    {
		isJustStart = 0;
		uavEnemyHome=make_pair(mpstMatch->astEnemyUav[0].nX,mpstMatch->astEnemyUav[0].nY);
		cout<<"enemyHomeX="<<this->getUavEnemyHome().first<<" ; enemyHomeY="<<this->getUavEnemyHome().second<<endl;

    }
  }
}
