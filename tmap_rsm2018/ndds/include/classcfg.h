// classcfg.h

// 작성자 : 정인택
// 작성일 : 2007.12.27
// 설  명 : 클래스 특성 정의 매크로

#ifndef __CLASSCFG_H
#define __CLASSCFG_H

#define NOT_COPYABLE(class_name)							\
private :													\
	class_name(const class_name&);							\
	class_name& operator=(const class_name&);

#define NOT_INSTANCE(class_name)							\
private :													\
	class_name() {}											\
	~class_name() {}

#endif // __CLASSCFG_H
