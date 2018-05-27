#ifndef __SEARCH_H_
#define __SEARCH_H_

#include "include/SquareGraph.h"
#include "include/mapCreate.h"


class pathSearch{
    public:
        pathSearch(void);
        ~pathSearch(void);

        pathSearch(MAP *map);

        void createSearchMap(vector<pair<int, int>> obstaclePos, int zHight);
        vector<Node> createGraph(pair<int, int> start, pair<int, int> goal, int height, vector<pair<int, int>> obstaclePos);
        void printPath(vector<Node> path);
    private:
        MAP *mmap;

        int minFlyHeight;
        int maxFlyHeight;
        int mapDimension;
        

};


#endif
















