// ndds_memory.h
#ifndef __NDDS_MEMORY_H__
#define __NDDS_MEMORY_H__

class CNddsMemory
{
public:
	CNddsMemory();
	virtual ~CNddsMemory();

	//사용할 메모리를 할당 한다.
	void InitBuffer(unsigned char *pBuffer, int nBuffer);
	void InitBuffer(int nSize);

	//사용할 메모리를 해제 한다.
	void ExitBuffer();

	//메모리 블럭의 사용할 위치를 초기화 한다.
	unsigned char* ResetMemory();

	//메모리 블럭에서 사용할 메모리를 할당 받는다.
	//bAlignment : 기존 할당받은 메모리와 연속으로 사용할 때 false, 새로운 메모리 사용할 때 TRUE
	unsigned char* GetMemory(bool bAlignment, int &nFreeSize);

	//메모리 블럭에 사용 메모리를 갱신한다.
	bool UseMemory(int nSize, int &nFreeSize);

private:
	bool	m_bAlloc;		// 메모리를 직접 할당 했는지 여부
	unsigned char*	m_pBuffer;		//	블럭및 구조체 생성시 동적으로 사용할 메모리 블럭
	int	m_nBufferSize;	//	메모리 블럭 크기
	int	m_nBufferPos;	//현재까지 사용중인 메모리 블럭 위치
};

#endif // __NDDS_MEMORY_H__
