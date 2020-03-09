#include "BuddyAllocator.h"
#include <iostream>
#include <cmath>
using namespace std;

//From geek for geek 
bool isPowerOfTwo(int n) 
{ 
   if(n==0) 
   return false; 
  
   return (ceil(log2(n)) == floor(log2(n))); 
} 

//From geek for geek 
unsigned int nextPowerOf2(unsigned int n)  
{  
    unsigned count = 0;  
     if (n && !(n & (n - 1)))  
        return n;  
      
    while( n != 0)  
    {  
        n >>= 1;  
        count += 1;  
    }  
      
    return 1 << count;  
}  
  
BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length) {

  basic_block_size = (isPowerOfTwo (basic_block_size))? _basic_block_size:nextPowerOf2(_basic_block_size);
  total_memory_size = (isPowerOfTwo(total_memory_size))?  _total_memory_length:nextPowerOf2(_total_memory_length);
  int index = log2(total_memory_size / basic_block_size);

  remainingMemory = total_memory_size;

  FreeList.resize(index + 1);

  char* memory = new char[total_memory_size];
  start = memory;
  BlockHeader* bh = (BlockHeader*) memory;
  bh->Free = true;
  bh->block_size = total_memory_size;
  FreeList[FreeList.size() - 1].insert(bh);


}

BuddyAllocator::~BuddyAllocator () {
  delete[] start;

}

void* BuddyAllocator::alloc(int length) {
  /* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
  */
 
  int memoryNeeded = length  + sizeof(BlockHeader); 
  if(!isPowerOfTwo(memoryNeeded)){
    memoryNeeded = nextPowerOf2(memoryNeeded);
  }
  if(memoryNeeded < basic_block_size){

    memoryNeeded = basic_block_size;
  }


  // if (memoryNeeded > remainingMemory) {
  //   //cout<<"no memory";
  //   return nullptr;
  //   // return nullptr;
  // }
  int size;
  int listindex = -1;
  int minIndex = log2(memoryNeeded) - log2(basic_block_size);
  for (int i = minIndex; i < FreeList.size(); i++) {
    if (FreeList[i].size > 0 
      && basic_block_size * pow(2, i) > 0 && FreeList[i].head !=nullptr
      &&FreeList[i].head->block_size >= memoryNeeded) {
      size = basic_block_size * pow(2, i);
      listindex = i;
      break;
    }
  }
  if(listindex == -1){
     cout<<"no memory in the block.Pls increase memeory size."<<endl;
     return nullptr;
  //   // return nullptr;
  }

  BlockHeader* add;


  //cout<<"min: "<<minIndex;
  if (listindex == minIndex) {
    //cout<<"run min index: "<<endl;
    add = FreeList[minIndex].head;
    FreeList[listindex].remove(add);
    add->Free = false;
    remainingMemory -= memoryNeeded;
    return (char*)add + sizeof(BlockHeader);

  } else {
    //cout<<"run while: "<<endl;
    while (size / 2 >= memoryNeeded && size / 2 >= basic_block_size) {



      split(FreeList[listindex].head);
      listindex -= 1;
      size = size / 2;

      add = FreeList[listindex].head;
    }
  }
 // cout<<listindex<<endl;

  FreeList[listindex].remove(add);
  add->Free = false;
  remainingMemory -= memoryNeeded;



  return (char*)add + sizeof(BlockHeader);
}

void BuddyAllocator::free(void* a) {

  char* tobeFree = (char*)a;
  tobeFree = tobeFree - sizeof(BlockHeader);
  BlockHeader* bl = (BlockHeader*) tobeFree;

  int index = log2(bl->block_size) - log2(basic_block_size);
  bl->Free = true;

  for (int i = index; i < FreeList.size() ; i++) {

    BlockHeader* buddy = getbuddy(bl);
    //cout<<bl<<" "<<buddy<<endl;
    //cout <<"buddy->block_size == bl->block_size: "<<(int)(buddy->block_size == bl->block_size) <<" buddy->Free ==true; "<<int(buddy->Free ==true)<<endl;
    if (arebuddies(bl,buddy)) {



      bl = merge(bl, buddy);

    } else {

      bl->Free = true;
      FreeList[i].insert(bl);
      remainingMemory += bl->block_size;
      break;
    }
  }






}

void BuddyAllocator::printlist (){
  cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
  int64_t total_free_memory = 0;
  for (int i=0; i<FreeList.size(); i++){
    int blocksize = ((1<<i) * basic_block_size); // all blocks at this level are this size
    cout << "[" << i <<"] (" << blocksize << ") : ";  // block size at index should always be 2^i * bbs
    int count = 0;
    BlockHeader* b = FreeList [i].head;
    // go through the list from head to tail and count
    while (b){
      total_free_memory += blocksize;
      count ++;
      // block size at index should always be 2^i * bbs
      // checking to make sure that the block is not out of place
      if (b->block_size != blocksize){
        cout<<b->block_size<<" "<<blocksize<<endl;;
        cerr << "ERROR:: Block is in a wrong list" << endl;
        exit (-1);
      }
      b = b->next;
    }
    cout << count << endl;
    cout << "Amount of available free memory: " << total_free_memory << " byes" << endl;  
  }
}

BlockHeader* BuddyAllocator::split (BlockHeader* block) {
  // if(block == nullptr){
  //   cout<<"nullptr"<<endl;
  // }

  BlockHeader* block1 = block;
  BlockHeader* block2;

  int index = log2(block->block_size) - log2(basic_block_size);
  // cout<<"index: "<<index<<endl;;
  block1->block_size = block->block_size/2;

  char* temp = (char*)block;
  block2 = (BlockHeader*)(temp + block1->block_size);
  block2->Free = true;
  FreeList[index].remove(block);
  block2->block_size = block1->block_size;
  FreeList[index-1].insert(block2);
  FreeList[index-1].insert(block1);

  return block1;



}
BlockHeader* BuddyAllocator::merge (BlockHeader* block1, BlockHeader* block2) {


  int index = log2(block1->block_size) - log2(basic_block_size);
  //cout<<"index: "<<index<<endl;
  //cout<<"before FreeList[index].remove(block1): "<<endl;

  //cout<<"after FreeList[index].remove(block1): "<<endl;
  //cout<<"before FreeList[index].remove(block2): "<<endl;
  FreeList[index].remove(block2);
  FreeList[index].remove(block1);
  // cout<<"block1: "<<block1<<endl;
  // if(block1 == nullptr){
  //   cout<<"block1 is nullptr"<<endl;
  // }
  // cout<<"block2: "<<block2<<endl;
  // if(block2 == nullptr){
  //   cout<<"block2 is nullptr"<<endl;
  // }
 // cout<<"after FreeList[index].remove(block2): "<<endl;
  BlockHeader* block = ((char*)block1 > (char*)block2) ? block2 : block1;
  block->block_size = block1->block_size * 2;
  block->Free = true;


  return block;
}
BlockHeader* BuddyAllocator::getbuddy (BlockHeader * addr){
  return (BlockHeader*)((((char*)addr - start)^addr->block_size)+start);
}
bool BuddyAllocator::arebuddies (BlockHeader* bl, BlockHeader* buddy){
  return buddy->block_size == bl->block_size and buddy->Free ==true;
}

