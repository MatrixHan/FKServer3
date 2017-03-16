/**
*	created:		2013-4-7   22:24
*	filename: 		FKRandomPool
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <windows.h>
#include "../Include/FKRandomPool.h"
#include "../Include/FKLogger.h"
#include "../Include/Network/FKPacket.h"
//------------------------------------------------------------------------
RandomPool g_RandomPool;
//------------------------------------------------------------------------
/// ���һ��С��[0, dst) ������
DWORD RandomPool::GetOneRandLess( DWORD dst )
{

	DWORD _rand = GetOneRand();
	return _rand % dst;
}
//------------------------------------------------------------------------
/// ���һ��[__min, __max)
DWORD RandomPool::GetOneRandBetween( DWORD __min, DWORD __max )
{
	DWORD __rand = GetOneRandLess( __max );
	while( __rand < __min )
	{
		DWORD _delta = __min - __rand;
		__rand += rand() % (_delta + 100 );
	}
	__rand = min( __max - 1, __rand );
	return __rand;
}
//------------------------------------------------------------------------
DWORD RandomPool::GetOneRand()
{
	DWORD _elem = m_Pool[m_ElePtr];
	StepPtr();
	return _elem;
}
//------------------------------------------------------------------------
void RandomPool::InitPool()
{
	for( int i = 0; i < PoolSize; i++ )
	{
		m_Pool.push_back( GetRandomGenerator()->Generate32() );
	}
}
//------------------------------------------------------------------------
void RandomPool::StepPtr()
{
	m_ElePtr++;
	if( m_ElePtr == PoolSize )
		m_ElePtr = 0;
}
//------------------------------------------------------------------------
RandomPool::RandomPool()
{
	m_Pool.reserve( PoolSize );
	InitPool();
}
//------------------------------------------------------------------------
RandomPool::~RandomPool()
{
	m_Pool.clear();
}
//------------------------------------------------------------------------
MsgCounter g_msgCounter;
//------------------------------------------------------------------------
/// MSGCounter
MsgCounter::MsgCounter()
{
	m_CounterMap.clear();
	m_SortTimeDelta = 1000*60;
	m_SendCounter = 0;
}
//------------------------------------------------------------------------
MsgCounter::~MsgCounter()
{

}
//------------------------------------------------------------------------
/// ����
void MsgCounter::_Reset()
{
	m_CounterMap.clear();
	m_AllMsgCounter = 0;
	m_LastPrintTime = 0;
}
//------------------------------------------------------------------------
/// ������Ϣ��Ŀ
void MsgCounter::AddMsg( int mainid, int subid )
{
	AddAllCounter();

	int _cmdValue = MAKECMDVALUE( mainid, subid );

	stdext::hash_map< unsigned int, unsigned int >::iterator _iter = m_CounterMap.find( _cmdValue );
	m_CounterMap[_cmdValue]++;

	DWORD _TheTick = GetTickCount();

	if( _TheTick - m_LastPrintTime >= m_SortTimeDelta )
	{
		m_LastPrintTime = _TheTick;

		_Print();
	}
}
//------------------------------------------------------------------------
struct VecInfo
{
	BYTE main;
	BYTE sub;
	unsigned int count;
};
//------------------------------------------------------------------------
std::list< VecInfo > _Vec;
//------------------------------------------------------------------------
bool msgsorter( const VecInfo& _Left, const VecInfo& _Right )
{
	return _Left.count >= _Right.count;
}
//------------------------------------------------------------------------
/// ��ӡ
void MsgCounter::_Print()
{
	_Vec.clear();
	stdext::hash_map< unsigned int, unsigned int >::iterator _iter = m_CounterMap.begin();
	VecInfo _info;

	for( ; _iter != m_CounterMap.end(); ++_iter )
	{
		int _Total = _iter->first;
		int _Count = _iter->second;

		BYTE _Main, _Sub;

		_Sub  = ( _Total << 7 ) >> 7;;
		_Main = _Total >> 8;
		_info.main = _Sub;
		_info.sub = _Main;
		_info.count = _Count;

		_Vec.push_back( _info );
	}

	if( !_Vec.empty() )
	{
		_Vec.sort( &msgsorter );

		if( m_AllMsgCounter <= 0 )
			m_AllMsgCounter = 1;

		while( _Vec.size() >= 30 )
		{
			_Vec.pop_back();
		}

		g_logger.forceLog( zLogger::zFATAL, "��ʼ��Ϣ��ӡ" );

		std::list< VecInfo >::iterator _IterList;
		for( _IterList = _Vec.begin(); _IterList != _Vec.end(); ++_IterList )
		{
			g_logger.forceLog( zLogger::zFATAL, "������Ϣ �� 1. ʮ����[%d][%d], 2. ʮ������[%x][%x] ��Ϣ��������[%d] ����Ϣ��Ŀ[%d], ��ռ�ٷֱ�[%.3f%]",
				_IterList->main, _IterList->sub,
				_IterList->main, _IterList->sub, m_AllMsgCounter, _IterList->count, _IterList->count*1.0f/m_AllMsgCounter*100 );

		}
		g_logger.forceLog( zLogger::zFATAL, "==========������Ϣ��ӡ===========" );
	}
}
//------------------------------------------------------------------------
void MsgCounter::AddSendCounter()
{
	AILOCKT( m_Locker );
	m_SendCounter++;
	if( m_SendCounter % 300 == 0 )
	{
		g_logger.forceLog( zLogger::zFATAL, "�ܴ���:%u", m_SendCounter );
	}
}
//------------------------------------------------------------------------