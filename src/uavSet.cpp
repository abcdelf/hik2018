/**	@file		uavset.cpp
 *	@note		gongchengdaxue.
 *	@brief		set the uavstep
 *
 *	@author		huangjingwei
 *	@date		2018/5/24
 *	@note		历史记录：
 *	@note		V1.0.0  
 *	@warning	
 */
#include "include/uavSet.h"
//#include <boost/concept_check.hpp>
#define MAX_PLANE_NUM 50

PLANE::PLANE()
{
}

PLANE::PLANE(MAP* map,FLAY_PLANE* pstFlayPlane,pathSearch* roadSearch ,MATCHSTATUS* matchstatus):\
        mmap(map),mpstFlayPlane(pstFlayPlane),mpathSearch(roadSearch),mmatchStatus(matchstatus)
{
    for(int i=0;i<MAX_PLANE_NUM;i++)
      PlanSetate.push_back(-1);
    for(int i=0; i<1000; i++)
    {
      enemyLastState[i].enemyId    = -1;
      enemyLastState[i].goodsId    = -1;
      enemyLastState[i].trackState = -1;
      enemyLastState[i].coord      = make_pair(-1,-1);      
    }
}

void PLANE::plane_init(void)  //flag:0=back_search,flag=1:back_trans
{

  int planeInHomeId[100];
  int planeInHomeHeight[100];

  int planeInHomeNum=0;
  int maxHigh = mmap->getMaxFlyHeight();
  int minHigh = mmap->getMinFlyHeight();

  int planeNowMinHigh=maxHigh;

  for(int i=0; i< mpstMatch->nUavWeNum; i++)//寻找下一步计划会路过家位置的飞机
  {
    if(mpstMatch->astWeUav[i].nStatus != UAV_CRASH)//飞机有效
    {
      if(mpstFlayPlane->astUav[i].nX == mmatchStatus->getHomeX()&&\
         mpstFlayPlane->astUav[i].nY == mmatchStatus->getHomeY())//下一秒会出现在家的位置上 
      {
        if( mpstFlayPlane->astUav[i].nX != mpstMatch->astWeUav[i].nX ||\
            mpstFlayPlane->astUav[i].nY != mpstMatch->astWeUav[i].nY)//飞机原来不在家的位置上
        {
          planeInHomeId[planeInHomeNum]=i;//规划路过的飞机ID
          planeInHomeNum++;
        }
        // if(mpstFlayPlane->astUav[i].nZ >= mmap->getMinFlyHeight())
        // {
        //   planeInHomeId[planeInHomeNum]=i;
        //   planeInHomeNum++;
        // }
      }
    }
  }
  for(int j=0; j<planeInHomeNum; j++)
  {
    if(planeNowMinHigh > mpstFlayPlane->astUav[planeInHomeId[j]].nZ)
    {
      planeNowMinHigh = mpstFlayPlane->astUav[planeInHomeId[j]].nZ-1;//得到可以飞的最低高度
    }
  }

  planeInHomeNum=0;
  for(int i=0; i< mpstMatch->nUavWeNum; i++)//寻找家位置的飞机
  {
    if(mpstMatch->astWeUav[i].nStatus != UAV_CRASH)//飞机有效
    {
      if(mpstMatch->astWeUav[i].nX == mmatchStatus->getHomeX()&&\
         mpstMatch->astWeUav[i].nY == mmatchStatus->getHomeY()&&\
         mpstMatch->astWeUav[i].nX == mpstFlayPlane->astUav[i].nX&&\
         mpstMatch->astWeUav[i].nY == mpstFlayPlane->astUav[i].nY   )//原来就在家的位置上 
      {
        mpstFlayPlane->astUav[i].nGoodsNo=-1;
        planeInHomeId[planeInHomeNum]=i;//在家位置上的飞机ID
        planeInHomeHeight[planeInHomeNum] = mpstMatch->astWeUav[i].nZ;
        planeInHomeNum++;//在家位置的飞机数量
      }
    }
  }

    int n = planeInHomeNum;
    for (int i = 0; i<n - 1; i++) 
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            //如果前面的数比后面xiao，进行交换
          if (planeInHomeHeight[j] < planeInHomeHeight[j + 1]) {
              int temp = planeInHomeHeight[j]; 
              planeInHomeHeight[j] = planeInHomeHeight[j + 1]; 
              planeInHomeHeight[j + 1] = temp;
          }
        }
    }  
    for(int i=0;i<n;i++)
    {
        printf("planeInHomeHeight=%d\n",planeInHomeHeight[i]);
    }
  if(planeInHomeHeight[0]==0)
  {
    mpstFlayPlane->astUav[0].nZ++;
  }else
  {
    for(int i=0; i<planeInHomeNum; i++)
    {
      
      for(int j=0; j<planeInHomeNum; j++)
      {
        if(planeInHomeHeight[i] == mpstMatch->astWeUav[planeInHomeId[j]].nZ )
        {
          if(planeInHomeHeight[i]<planeNowMinHigh)
          {
            mpstFlayPlane->astUav[planeInHomeId[j]].nZ++;
            planeNowMinHigh = mpstFlayPlane->astUav[planeInHomeId[j]].nZ;
            cout<<planeInHomeId[j]<<";"<<mpstFlayPlane->astUav[planeInHomeId[j]].nZ<<endl;
            break;
          }
        }
        //if(mpstMatch->astWeUav[planeInHomeId[j]].nZ == )
      }
      //mpstMatch->astWeUav[planeInHomeId[j]].nZ
    } 
  }


  
}

