
#pragma once

#include <list>

namespace JAPR
{
	class JAMemoryBlock 
	{
	public:
		JAMemoryBlock();
		virtual ~JAMemoryBlock() {}
		JAMemoryBlock(const JAMemoryBlock& Other);
		JAMemoryBlock& operator=(const JAMemoryBlock& Other);

		void Init(unsigned char* Ptr, unsigned int Size);
		inline unsigned char* GetPtr() const { return Ptr; }
		inline unsigned int GetSize() const { return Size; }

		void Resize(unsigned int NewSize);
		void Clear();

	protected:
		unsigned char* Ptr;
		unsigned int Size;
	};
}
