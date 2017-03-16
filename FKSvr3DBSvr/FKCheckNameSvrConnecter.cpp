/**
*	created:		2013-4-8   16:34
*	filename: 		FKCheckNameSvrConnecter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "FKCheckNameSvrConnecter.h"
#include "FKDBSvr.h"
//------------------------------------------------------------------------
void CCheckNameSvrConnecter::OnIocpConnect(){
	__super::OnIocpConnect();
	DBService* service=DBService::instance();

	service->m_Svr2SvrLoginCmd.m_now=time(NULL);
	sendcmd(&service->m_Svr2SvrLoginCmd,sizeof(service->m_Svr2SvrLoginCmd));
}
//------------------------------------------------------------------------
void CCheckNameSvrConnecter::OnDisconnect(){
	__super::OnDisconnect();

	DBService* service=DBService::instance();
	do {
		AILOCKT(service->m_checkcreateplayernametransmanage);
		DBService::zBaseCmdTransidManage::iterator it;
		for (it=service->m_checkcreateplayernametransmanage.begin();it!=service->m_checkcreateplayernametransmanage.end();it++){
			FreePacketBuffer(it->second.pmsgbuffer);
		}
		service->m_checkcreateplayernametransmanage.clear();
	} while (false);

	m_bovalid = false;
}
//------------------------------------------------------------------------
bool CCheckNameSvrConnecter::isvalid(){
	return m_bovalid;
}
//------------------------------------------------------------------------
time_t CCheckNameSvrConnecter::valid_timeout(){
	return 30;
}
//------------------------------------------------------------------------
bool CCheckNameSvrConnecter::msgParse(stBaseCmd* pcmd, unsigned int ncmdlen,stQueueMsgParam* bufferparam){
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
bool CCheckNameSvrConnecter::OnstSvr2SvrLoginCmd(stSvr2SvrLoginCmd* pcmd, unsigned int ncmdlen,stQueueMsgParam* bufferparam){
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
		m_bovalid = true;
		g_logger.debug("�����ظ��Լ������� %d(%d) ��½У��ɹ�!",svr_id,svr_type);
	}
	return true;
}
//------------------------------------------------------------------------