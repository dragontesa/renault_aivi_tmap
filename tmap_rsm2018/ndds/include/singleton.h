// singleton.h

// 작성자 : 정인택
// 작성일 : 2007.12.27
// 설  명 : Singleton 디자인 패턴 매크로 정의

#ifndef __SINGLETON_H
#define __SINGLETON_H

#include <assert.h>

#ifdef _DEBUG
#define _DbgPrintException(err)									\
	{																	\
	DBGPRINT(													\
		_T("Exception : Error = %d, File = %s, Line = %d\n"),		\
		err,__FILE__,__LINE__);										\
	}
#else
#define _DbgPrintException(err)		((void)0)
#endif

#define DECLARE_SINGLETON(class_name)								\
protected :															\
	static class_name*	m_pInstance;								\
public :															\
	static bool IsInstance() { return m_pInstance != NULL; }		\
	static class_name& GetInstance();								\
	static bool CreateInstance();									\
	static void DestroyInstance();

#define IMPLEMENT_SINGLETON(class_name)								\
	class_name*	class_name::m_pInstance = NULL;						\
	class_name& class_name::GetInstance()							\
	{																\
		return *m_pInstance;										\
	}																\
																	\
	bool class_name::CreateInstance()								\
	{																\
		if(m_pInstance == NULL)										\
			m_pInstance = new class_name;							\
		return (m_pInstance != NULL);								\
	}																\
																	\
	void class_name::DestroyInstance()								\
	{																\
		if(m_pInstance)												\
			delete m_pInstance;										\
		m_pInstance = NULL;											\
	}

#define DECLARE_SINGLETON_INSTANCE(class_name)						\
protected :															\
	class class_name##Instance										\
	{																\
	public :														\
		class_name##Instance()										\
		{															\
			if(!class_name::CreateInstance())						\
				_DbgPrintException(SFL_ERROR_OUTOFMEMORY);			\
		}															\
		~class_name##Instance()										\
		{															\
			class_name::DestroyInstance();							\
		}															\
	};																\
	static class_name##Instance		m_singletonInstance;

#define IMPLEMENT_SINGLETON_INSTANCE(class_name)					\
	class_name::class_name##Instance								\
	class_name::m_singletonInstance;

#endif // __SINGLETON_H
