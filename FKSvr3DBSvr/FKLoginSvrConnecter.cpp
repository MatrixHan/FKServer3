/**
*	created:		2013-4-9   15:21
*	filename: 		FKLoginSvrConnecter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "FKLoginSvrConnecter.h"
#include "FKGameSvrSession.h"
#include "FKDBSvr.h"
#include "FKDBScript.h"
//------------------------------------------------------------------------
void CLoginSvrDBConnecter::OnIocpConnect(){
	__super::OnIocpConnect();
	DBService* dbsvr=DBService::instance();

	dbsvr->m_Svr2SvrLoginCmd.m_now=time(NULL);
	sendcmd(&dbsvr->m_Svr2SvrLoginCmd,sizeof(dbsvr->m_Svr2SvrLoginCmd));
}
//------------------------------------------------------------------------
void CLoginSvrDBConnecter::OnDisconnect(){
	__super::OnDisconnect();
	do {
		DBService* dbsvr=DBService::instance();
		AILOCKT(dbsvr->m_loginsvrconnter);
		LoginServerHashCodeMap::iterator it,itnext;
		for (it=dbsvr->m_loginsvrconnter_hashcode.begin(),itnext=it;it!=dbsvr->m_loginsvrconnter_hashcode.end();it=itnext){
			itnext++;
			if (it->second==this){
				dbsvr->m_loginsvrconnter_hashcode.erase(it);
			}
		}
	} while (false);
	m_bovalid = false;
}
//------------------------------------------------------------------------
bool CLoginSvrDBConnecter::isvalid(){
	return m_bovalid;
}
//------------------------------------------------------------------------
time_t CLoginSvrDBConnecter::valid_timeout(){
	return 30;
}
//------------------------------------------------------------------------
bool CLoginSvrDBConnecter::msgParse(stBaseCmd* pcmd, unsigned int ncmdlen,stQueueMsgParam* bufferparam){
	switch (pcmd->value)
	{
	case stSvr2SvrLoginCmd::_value:
		{
			return OnstSvr2SvrLoginCmd((stSvr2SvrLoginCmd*)pcmd,ncmdlen,bufferparam);
		}
		break;
	}
	return true;
}
//------------------------------------------------------------------------
bool CLoginSvrDBConnecter::OnstSvr2SvrLoginCmd(stSvr2SvrLoginCmd* pcmd, unsigned int ncmdlen,stQueueMsgParam* bufferparam){
	FUNCTION_BEGIN;
	if (svr_id_type_value != pcmd->svr_id_type_value) {
		Terminate();
		g_logger.error("���ӵķ�����(%u:%u <> %u:%u)�������ļ���¼��һ��...", svr_id, svr_type, pcmd->svr_id, pcmd->svr_type);
		m_bovalid = false;
	} else {
		time_t difftime=time(NULL)-pcmd->m_now;
		if ( difftime>(60*15) ){
			g_logger.forceLog(zLogger::zERROR ,"������ %d(%d) [%s:%d] ϵͳʱ���쳣( %s <> %s ),��У�����з�����ʱ���������������!",
				svr_id,svr_type,GetRemoteAddress(),GetRemotePort(),timetostr(time(NULL)) ,timetostr(pcmd->m_now));
		}else if(difftime>10){
			g_logger.forceLog(zLogger::zDEBUG ,"������ %d(%d) [%s:%d] ϵͳʱ���Ϊ %d ��( %s <> %s )!",svr_id,svr_type,GetRemoteAddress(),GetRemotePort(),difftime,timetostr(time(NULL)) ,timetostr(pcmd->m_now));
		}
		do{
			DBService* dbsvr=DBService::instance();
			AILOCKT(dbsvr->cfg_lock);
		}while(false);
		m_bovalid = true;
		g_logger.debug("�ʺŷ����� %d(%d) ��½У��ɹ�!",svr_id,svr_type);
	}
	return true;
}
//------------------------------------------------------------------------