void PLANE::plane_up(int plane_num,int high,int flag)  //flag:0=back_search,flag=1:back_trans
{
    int step=0;
     if(flag)
    {
     while( mpstFlayPlane->astUav[plane_num].nZ<high)
    {
      mpstFlayPlane->astUav[plane_num].nZ+=1;
      break;
    }
	
    }
    else
    {
      while( mpstFlayPlane->astUav[plane_num].nZ<high)
      {
        for(int i;i<mpstFlayPlane->nUavNum;i++)
        {
          if(i==plane_num)
            continue;
          if(mpstFlayPlane->astUav[plane_num].nStatus == 0&&\
            mpstMatch->astWeUav[i].nStatus ==0)
          {
            if((mpstFlayPlane->astUav[plane_num].nZ+1)==(mpstFlayPlane->astUav[i].nZ)&&(mpstFlayPlane->astUav[plane_num].nX)==(mpstFlayPlane->astUav[i].nX)&&(mpstFlayPlane->astUav[plane_num].nY)==(mpstFlayPlane->astUav[i].nY))
            {
              step++;
            // break;
            }
          }

        }	
        if(!step)
          mpstFlayPlane->astUav[plane_num].nZ+=1;
          break;
      }
      mpstFlayPlane->astUav[plane_num].nGoodsNo=-1;
    }


}

void PLANE::plane_get(int plane_num,int high,int goods_no)
{
    if(goods_no==-1)
    return;
  int goods_num;
  for(goods_num=0;goods_num<mpstMatch->nGoodsNum;goods_num++)
  {
    if(goods_no==mpstMatch->astGoods[goods_num].nNO)
      break;	
  }
  int step=0;
  while( mpstFlayPlane->astUav[plane_num].nZ>high)
  {
    mpstFlayPlane->astUav[plane_num].nZ-=1;
    step++;
    break;
  }
  if(step==0&&mpstFlayPlane->astUav[plane_num].nZ==high)
  { 
    mpstFlayPlane->astUav[plane_num].nGoodsNo=goods_no;
  }
}
  
