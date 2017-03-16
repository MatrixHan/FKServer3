/**
*	created:		2013-4-7   21:55
*	filename: 		FKGlobal
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <time.h>
#include <Windows.h>
#include <malloc.h>
#include "Network/FKAsynSocket.h"
#include "Endec/FKEncDec.h"
#include "Endec/FKCrc32.h"
#include "FKStringEx.h"
#include "RTTI/FKReflect.h"
#include "FKXmlParser.h"
#include "Network/FKPacket.h"
#include "Endec/FKBase64.h"
#include "DataBase/FKDBConnPool.h"
//------------------------------------------------------------------------
#define		_LOGINSVR_TYPE				100		//�ʺŷ�����
#define		_DBSVR_TYPE					200		//���ݿ������
#define		_GAMESVR_TYPE				300		//��Ϸ���������(���ظ�����Ϸģ��,Ϊ����Ϸģ���ṩͨ�ŷ���)
#define		_SUPERGAMESVR_TYPE			400		//��Ϸ������(����ȫ������)
#define		_CHECKNAMESVR_TYPE			500		//�����ظ���������(����)
#define		_GLOBAL_GAMESVR_TYPE		600		//��������Ϸ������(����)
#define		_LOGSVR_TYPE				700		//��־������
#define		_GMSERVERMANAGE_TYPE		800		//��Ϸ���������(����)
#define		_GLOBAL_LOGSVR_TYPE			900		//ȫ����־������(����)
#define		_RMBSVR_TYPE				1000	//��ֵ������(����һ������ֻ����LOGIN)
#define		_LOGINSVR_GATEWAY_TYPE		1100	//��½����
#define		_GAMESVR_GATEWAY_TYPE		1300	//��Ϸ����
//------------------------------------------------------------------------
#define		_MAX_ACCOUT_LEN_			48		//����˺ų���
#define		_MAX_NAME_LEN_				48		//������ֳ���
#define		_MAX_PASS_LEN_				48		//������볤��
#define		_MAX_IP_LEN_				24		//���IP����
#define		_MAX_RMB_LEN_				24		//��󶩵�����
//------------------------------------------------------------------------
#define		_PRE_CHECK_CODE_				0x55884433					//���� �ʺŷ����� ��һ������У����
#define		DB_CONFIG_TBL					"mydb_svrcfg_tbl"			//���кϷ��������б�
#define		DB_CHKSERVERPARAM_TBL			"mydb_chksvrparam_tbl"		//�������ı�������ı�
#define 	_FLASH_AUTHORITY_IP_			"0.0.0.0"
#define 	_FLASH_AUTHORITY_PORT_			843
#define 	_FLASH_AUTHORITY_NAME_			"flashauthority_549sdfjuaosud5trs"
#define 	_FLASH_AUTHORITY_RET_			"<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\"/></cross-domain-policy>\0"

//------------------------------------------------------------------------
struct stServerInfo
{
	union
	{
		struct
		{
			union
			{
				struct
				{
					WORD wgame_type;
					WORD wzoneid;
				};
				DWORD game_type_zoneid_value;
			};

			union
			{
				struct
				{
					WORD svr_id;
					WORD svr_type;
				};
				DWORD svr_id_type_value;
			};
		};
		__int64 svr_marking;
	};
	char szZoneName[_MAX_NAME_LEN_];
	char szName[_MAX_NAME_LEN_];
	char szIp[_MAX_IP_LEN_];			//�������ڲ����ݽ���ʹ�õ�IP
	WORD GatewayPort;					//���ض˿�
	WORD DBPort;						//���ݿ���������ӵĶ˿�
	WORD GamePort;						//��Ϸ���������ӵĶ˿�
	WORD SubsvrPort;

	stServerInfo()
	{
		ZEROOBJ(this);
	};
};
//------------------------------------------------------------------------
typedef std::map< __int64, stServerInfo> svrinfomap;
typedef std::map< __int64, stServerInfo>::value_type svrinfomaptype;
typedef std::map< __int64, stServerInfo>::iterator svrinfomapiter;
//------------------------------------------------------------------------
struct stMapInfo
{
	union
	{
		struct
		{
			WORD wmapid;
			BYTE wmapcountryid;
			BYTE btmapsublineid;
		};
		DWORD dwmapid;
	};
	WORD w;
	WORD h;

	stMapInfo()
	{
		memset(this,0,sizeof(*this));
	}
};
//------------------------------------------------------------------------
struct stIpInfoState
{
	in_addr ip;
	union
	{
		struct
		{
			WORD svr_id;
			WORD port;
		};
		DWORD ipinfo_id;
	};
	WORD ncount;

	BYTE type;
	BYTE state;			//0 ����  -1 �ر�

	stIpInfoState()
	{
		ipinfo_id = 0;
		ncount = 0;
		state = 0;
		type = 0;
	}
};
//------------------------------------------------------------------------
/*[��������ͼ����]*/
struct stMapSubLineInfo
{
	BYTE btmapsublineid;
	int nmapcount;
	int nonlineusercount;

