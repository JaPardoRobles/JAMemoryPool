
#pragma once

#include <list>
#include "JAMemoryBlock.h"

#define NUM_MEMORY_BLOCKS 1024
#define MEMORY_SIZE 1024 * 1024 * 16 

namespace JAPR
{
	class JAMemoryManager
	{

	public:
		// Singleton Getter
		static JAMemoryManager* Get();

		// Returns a pointer to a block of the specified size.
		void* GetBlock(unsigned int Size);
		
		// Frees a block from a specified ptr and makes available again.
		void FreeMemoryBlock(void* Ptr);

		// Get the current amount of memory managed.
		inline unsigned int GetMemoryManagedSize() const { return ManagedSize; }

		// Get the current aloccated size.
		inline unsigned int GetAllocatedSize() const { return AllocatedSize; }

		// Check the coherence of the blocks. Debug purposes
		void CheckCoherence() const;

	protected:
		JAMemoryManager();
		virtual ~JAMemoryManager();
		inline JAMemoryManager(const JAMemoryManager& Other);
		JAMemoryManager& operator=(const JAMemoryManager& Other) {}

		// Get a new memory block object;
		JAMemoryBlock* GetNewMemoryBlock();

		bool AreBlockContinuous(const JAMemoryBlock* FirstBlock, const JAMemoryBlock* SecondBlock) const;

		// Member variables
	protected:
		JAMemoryBlock MemoryBlockContainer[NUM_MEMORY_BLOCKS];

		std::list<JAMemoryBlock*> FreeBlocks;
		std::list<JAMemoryBlock*> UsedBlocks;
		std::list<JAMemoryBlock*> UnUsedBlocks;

		void* MainMemoryBlockPtr;
		unsigned int ManagedSize;
		unsigned int AllocatedSize;
	};
}















