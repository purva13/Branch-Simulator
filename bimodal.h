#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include <math.h>
#include<map>
#include <iomanip>


#ifndef COMM_ENUMS
#define COMM_ENUMS
#include "common_enums.h"
#endif

using namespace std;
typedef unsigned long ulong;

class bimodal{
	
 protected:

  map<int,int> p_table;
  ulong index, predictions, mispredictions;
  int mask, p_branch, size; 
  map<int,int>::iterator it;
	
 public:
  void init(int m1) {
	
	size = (int) pow(2, (double) m1); //Initializes the size to 2 the power of m
	mask  = size - 1 ;
    index = predictions = mispredictions = 0; //Initializes the index, predicitons, mispredicitons to zero
    for (int i = 0; i < size; i++) p_table[i] = WEAKLY_TAKEN; 
  }

  void get_index(ulong address){
    index = address >> 2; //Right shift the index with 2
    index = index & mask; 
  }

  int access_hybrid(ulong address, int a_branch){
    return p_update(a_branch, p_branch);
  }

  int access(ulong address, int a_branch){
    predictions++;
    get_index(address);
    p_branch = p_table.at(index) >= 4 ? TAKEN : NOTTAKEN;   //Checks the index value in the table
    return p_update(a_branch, p_branch);
  }
  
  int p_update(int a_branch, int p_branch) {
    switch (a_branch){
	case NOTTAKEN:
      if(p_branch == TAKEN) {
	mispredictions++;
      }
	  if(p_table.at(index) > 0) {
	p_table.at(index)--;
	return 0;
      }
      break;
	  
    case TAKEN:
    if(p_branch == NOTTAKEN) {
	mispredictions++;
      }
	if(p_table.at(index) < 7) {
	p_table.at(index)++;
	return 0;
      }
      break; 
    }
    return 1;
  }
  void print_stats(){
    cout << "FINAL BIMODAL CONTENTS\n";
    for (it=p_table.begin(); it!=p_table.end(); ++it) 
      cout << it->first << "\t" << it->second << "\n"; 
  }
  
  int is_taken(ulong address){
    get_index(address);
    return p_table.at(index) >= 4? TAKEN : NOTTAKEN;    
  }
  
  void print_output(){
    cout << "OUTPUT\nnumber of predictions: " << predictions << "\nnumber of mispredictions: " << mispredictions << fixed << setprecision(2) << "\nMisprediction rate: " << (double)mispredictions*100/predictions << "%\n";
  }

  
};