pair<int, int> PLANE::plane_search(int plane_num,int goods_no, vector<pair<int, int>> obstaclePos)
{
  int uavX = mpstMatch->astWeUav[plane_num].nX;
  int uavY = mpstMatch->astWeUav[plane_num].nY;
  int uavZ = mpstMatch->astWeUav[plane_num].nZ;
  int dist2HomeX=0;
  int dist2HomeY=0;

  if(goods_no==-1)
  {
    dist2HomeX = abs(mmatchStatus->getHomeX() - uavX);
    dist2HomeY = abs(mmatchStatus->getHomeY() - uavY);

    // if(dist2HomeX<5&&dist2HomeY<5)
    // {
    //   cout<<"0000000000000000--------------------------000000000"<<endl;

    //   // srand((unsigned int)plane_num);//设置当前时间为种子
    //   // int uavsX = rand()%10+1;//产生1~10的随机数
    //   // int uavsY = rand()%10+1;//产生1~10的随机数
    //   int uavsX = 5;
    //   int uavsY =5;
    //   vector<Node> path = mpathSearch->createGraph(make_pair(uavX,uavY),make_pair(uavsX,uavsY),uavZ,obstaclePos);
    //   int uavNextX=uavX;
    //   int uavNextY=uavY;
    //   int uavNextZ=uavZ;
      
    //   //mpathSearch->printPath(path);
    //   if(path.size()>1)
    //   {
    //     auto p=path.begin();
        
    //     p++;
    //     Node node = *p;

    //     uavNextX = node.x;
    //     uavNextY = node.y;
    //     uavNextZ = uavZ;
          
    //   }else if(uavZ < mmap->getMaxFlyHeight()-1)
    //   {
    //     uavNextZ = uavZ+1;

    //   }
    //   mpstFlayPlane->astUav[plane_num].nY = uavNextY;
    //   mpstFlayPlane->astUav[plane_num].nX = uavNextX;
    //   mpstFlayPlane->astUav[plane_num].nZ = uavNextZ;
    // cout<<"asadsdfaffffffffffffff--------------------------000000000"<<endl;
    //  return make_pair(uavNextX,uavNextY);

    // }else
    {


      if(uavZ< mmap->getMaxFlyHeight()-1)
      {
        for(int i=0; i<mpstMatch->nUavWeNum; i++)
        {
          if(mpstMatch->astWeUav[i].nStatus != UAV_CRASH)//飞机有效
          {
            if((uavZ+1) == mpstFlayPlane->astUav[i].nZ)
            {
              if(uavX == mpstFlayPlane->astUav[i].nX &&\
                 uavY == mpstFlayPlane->astUav[i].nY)
              {
                mpstFlayPlane->astUav[plane_num].nX = uavX;
                mpstFlayPlane->astUav[plane_num].nY = uavY;
                mpstFlayPlane->astUav[plane_num].nZ = uavZ;        
                return make_pair(uavX,uavY);         
              }else{
                mpstFlayPlane->astUav[plane_num].nX = uavX;
                mpstFlayPlane->astUav[plane_num].nY = uavY;
                mpstFlayPlane->astUav[plane_num].nZ = uavZ+1; 
                return make_pair(uavX,uavY);
              }
            }else{
              mpstFlayPlane->astUav[plane_num].nX = uavX;
              mpstFlayPlane->astUav[plane_num].nY = uavY;
              mpstFlayPlane->astUav[plane_num].nZ = uavZ+1; 
              return make_pair(uavX,uavY);
            }

          }
        }

      }else{

      }

      
    }
    return make_pair(uavX,uavY);
  }
    
  int goods_num;
  for(goods_num=0;goods_num<mpstMatch->nGoodsNum;goods_num++)
  {
    if(goods_no==mpstMatch->astGoods[goods_num].nNO)
      break;	  
  }
  

  int goodsXStart = mpstMatch->astGoods[goods_num].nStartX;
  int goodsYStart = mpstMatch->astGoods[goods_num].nStartY;

  for(int i=0; i<mpstMatch->nUavEnemyNum; i++)
  {
    if( mpstMatch->astEnemyUav[i].nX == goodsXStart&&\
        mpstMatch->astEnemyUav[i].nY == goodsYStart)
    {
      mmatchStatus->clearPlaneGoodsNum(plane_num) ; 
      if(uavZ< mmap->getMaxFlyHeight()-1)
      {
        mpstFlayPlane->astUav[plane_num].nX = uavX;
        mpstFlayPlane->astUav[plane_num].nY = uavY;
        mpstFlayPlane->astUav[plane_num].nZ = uavZ+1;  
      }else{

      }
      cout<<"clear goodsNUM"<<endl;
      return make_pair(uavX,uavY);
    }
  }
  printf("uavX=%3d,uavY=%3d,uavZ=%3d,goodsXStart=%d,goodsYStart=%d\r\n",uavX,uavY,uavZ,goodsXStart,goodsYStart);
  //mstar->findpath(uavX, uavY, uavZ, goodsXStart, goodsYStart,uavZ);
  vector<Node> path = mpathSearch->createGraph(make_pair(uavX,uavY),make_pair(goodsXStart,goodsYStart),uavZ,obstaclePos);
  int uavNextX=uavX;
  int uavNextY=uavY;
  int uavNextZ=uavZ;
  
  //mpathSearch->printPath(path);
  if(path.size()>1)
  {
    auto p=path.begin();
    
    p++;
    Node node = *p;

    uavNextX = node.x;
    uavNextY = node.y;
    uavNextZ = uavZ;
      
  }else if(uavZ < mmap->getMaxFlyHeight()-1)
  {
    uavNextZ = uavZ+1;

  }
  printf("uavNextX=%3d,uavNextY=%3d,uavNextZ=%3d\r\n",uavNextX,uavNextY,uavNextZ);

  
  // if(uavNextZ>uavZ)
  // {
  //   if(mmap->get_mappoint (uavX,uavY, uavNextZ)==0)
  //   {    
  //     uavNextX=uavX;
  //     uavNextY=uavY;
  //   }
  //   else
  //   {
  //     int flag=0;
  //     for(int i=-1;i<2;i++)
  //     {	
  //     	for(int j=-1;j<2;j++)
  //     	{
  //     	  if(i==0&&j==0)
  //     	    continue;
  //     	  if(mmap->get_mappoint (uavX+i,uavY+j, uavZ)==0)
  //     	  {
  //     	    uavNextX=uavX+i;
  //     	    uavNextY=uavY+j;
  //     	    uavNextY=uavZ;
  //     	    flag=1;
  //     	    break;
  //     	  }	 
  //     	}	
  //     	if(flag==1)
  //     	  break;
  //     }	   
  //   }
  // }
    
  //if(mstar->getPathSize()>1)
  {
    mpstFlayPlane->astUav[plane_num].nY = uavNextY;
    mpstFlayPlane->astUav[plane_num].nX = uavNextX;
    mpstFlayPlane->astUav[plane_num].nZ = uavNextZ;
  }
  return make_pair(uavNextX,uavNextY);
}
  
