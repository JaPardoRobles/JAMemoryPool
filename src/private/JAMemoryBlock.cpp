
#include "JAMemoryBlock.h"

using namespace JAPR;

JAMemoryBlock::JAMemoryBlock() 
	: Ptr(nullptr)
	, Size(0)
{
}

JAMemoryBlock::JAMemoryBlock(const JAMemoryBlock& Other)
{
	Ptr = Other.Ptr;
	Size = Other.Size;
}

JAMemoryBlock& JAMemoryBlock::operator=(const JAMemoryBlock& Other)
{
	Ptr = Other.Ptr;
	Size = Other.Size;
	return *this;
}

void JAMemoryBlock::Init(unsigned char* InPtr, unsigned int InSize)
{
	Ptr = InPtr;
	Size = InSize;
}

void JAMemoryBlock::Resize(unsigned int NewSize)
{
	Size = NewSize;
}

void JAMemoryBlock::Clear()
{
	Init(nullptr, 0);
}
