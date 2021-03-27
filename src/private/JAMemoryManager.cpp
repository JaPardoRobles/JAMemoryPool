
#include "JAMemoryManager.h"
#include <assert.h>

using namespace JAPR;
using namespace std;

#define _MEMORY_DEBUG 1

JAMemoryManager::JAMemoryManager()
	: MemoryBlockContainer()
	, FreeBlocks()
	, UsedBlocks()
	, UnUsedBlocks()
	, MainMemoryBlockPtr(nullptr)
	, ManagedSize(MEMORY_SIZE)
	, AllocatedSize(0)
{

	MainMemoryBlockPtr = malloc(ManagedSize);

	// Initialize Container and set First block
	JAMemoryBlock* FirstBlock = &MemoryBlockContainer[0];
	FirstBlock->Init(reinterpret_cast<unsigned char*>(MainMemoryBlockPtr), ManagedSize);
	FreeBlocks.push_back(FirstBlock);

	// Initialize rest of unused blocks. Iterator start as 1 because 0 has been initialized already.
	for (int i = 1; i < NUM_MEMORY_BLOCKS; ++i)
	{
		UnUsedBlocks.push_back(&MemoryBlockContainer[i]);
	}
}

JAMemoryManager::~JAMemoryManager()
{
	FreeBlocks.clear();
	UsedBlocks.clear();
	UnUsedBlocks.clear();
	free(MainMemoryBlockPtr);
}

JAMemoryManager::JAMemoryManager(const JAMemoryManager& Other)
	: MemoryBlockContainer(), FreeBlocks(), UsedBlocks()
	, UnUsedBlocks()
	, MainMemoryBlockPtr(nullptr)
	, ManagedSize(MEMORY_SIZE)
	, AllocatedSize(0) 
{

}

JAMemoryManager* JAMemoryManager::Get()
{
	static JAMemoryManager MemoryManager;
	return &MemoryManager;
}

void* JAMemoryManager::GetBlock(unsigned int Size)
{
	JAMemoryBlock* SelectedFreeBlock = nullptr;

	bool bFound = false;
	std::list<JAMemoryBlock*>::iterator BeginIt = FreeBlocks.begin();
	std::list<JAMemoryBlock*>::const_iterator EndIt = FreeBlocks.end();
	while (!bFound && BeginIt != EndIt)
	{
		SelectedFreeBlock = (*BeginIt);
		bFound = SelectedFreeBlock->GetSize() >= Size;
		++BeginIt;
	}

	assert(bFound && "JAMemoryManager::GetBlock: No suitable block found");
	if (!bFound)
	{
		return nullptr;
	}

	JAMemoryBlock* AllocatedBlock = nullptr;
	// Update lists
	if (SelectedFreeBlock->GetSize() == Size)
	{
		// Swap
		FreeBlocks.remove(SelectedFreeBlock);
		UsedBlocks.push_back(SelectedFreeBlock);
		AllocatedBlock = SelectedFreeBlock;
	}
	else
	{
		// Add used block
		AllocatedBlock = GetNewMemoryBlock();
		AllocatedBlock->Init(SelectedFreeBlock->GetPtr(), Size);
		UsedBlocks.push_back(AllocatedBlock);

		// Update Free Block
		unsigned char* MovedPtr = SelectedFreeBlock->GetPtr() + Size;
		unsigned int ResizedSize = SelectedFreeBlock->GetSize() - Size;
		SelectedFreeBlock->Init(MovedPtr, ResizedSize);
	}

	AllocatedSize += Size;
	return AllocatedBlock->GetPtr();
}

