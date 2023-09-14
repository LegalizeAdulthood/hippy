/*
 * Can BAŞARAN
 * 200702034
 */
 
#ifndef _STACH_123_H__
#define _STACH_123_H__

#include "sym_table.h"

/*
 * *TOKEN stack for compound stmt
 *
 */
 

template<class T>
class stack{
private:
  T **mem;
  int size;
public:
  stack(){size=0;mem=NULL;}
  ~stack(){ free(mem);}
  bool isempty(){
    return size==0;
  }
  bool top(T **ppToken){
    if(size){
      *ppToken = mem[size-1];
      return true;
    }
    return false;
  }
  bool pop(T **ppToken){
    if(size){ 
      size--;
      if(ppToken) *ppToken=mem[size];
      mem = (T**)realloc(mem, sizeof(T*)*size);
      return true;
     }
     return false;
  }
  
  void push(T *pToken){
    size++;
    mem = (T**)realloc(mem, sizeof(T*)*size);
    mem[size-1] = pToken;
  }
};

/*
class stack<T>{
private:
  TOKEN **mem;
  int size;
public:
  stack(){size=0;mem=NULL;}
  ~stack(){ free(mem);}
  bool isempty(){
    return size==0;
  }
  bool top(TOKEN **ppToken){
    if(size){
      *ppToken = mem[size-1];
      return true;
    }
    return false;
  }
  bool pop(TOKEN **ppToken){
    if(size){ 
      size--;
      if(ppToken) *ppToken=mem[size];
      mem = (TOKEN**)realloc(mem, sizeof(TOKEN*)*size);
      return true;
     }
     return false;
  }
  
  void push(TOKEN *pToken){
    size++;
    mem = (TOKEN**)realloc(mem, sizeof(TOKEN*)*size);
    mem[size-1] = pToken;
  }
};
*/
#endif