pair<int, int> PLANE::plane_tran(int plane_num,int goods_no, vector<pair<int, int>> obstaclePos)
{
  int uavX = mpstMatch->astWeUav[plane_num].nX;
  int uavY = mpstMatch->astWeUav[plane_num].nY;
  int uavZ = mpstMatch->astWeUav[plane_num].nZ;

  if(goods_no==-1)
    return make_pair(uavX,uavY);
  int goods_num;
  for(goods_num=0;goods_num<mpstMatch->nGoodsNum;goods_num++)
  {
    if(goods_no==mpstMatch->astGoods[goods_num].nNO)
      break;	  
  }

  int goodsXStart = mpstMatch->astGoods[goods_num].nEndX;
  int goodsYStart = mpstMatch->astGoods[goods_num].nEndY;

  printf("uavX=%3d,uavY=%3d,uavZ=%3d,goodsXStart=%d,goodsYStart=%d\r\n",uavX,uavY,uavZ,goodsXStart,goodsYStart);
  //mstar->findpath(uavX, uavY, uavZ, goodsXStart, goodsYStart,uavZ);

  vector<Node> path = mpathSearch->createGraph(make_pair(uavX,uavY),make_pair(goodsXStart,goodsYStart),uavZ,obstaclePos);
  int uavNextX=uavX;
  int uavNextY=uavY;
  int uavNextZ=uavZ;
  if(path.size()>1)
  {
    auto p=path.begin();
    
    p++;
    Node node = *p;
    uavNextX = node.x;
    uavNextY = node.y;
    uavNextZ = uavZ;
      
  }else if(uavZ < mmap->getMaxFlyHeight()-1)
  {
    uavNextZ = uavZ+1;

  }
  printf("uavNextX=%3d,uavNextY=%3d,uavNextZ=%3d\r\n",uavNextX,uavNextY,uavNextZ);
  
  //   if(uavNextZ<uavZ)
  // {
  //   if(mmap->get_mappoint (uavX,uavY, uavNextZ)==0)
  //   {    
  //     uavNextX=uavX;
  //     uavNextY=uavY;
  //   }
  //   else
  //   {
  //     int flag=0;
  //     for(int i=-1;i<2;i++)
  //     {	
	// for(int j=-1;j<2;j++)
	// {
	//   if(i==0&&j==0)
	//     continue;
	//   if(mmap->get_mappoint (uavX+i,uavY+j, uavZ)==0)
	//   {
	//     uavNextX=uavX+i;
	//     uavNextY=uavY+j;
	//     uavNextY=uavZ;
	//     flag=1;
	//     break;
	//   }	 
	// }	
	// if(flag==1)
	//   break;
  //     }	   
  //   }
  // }

  //if(mstar->getPathSize()>1)
  {
    mpstFlayPlane->astUav[plane_num].nY = uavNextY;
    mpstFlayPlane->astUav[plane_num].nX = uavNextX;
    mpstFlayPlane->astUav[plane_num].nZ = uavNextZ;
  }
  return  make_pair(uavNextX,uavNextY);
}
  
