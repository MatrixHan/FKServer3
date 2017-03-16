/**
*	created:		2013-4-9   15:23
*	filename: 		FKDBScript
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../FKSvr3Common/FKCommonInclude.h"
//------------------------------------------------------------------------
enum{
	eScript_reload=0,
	eScript_init=1,
	eScript_uninit=2,
};
//------------------------------------------------------------------------
class CScriptSystem:public Singleton<CScriptSystem>
{
public:
	CLuaVM *m_LuaVM;	//lua�����ָ��
public:
	CScriptSystem();
	virtual ~CScriptSystem();
public:
	bool InitScript(char* pszScriptFileName,DWORD initstate=eScript_reload);	//��ʼ���ű�����
	void Bind(CLuaVM* luavm);
};
//------------------------------------------------------------------------