
#include <iostream>
#include "JAMemoryManager.h"

using namespace JAPR;

void TestingMemoryManager()
{
    JAMemoryManager* MemoryManager = JAMemoryManager::Get();
    const int NumAllocations = 64;

    void* pointers[NumAllocations];
    int rands[NumAllocations];

    // Allocate sequencial 
    for (int i = 0; i < NumAllocations; ++i)
    {
        pointers[i] = MemoryManager->GetBlock(16);
        rands[i] = i;
    }

    // deallocate sequencial
    for (int i = 0; i < NumAllocations; ++i)
    {
        MemoryManager->FreeMemoryBlock(pointers[rands[i]]);
    }


    // Allocate sequencial with different size
    void* OtherPtrTest[6];
    OtherPtrTest[0] = MemoryManager->GetBlock(16);
    OtherPtrTest[1] = MemoryManager->GetBlock(17);
    OtherPtrTest[2] = MemoryManager->GetBlock(18);
    OtherPtrTest[3] = MemoryManager->GetBlock(19);
    OtherPtrTest[4] = MemoryManager->GetBlock(50);
    OtherPtrTest[5] = MemoryManager->GetBlock(1);

    // deallocate random
    MemoryManager->FreeMemoryBlock(OtherPtrTest[3]);
    MemoryManager->FreeMemoryBlock(OtherPtrTest[2]);
    MemoryManager->FreeMemoryBlock(OtherPtrTest[5]);
    MemoryManager->FreeMemoryBlock(OtherPtrTest[0]);
    MemoryManager->FreeMemoryBlock(OtherPtrTest[1]);
    MemoryManager->FreeMemoryBlock(OtherPtrTest[4]);

    MemoryManager->CheckCoherence();
}

void PracticalExampleWithPtrIntegers()
{
    using namespace std;
    // Practical example.
    // Take into account that you should initialize each block or may be pointing to old/trash values.

    JAMemoryManager* MemoryManager = JAMemoryManager::Get();
    cout << "- Allocate new block PtrInt1 \n";
    int* PtrInt1 = reinterpret_cast<int*>(MemoryManager->GetBlock(sizeof(int)));
    *PtrInt1 = 555;
    cout << "PtrInt1 has the value: " << *PtrInt1 << " and the memory address is: 0x" << PtrInt1 << "\n";
    MemoryManager->FreeMemoryBlock(PtrInt1);
    cout << "- Deallocate PtrInt1 \n";

    cout << "- Allocate new block PtrInt2 \n";
    int* PtrInt2 = reinterpret_cast<int*>(MemoryManager->GetBlock(sizeof(int)));
    cout << "PtrInt2 has the value: " << *PtrInt2 << " and the memory address is: 0x" << PtrInt2 << "\n";

    // Free and allocate again, then look at the value
    cout << "- Allocate new block PtrInt3 \n";
    int* PtrInt3 = reinterpret_cast<int*>(MemoryManager->GetBlock(sizeof(int)));
    *PtrInt3 = 3333;
    cout << "PtrInt3 has the value: " << *PtrInt3 << " and the memory address is: 0x" << PtrInt3 << "\n";
    MemoryManager->FreeMemoryBlock(PtrInt3);
    cout << "- Deallocate PtrInt3 \n";

    MemoryManager->FreeMemoryBlock(PtrInt2);
    cout << "- Deallocate PtrInt2 \n";

    cout << "- Allocate new block PtrInt4 \n";
    int* PtrInt4 = reinterpret_cast<int*>(MemoryManager->GetBlock(sizeof(int)));
    cout << "PtrInt4 has the value: " << *PtrInt4 << " and the memory address is: 0x" << PtrInt4 << "\n";
    MemoryManager->FreeMemoryBlock(PtrInt4);
    cout << "- Deallocate PtrInt4 \n";

    MemoryManager->CheckCoherence();
}

int main(int argc, char* argv[])
{
    PracticalExampleWithPtrIntegers();
    //TestingMemoryManager();

    return 0;
}