void PLANE::plane_put(int plane_num,int high)
{
  int step=0;
  while( mpstFlayPlane->astUav[plane_num].nZ>high)
  {
    mpstFlayPlane->astUav[plane_num].nZ-=1;
    step++;
    break;
  }
}

void PLANE:: planePathCorretion(void)
{
  int uavX=0 ,uavNextX=0;
  int uavY=0 ,uavNextY=0;
  int uavZ=0 ,uavNextZ=0;
  
  int uavX1=0 ,uavNextX1=0;
  int uavY1=0 ,uavNextY1=0;
  int uavZ1=0 ,uavNextZ1=0;

  int correctState=0;

  for(int uavnum=0;uavnum< mpstMatch->nUavWeNum;uavnum++)//判断交叉飞行存不存在，撞击风险
  {
    if(mpstMatch->astWeUav[uavnum].nStatus!=1)//飞机有效
    {
      uavX      = mpstMatch->astWeUav[uavnum].nX;
      uavY      = mpstMatch->astWeUav[uavnum].nY;
      uavZ      = mpstMatch->astWeUav[uavnum].nZ;
      uavNextX  = mpstFlayPlane->astUav[uavnum].nX; 
      uavNextY  = mpstFlayPlane->astUav[uavnum].nY;
      uavNextZ  = mpstFlayPlane->astUav[uavnum].nZ;
      if(uavZ == uavNextZ)//同一平面
      {
        int tmpX = abs(uavNextX - uavX);
        int tmpY = abs(uavNextY - uavY);
        if((tmpX==1)&&(tmpY==1))//走的对角线
        {
          for(int uavTemp=0;uavTemp< mpstMatch->nUavWeNum;uavTemp++)
          {
            if(uavTemp != uavnum)
            {
              if(mpstMatch->astWeUav[uavTemp].nStatus!=1)//飞机有效
              {
                uavX1      = mpstMatch->astWeUav[uavTemp].nX;
                uavY1      = mpstMatch->astWeUav[uavTemp].nY;
                uavZ1      = mpstMatch->astWeUav[uavTemp].nZ;
                uavNextX1  = mpstFlayPlane->astUav[uavTemp].nX; 
                uavNextY1  = mpstFlayPlane->astUav[uavTemp].nY;
                uavNextZ1  = mpstFlayPlane->astUav[uavTemp].nZ;
                if(uavNextZ1 == uavNextZ)
                {
                  int tmpX1 = abs(uavNextX1 - uavX1);
                  int tmpY1 = abs(uavNextY1 - uavY1);  
                  
                  if((tmpX1 ==1 )&& (tmpY1 ==1))//走的对角线
                  {
                    int tmpX2 = abs(uavNextX1 - uavNextX);
                    int tmpY2 = abs(uavNextY1 - uavNextY);  

                    if((tmpX2<=1)&&(tmpY2<=1))//存在交叉问题
                    {
                    // printf("\n");
                    // printf("uavX=%d,uavY=%d,uavNextX=%d,uavNextY=%d\n",uavX,uavY,uavNextX,uavNextY);
                    // printf("uavX1=%d,uavY1=%d,uavNextX1=%d,uavNextY1=%d\n",uavX1,uavY1,uavNextX1,uavNextY1);
                    // printf("\n");
                    // string response;
                    // cin >> response;
                      correctState = 1;//改变了原来的计划路线
                      if(mpstFlayPlane->astUav[uavnum].nZ< mmap->getMaxFlyHeight())
                      {
                        mpstFlayPlane->astUav[uavnum].nX = mpstMatch->astWeUav[uavnum].nX;
                        mpstFlayPlane->astUav[uavnum].nY = mpstMatch->astWeUav[uavnum].nY;
                        mpstFlayPlane->astUav[uavnum].nZ++;
                      }else{
                        if(1 != mmap->get_mappoint(mpstFlayPlane->astUav[uavnum].nX,mpstFlayPlane->astUav[uavnum].nY,mpstFlayPlane->astUav[uavnum].nZ-1))
                        {
                          mpstFlayPlane->astUav[uavnum].nX = mpstMatch->astWeUav[uavnum].nX;
                          mpstFlayPlane->astUav[uavnum].nY = mpstMatch->astWeUav[uavnum].nY;
                          mpstFlayPlane->astUav[uavnum].nZ--;
                        }  
                        else{
                          mpstFlayPlane->astUav[uavnum].nX = mpstMatch->astWeUav[uavnum].nX;
                          mpstFlayPlane->astUav[uavnum].nY = mpstMatch->astWeUav[uavnum].nY;
                          mpstFlayPlane->astUav[uavnum].nZ = mpstMatch->astWeUav[uavnum].nZ;
                        }
                      }
                    }
                  }

                }
              }
            }
            
          }

        }
      }
      

    }
  }


  while(correctState == 1)//如果改变了飞行计划，则需要校准一下飞行计划，防止自我撞击
  {
    
    correctState=0;
    for(int uavnum=0;uavnum< mpstMatch->nUavWeNum;uavnum++)//判断飞行存不存在，撞击风险
    {
      if(mpstMatch->astWeUav[uavnum].nStatus!=1)//飞机有效
      {
        uavX      = mpstMatch->astWeUav[uavnum].nX;
        uavY      = mpstMatch->astWeUav[uavnum].nY;
        uavZ      = mpstMatch->astWeUav[uavnum].nZ;
        uavNextX  = mpstFlayPlane->astUav[uavnum].nX; 
        uavNextY  = mpstFlayPlane->astUav[uavnum].nY;
        uavNextZ  = mpstFlayPlane->astUav[uavnum].nZ;
        for(int uavTmp=0; uavTmp< mpstMatch->nUavWeNum;uavTmp++)//判断飞行存不存在，撞击风险
        {
          if(mpstMatch->astWeUav[uavTmp].nStatus!=1)//飞机有效
          {
            uavNextX1  = mpstFlayPlane->astUav[uavTmp].nX; 
            uavNextY1  = mpstFlayPlane->astUav[uavTmp].nY;
            uavNextZ1  = mpstFlayPlane->astUav[uavTmp].nZ;   
            if(uavNextX==uavNextX1&&\
               uavNextY==uavNextX1&&\
               uavNextZ==uavNextZ1)   //两架飞机目标点相同，需要处理
               {
                 mpstFlayPlane->astUav[uavnum].nX = uavX;
                 mpstFlayPlane->astUav[uavnum].nY = uavY;
                 mpstFlayPlane->astUav[uavnum].nZ = uavZ;
                 correctState=1;
               }      
          }
        }
      }
    }

  }


}