void JAMemoryManager::FreeMemoryBlock(void* Ptr)
{
	JAMemoryBlock* MemoryBlock = nullptr;

	bool bFound = false;
	std::list<JAMemoryBlock*>::const_iterator ItUsedMemoryBlock = UsedBlocks.cbegin();
	while (!bFound && ItUsedMemoryBlock != UsedBlocks.cend())
	{
		MemoryBlock = *ItUsedMemoryBlock;
		bFound = MemoryBlock->GetPtr() >= Ptr;
		ItUsedMemoryBlock = bFound ? ItUsedMemoryBlock : ++ItUsedMemoryBlock;
	}

	assert(bFound && "JAMemoryManager::FreeMemoryBlock: Ptr not found");
	if (bFound)
	{
		AllocatedSize -= MemoryBlock->GetSize();
		UsedBlocks.remove(MemoryBlock);

		std::list<JAMemoryBlock*>::iterator ItFreeBlocks = FreeBlocks.begin();
		unsigned char* UCharPtr = reinterpret_cast<unsigned char*>(Ptr);

		while (ItFreeBlocks != std::prev(FreeBlocks.cend()) && UCharPtr > (*ItFreeBlocks)->GetPtr())
		{
			++ItFreeBlocks;
		}

		std::list<JAMemoryBlock*>::const_iterator ItNewInsertedFreeBlock = FreeBlocks.insert(ItFreeBlocks, MemoryBlock);

		//Resize forward memory if is continuous
		JAMemoryBlock* ForwardMemoryBlock = (*ItFreeBlocks);
		std::list<JAMemoryBlock*>::const_iterator ItFreeBlockEnd = FreeBlocks.cend();
		if (ItFreeBlocks != ItFreeBlockEnd && AreBlockContinuous(MemoryBlock, ForwardMemoryBlock))
		{
			MemoryBlock->Resize(MemoryBlock->GetSize() + ForwardMemoryBlock->GetSize());
			FreeBlocks.remove(ForwardMemoryBlock);
			UnUsedBlocks.push_back(ForwardMemoryBlock);
		}

		// Resize previous memory. First need to find the position of 
		if (ItNewInsertedFreeBlock != FreeBlocks.cbegin())
		{
			JAMemoryBlock* PreviousMemoryBlock = (*std::prev(ItNewInsertedFreeBlock));
			if (AreBlockContinuous(PreviousMemoryBlock, MemoryBlock))
			{
				PreviousMemoryBlock->Resize(PreviousMemoryBlock->GetSize() + MemoryBlock->GetSize());
				FreeBlocks.remove(MemoryBlock);
				UnUsedBlocks.push_back(MemoryBlock);
			}
		}
	}
}

void JAMemoryManager::CheckCoherence() const
{
	// Debug purposes
#ifdef _MEMORY_DEBUG
	
	// check free blocks order
	unsigned int PreviousUintPtr = 0;
	for (std::list<JAMemoryBlock*>::const_iterator it = FreeBlocks.cbegin(); it != FreeBlocks.cend(); ++it)
	{
		JAMemoryBlock* PreviousMemoryBlock = (*it);
		unsigned int CurrentPtr = reinterpret_cast<unsigned int>(PreviousMemoryBlock->GetPtr());
		assert(CurrentPtr > PreviousUintPtr);
		PreviousUintPtr = CurrentPtr;
	}

	// check used blocks duplicates
	for (std::list<JAMemoryBlock*>::const_iterator ItUsedBlockA = UsedBlocks.cbegin(); ItUsedBlockA != UsedBlocks.cend(); ++ItUsedBlockA)
	{
		unsigned char* PtrA = (*ItUsedBlockA)->GetPtr();
		std::list<JAMemoryBlock*>::const_iterator ItUsedBlockB = ItUsedBlockA;
		ItUsedBlockB++;
		for (ItUsedBlockB; ItUsedBlockB != UsedBlocks.cend(); ++ItUsedBlockB)
		{
			unsigned char* PtrB = (*ItUsedBlockB)->GetPtr();
			assert(PtrA != PtrB && "JAMemoryManager::CheckCoherence: Duplicated pointer found in UsedBlocks");
		}
	}
#endif
}

bool JAMemoryManager::AreBlockContinuous(const JAMemoryBlock* FirstBlock, const JAMemoryBlock* SecondBlock) const
{
	unsigned int FirstBlocWithSize = reinterpret_cast<unsigned int>(FirstBlock->GetPtr()) + FirstBlock->GetSize();
	unsigned int SecondBlockInt = reinterpret_cast<unsigned int>(SecondBlock->GetPtr());

	return FirstBlocWithSize == SecondBlockInt;
}


JAMemoryBlock* JAMemoryManager::GetNewMemoryBlock()
{
	JAMemoryBlock* NewBlock = std::move(*UnUsedBlocks.begin());
	assert(NewBlock && "JAMemoryManager::GetNewMemoryBlock: No memory blocks left");
	UnUsedBlocks.pop_front();
	return NewBlock;
}
