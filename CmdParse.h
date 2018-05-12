/**	@file       CmdParse.h
 *	@note       Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *	@brief		
 *
 *	@author     lipengfei
 *	@date       2018/05/10
 *	@note       历史记录：
 *	@note       V1.0.0  
 *	@warning	
 */
#ifndef __CMDPARSE_H__
#define __CMDPARSE_H__

#include "JsonParse.h"

#define SOCKET_HEAD_LEN          8                      ///< 8个字节的头部长度


typedef struct _CONNECT_NOTICE_
{
    char    szNotice[64];
    char    szMsg[128];
}CONNECT_NOTICE;

typedef struct _TOKEN_INFO_
{
    char    szToken[64];
    char    szAction[64];
}TOKEN_INFO;


typedef struct _TOKEN_RESULT_
{
    char    szToken[64];
    char    szNotice[64];
    char    szRoundId[64];
    char    szPalyerId[64];
    int     nResult;
}TOKEN_RESULT;

typedef struct _READY_PARAM_
{
    char    szToken[64];
    char    szAction[64];
}READY_PARAM;



typedef struct _BUILDING_
{
    int     nX;
    int     nY;
    int     nL;
    int     nW;
    int     nH;
}BUILDING;

typedef struct _FOG_
{
    int     nX;
    int     nY;
    int     nL;
    int     nW;
    int     nB;
    int     nT;
}FOG;




#define MAX_BUILDING_NUM        50      
#define MAX_FOG_NUM        50

#define MAX_UAV_NUM         512

#define MAX_UAV_PRICE_NUM    64

#define MAX_GOODS_NUM       256



typedef enum _UAV_STATUS_
{
    UAV_NOMAL = 0,
    UAV_CRASH,
    UAV_FOG
}UAV_STATUS;

typedef struct _UAV_
{
    int     nNO;
    char    szType[8];
    int     nX;
    int     nY;
    int     nZ;
    int     nLoadWeight;            ///< 跟type对应的无人机的载重一样，
    UAV_STATUS  nStatus;
    int     nGoodsNo;
}UAV;


typedef struct _UAV_PRICE_
{
    char    szType[8];
    int     nLoadWeight;
    int     nValue;
}UAV_PRICE;

/** @struct
 * 	@brief	
 *	@note
 */
typedef struct _MAP_INFO_
{
    int     nMapX;
    int     nMapY;
    int     nMapZ;
    int     nParkingX;
    int     nParkingY;

    int     nHLow;
    int     nHHigh;

    int     nBuildingNum;
    BUILDING    astBuilding[MAX_BUILDING_NUM];
    int     nFogNum;
    FOG         astFog[MAX_FOG_NUM];
    int     nUavNum;
    UAV     astUav[MAX_UAV_NUM];
    int     nUavPriceNum;
    UAV_PRICE   astUavPrice[MAX_UAV_PRICE_NUM];
}MAP_INFO;


typedef struct _FLAY_PLANE_
{
    int     nUavNum;
    UAV     astUav[MAX_UAV_NUM];

    int     nPurchaseNum;
    char    szPurchaseType[MAX_UAV_PRICE_NUM][8];
}FLAY_PLANE;

typedef struct _GOODS_
{
    int     nNO;
    int     nStartX;
    int     nStartY;
    int     nEndX;
    int     nEndY;
    int     nWeight;
    int     nValue;
    int     nStartTime;
    int     nRemainTime;
    int     nState;
}GOODS;

typedef struct _MATCH_STATUS_
{
    int     nTime;
    int     nMacthStatus;
    int     nUavWeNum;
    UAV     astWeUav[MAX_UAV_NUM];
    int     nWeValue;
    int     nUavEnemyNum;
    UAV     astEnemyUav[MAX_UAV_NUM];
    int     nEnemyValue;
    int     nGoodsNum;
    GOODS   astGoods[MAX_GOODS_NUM];
}MATCH_STATUS;

/** @fn     int ParserConnect(char *pBuffer, CONNECT_NOTICE *pstNotice)
 *  @brief	
 *	@param  -I   - char * pBuffer
 *	@param  -I   - CONNECT_NOTICE * pstNotice
 *	@return int
 */
int ParserConnect(char *pBuffer, CONNECT_NOTICE *pstNotice);


/** @fn     int ParserTokenResult(char *pBuffer, TOKEN_RESULT *pResult)
 *  @brief	
 *	@param  -I   - char * pBuffer
 *	@param  -I   - TOKEN_RESULT * pResult
 *	@return int
 */
int ParserTokenResult(char *pBuffer, TOKEN_RESULT *pResult);


/** @fn     int ParserMapInfo(char *pBuffer, MAP_INFO *pstMap)
 *  @brief	
 *	@param  -I   - char * pBuffer
 *	@param  -I   - MAP_INFO * pstMap
 *	@return int
 */
int ParserMapInfo(char *pBuffer, MAP_INFO *pstMap);


/** @fn     int ParserUav(cJSON *pUavArray, UAV *astUav, int *pNum)
 *  @brief	
 *	@param  -I   - cJSON * pUavArray
 *	@param  -I   - UAV * astUav
 *	@param  -I   - int * pNum
 *	@return int
 */
int ParserUav(cJSON *pUavArray, UAV *astUav, int *pNum);

/** @fn     int ParserMatchStatus(char *pBuffer, MATCH_STATUS *pstStatus)
 *  @brief	
 *	@param  -I   - char * pBuffer
 *	@param  -I   - MATCH_STATUS * pstStatus
 *	@return int
 */
int ParserMatchStatus(char *pBuffer, MATCH_STATUS *pstStatus);


/** @fn     int CreateTokenInfo(TOKEN_INFO *pstInfo, char *pBuffer)
 *  @brief	
 *	@param  -I   - TOKEN_INFO * pstInfo
 *	@param  -I   - char * pBuffer
 *	@return int
 */
int CreateTokenInfo(TOKEN_INFO *pstInfo, char *pBuffer, int *pLen);

/** @fn     int CreateReadyParam(READY_PARAM *pstParam, char *pBuffer, int *pLen)
 *  @brief	
 *	@param  -I   - READY_PARAM * pstParam
 *	@param  -I   - char * pBuffer
 *	@param  -I   - int * pLen
 *	@return int
 */
int CreateReadyParam(READY_PARAM *pstParam, char *pBuffer, int *pLen);


/** @fn     int CreateFlayPlane(FLAY_PLANE *pstPlane, char *pBuffer, int *pLen)
 *  @brief	
 *	@param  -I   - FLAY_PLANE * pstPlane
 *	@param  -I   - char * pBuffer
 *	@param  -I   - int * pLen
 *	@return int
 */
int CreateFlayPlane(FLAY_PLANE *pstPlane, char *szToken, char *pBuffer, int *pLen);

#endif