void PLANE::set_plane_newmatch(MATCH_STATUS * pstMatch)
{
  mpstMatch=pstMatch;
}
  
FLAY_PLANE* PLANE::renew()
{
  return mpstFlayPlane;
}

pair<int, int> PLANE::plane_trackEnemy(int plane_num,int enemy_id, vector<pair<int, int>> obstaclePos)
{
  //plane_num 我方攻击机器的ID， enemy_id 敌方机器ID， 
  int uavX = mpstMatch->astWeUav[plane_num].nX;
  int uavY = mpstMatch->astWeUav[plane_num].nY;
  int uavZ = mpstMatch->astWeUav[plane_num].nZ;

  if(enemy_id<0)
    return make_pair(uavX,uavY);
  if(enemy_id> mpstMatch->nUavEnemyNum)
  {
    return make_pair(uavX,uavY);
  }
  int uavEnemyX = mpstMatch->astEnemyUav[enemy_id].nX;
  int uavEnemyY = mpstMatch->astEnemyUav[enemy_id].nY;
  int uavEnemyZ = mpstMatch->astEnemyUav[enemy_id].nZ;

  int uavGoalX = uavX;//路径规划目标点
  int uavGoalY = uavY;

  // cout<<"------------==--------------------"<<endl;
  // cout<<"-------00-----==----------------00--"<<endl;
  // cout<< "wePlane="<<plane_num <<"; enemy_id="<< enemy_id <<"  ;Weight= "<<mpstMatch->astEnemyUav[enemy_id].nLoadWeight <<endl;


  if(mpstMatch->astEnemyUav[enemy_id].nStatus == UAV_FOG)//敌方无人机在雾区
  {
    if(enemyLastState[plane_num].enemyId == enemy_id)
    {
      
      uavGoalX = enemyLastState[plane_num].coord.first;
      uavGoalY = enemyLastState[plane_num].coord.second;  
    
    }
  }else if(mpstMatch->astEnemyUav[enemy_id].nStatus == 0)//敌方无人机可见
  {
    if(mpstMatch->astEnemyUav[enemy_id].nGoodsNo == -1)//敌方无人机未载货
    {

      uavGoalX = mpstMatch->astEnemyUav[enemy_id].nX;//路径规划目标点
      uavGoalY = mpstMatch->astEnemyUav[enemy_id].nY; 
      if(enemy_id<1000)
      {
        enemyLastState[plane_num].enemyId    = enemy_id;
        enemyLastState[plane_num].goodsId    = -1;
        enemyLastState[plane_num].trackState = -1;
        enemyLastState[plane_num].coord      = make_pair(uavGoalX,uavGoalY);
      }
     
    }else{//敌方无人机已经载货

      if(uavEnemyZ< mmap->getMinFlyHeight())//敌方无人机在最低线以下
      {
        if(uavEnemyX==uavX && uavEnemyY==uavY)//到达敌方无人机上方
        {
          mpstFlayPlane->astUav[plane_num].nY = uavY;//发送飞行数据
          mpstFlayPlane->astUav[plane_num].nX = uavX;
          if(uavZ>0)
          {
            uavZ--;
          }
          mpstFlayPlane->astUav[plane_num].nZ = uavZ;
          
          if(enemy_id<1000)
          {
            enemyLastState[plane_num].enemyId    = enemy_id;
            enemyLastState[plane_num].goodsId    = -1;
            enemyLastState[plane_num].trackState =  1;
            enemyLastState[plane_num].coord      = make_pair(uavX,uavY);
          }

          return make_pair(uavX,uavY);
          
        }
      }else{//敌方无人机在最低线以上
        if( (enemyLastState[plane_num].goodsId == -1) || \
            (enemyLastState[plane_num].enemyId != enemy_id) ||\
            (enemyLastState[plane_num].goodsId >= mpstMatch->nGoodsNum)\
          )//还未获取货物信息
        {
          for(int i=0; i< mpstMatch->nGoodsNum; i++)
          {
            if(mpstMatch->astGoods[i].nNO == mpstMatch->astEnemyUav[enemy_id].nGoodsNo)
            {
              uavGoalX = mpstMatch->astGoods[i].nEndX;
              uavGoalY = mpstMatch->astGoods[i].nEndY;
              if(enemy_id<1000)
              {
                enemyLastState[plane_num].enemyId    = enemy_id;
                enemyLastState[plane_num].goodsId    = i;
                enemyLastState[plane_num].trackState = -1;
                enemyLastState[plane_num].coord      = make_pair(uavGoalX,uavGoalY);
              }

              break;
            }
          }
        }else{//已经有货物信息

          uavGoalX = mpstMatch->astGoods[enemyLastState[plane_num].goodsId].nEndX;
          uavGoalY = mpstMatch->astGoods[enemyLastState[plane_num].goodsId].nEndY;
      
          if(enemy_id< 1000)
          {
            enemyLastState[plane_num].enemyId    = enemy_id;
            enemyLastState[plane_num].trackState = -1;
            enemyLastState[plane_num].coord      = make_pair(uavGoalX,uavGoalY);
          }          

        }

      }      
    }

  }
  
  int uavNextX=uavX;
  int uavNextY=uavY;
  int uavNextZ=uavZ;

  if(uavGoalX==uavX && uavGoalY==uavY)//到达坐标点
  {
    if(mpstMatch->astEnemyUav[enemy_id].nStatus == 0)//敌方无人机可见
    {
      if(uavEnemyX==uavX && uavEnemyY==uavY)
      {
        if(uavEnemyZ< uavZ)//敌方无人机在下
        {
          if(uavNextZ>0)
          {
            uavNextZ--;
          }
        }else if(uavEnemyZ >uavZ){
          if(uavNextZ< mmap->getMaxFlyHeight())
          {
            uavNextZ++;
          }
        }
      }
      
    }else{

    }
   
  }else{
    if(uavZ< mmap->getMinFlyHeight())
    {
      uavNextZ++;
    }else{
      if(make_pair(uavX,uavY)!=make_pair(uavGoalX,uavGoalY))
      {
        vector<Node> path = mpathSearch->createGraph(make_pair(uavX,uavY),make_pair(uavGoalX,uavGoalY),uavZ,obstaclePos);

        if(path.size()>1)
        {
          auto p=path.begin();
          
          p++;
          Node node = *p;
          uavNextX = node.x;
          uavNextY = node.y;
          uavNextZ = uavZ;
            
        }else if(uavZ < mmap->getMaxFlyHeight())
        {
          uavNextZ = uavZ+1;
        }
      }
      
    }
  }

  mpstFlayPlane->astUav[plane_num].nY = uavNextY;
  mpstFlayPlane->astUav[plane_num].nX = uavNextX;
  mpstFlayPlane->astUav[plane_num].nZ = uavNextZ;
  
  return  make_pair(uavNextX,uavNextY);
}




  