	stMapSubLineInfo()
	{
		ZEROOBJ(this);
	}
};
//------------------------------------------------------------------------
struct stCountryInfo
{
	int nCountryId;
	char szCountry[_MAX_NAME_LEN_];
	char szCountryHomeMap[_MAX_NAME_LEN_];
	DWORD dwCountryHomeMapId;
	DWORD countryhomex;
	DWORD countryhomey;
	BYTE btisdel;
	int nonlineusercount;
	int nusercount;
	int nfaction;

	RTTI_DESCRIBE_STRUCT(stCountryInfo, (
		RTTI_FIELD_N(nCountryId, 0, countryid)
		, RTTI_ARRAY_N(szCountry, 0, countryname)
		, RTTI_ARRAY_N(szCountryHomeMap, 0, countryhomemap)
		, RTTI_FIELD_N(countryhomex, 0, countryhomex)
		, RTTI_FIELD_N(countryhomey, 0, countryhomey)
		, RTTI_FIELD_N(btisdel, 0, deleted)
		));

	stCountryInfo()
	{
		ZEROOBJ(this);
	}
};
//------------------------------------------------------------------------
struct stSvrTmpId
{
	union
	{
		struct
		{
			WORD loginsvr_id;		//�Ѳ�ͬ�� �ʺŷ����� ��������ʱID���ֿ� ��ֹ������ͬ��ID
			WORD tmp;				//���� //svr_type
			DWORD dwtmpid;
		};
		unsigned __int64 IDValue;
	};
	__inline DWORD getsvridtype(WORD wtype = _LOGINSVR_TYPE)
	{
		return MAKELONG(loginsvr_id , wtype);
	}
};
//------------------------------------------------------------------------
struct stLoginTokenData
{
public:
	time_t createtime;
	time_t updatatime;
	DWORD tokencheck;
	in_addr ip;						//��½IP
	stSvrTmpId loginsvr_tmpid;
public:
	stLoginTokenData()
	{
		createtime = time(NULL);
	}
	void encode(CEncrypt* enc, DWORD tock)
	{
		updatatime = time(NULL);

		for(int i = 0; i <= (sizeof(*this) - sizeof(tock)); i++)
		{
			*((DWORD*)(&((char*)this)[i])) = *((DWORD*)(&((char*)this)[i])) ^ tock;
		}

		if(enc)
		{
			enc->encdec(this, sizeof(*this), true);
		}
	}
	bool decode(CEncrypt* enc, DWORD tock)
	{
		if(enc)
		{
			enc->encdec(this, sizeof(*this), false);
		}

		for(int i = (sizeof(*this) - sizeof(tock)); i >= 0; i--)
		{
			*((DWORD*)(&((char*)this)[i])) = *((DWORD*)(&((char*)this)[i])) ^ tock;
		}

		return (loginsvr_tmpid.IDValue != 0 && ip.s_addr != INADDR_NONE);
	}
	bool isvalid(DWORD tock, DWORD svr_id, in_addr cip)
	{
		return ((tokencheck == tock) && loginsvr_tmpid.getsvridtype(_LOGINSVR_TYPE) == svr_id && (ip.s_addr == cip.s_addr || ip.s_addr == 0x0100007f /*127.0.0.1*/  || ip.s_addr == 0 /*0.0.0.0*/));
	}
	void init(DWORD tock, stSvrTmpId in_tmpid, in_addr cip)
	{
		ip.s_addr = cip.s_addr;
		loginsvr_tmpid = in_tmpid;
		tokencheck = tock;
	}
};
//------------------------------------------------------------------------
typedef		stLoginTokenData 	stLoginToken;
STATIC_ASSERTMN2(sizeof(stLoginToken) == ROUNDNUM2(sizeof(stLoginToken), 8), sizeof(stLoginToken), ROUNDNUM2(sizeof(stLoginToken), 8), stlogintoken_not_alig_power8);
//------------------------------------------------------------------------
typedef bool(__stdcall *fnInitAutoComplete)(HANDLE, char*);
typedef bool(__stdcall *fnSaveGMOrder)(char*);
//------------------------------------------------------------------------
#define INIT_AUTOCOMPLETE(h,dllname,p1,p2,cmdlist,saveproc,hdll)\
	if ( h && IsDebuggerPresent()!=TRUE )\
{\
	(hdll)=::LoadLibrary(dllname);\
	if (hdll)\
{\
	fnInitAutoComplete pInitAutoComplete=(fnInitAutoComplete)::GetProcAddress((hdll),p1);\
	if (pInitAutoComplete)\
{\
	pInitAutoComplete(h,cmdlist);\
}\
	(saveproc)=NULL;\
	(saveproc)=(fnSaveGMOrder)::GetProcAddress((hdll),p2);\
}\
}
//------------------------------------------------------------------------