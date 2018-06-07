#include "search.h"


using namespace std;

pathSearch::pathSearch(void)
{


}

pathSearch::~pathSearch(void)
{

    
}

pathSearch::pathSearch(MAP_CREATE *map):mmap(map)
{
    // mapXsizeG = mmap->getMapXsize();
    // mapYsizeG = mmap->getMapYsize();
    // mapZsizeG = mmap->getMapZsize();
    mapDimension = max(mmap->getMapXsize(),mmap->getMapYsize());

    minFlyHeight = mmap->getMinFlyHeight();
    maxFlyHeight = mmap->getMaxFlyHeight();

    // printf("%d,%d,%d\r\n",mapXsizeG,mapYsizeG,mapZsizeG);

}

void pathSearch::createSearchMap(vector<pair<int, int>> obstaclePos, int zHight)
{
    // if(zHight>mmap->getMapZsize())
    // {
    //     zHight = mmap->getMapZsize();
    // }
    // for(int i = 0; i< mmap->getMapXsize();i++){
    //     for(int j=0; j< mmap->getMapYsize(); j++){

    //         char type = mmap->get_mappoint(i,j,zHight);
    //         mAstar->setCellValue(make_pair(i, j), type);
    //     }
    // }//初始化最初的地图
    // if(obstaclePos.size()>0)
    //     mAstar->setObstaclePos(obstaclePos);//添加障碍点

}
vector<Node> pathSearch::createGraph(pair<int, int> start, pair<int, int> goal, int height, vector<pair<int, int>> obstaclePos)
{
    SquareGraph *mySquareGreph = new SquareGraph(mapDimension);

    mySquareGreph->setFirstRobotPos(start);
    mySquareGreph->setSecondRobotPos(goal);

    if(height>mmap->getMapZsize())
    {
        height = mmap->getMapZsize();
    }
    for(int i = 0; i< mmap->getMapXsize();i++){
        for(int j=0; j< mmap->getMapYsize(); j++){

            char type = mmap->get_mappoint(i,j,height);
            if(type == 2) type =0;
            mySquareGreph->setCellValue(make_pair(i, j), type);
        }
    }//初始化最初的地图
    if(obstaclePos.size()>0)
        mySquareGreph->setObstaclePos(obstaclePos);//添加障碍点

    vector<Node> path;
    createSearchMap(obstaclePos,height);

    path = mySquareGreph->executeAStar();

    delete mySquareGreph;

    return path;
}

void pathSearch::printPath(vector<Node> path)
{

}

