#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iomanip>
#include<map>

typedef unsigned long ulong;

using namespace std;

class gshare{

 protected:
  ulong index, predictions, mispredictions;
  map<int,int>::iterator it;
  int mask1, mask2, gbhr, m2, p_branch, size, h;
  map<int,int> p_table;
  

 public:
 
 void get_index(ulong address){
    ulong temp;
    temp = address >> 2;
    temp = temp & mask1;
    if (h > 0) {
	 unsigned long lower_n;
	 lower_n = temp & mask2;
	 lower_n = lower_n ^ gbhr;
	 temp = temp &(~mask2);
	 index = temp | lower_n;
    }
    else
      index = temp;
  }
  
  void init(int m2, int h) {
	index = predictions = mispredictions = 0; //Initializes the index, predicitons, mispredicitons to zero
	gbhr = 0;
    this->m2 = m2;
    this->h = h;
    
	size = (int) pow(2, (double) m2); //Initializes the size to 2 the power of m
	mask1 = size - 1;
    mask2 = (int) pow(2, (double) h) - 1;
    for (int i = 0; i < size; i++)
	{
		p_table[i] = WEAKLY_TAKEN;
	}
  }

  int access(ulong address, int a_branch){
    predictions++;
    get_index(address);
    p_branch = p_table.at(index) >= 4 ? TAKEN : NOTTAKEN; //Checks the index value in the table
    return p_update(a_branch, p_branch); 
  }

void update_gbhr(int i){
    if (h > 0) {
      gbhr = gbhr >> 1; //Right shift the gbhr with 1
      gbhr = gbhr | (i << (h - 1));
    }
  }
  
  int p_update(int a_branch, int p_branch){
    switch (a_branch){
    case NOTTAKEN:
      if(p_table.at(index) > 0) {
	p_table.at(index)--;
      }
      update_gbhr(0);
      if(p_branch == TAKEN) {
	mispredictions++;
	return 0;
      }
      break; 
	  
	case TAKEN:
      if(p_table.at(index) < 7) {
	p_table.at(index)++;
      }
      update_gbhr(1);
      if(p_branch == NOTTAKEN) {
	mispredictions++;
	return 0;
      }
      break;
    }
    return 1;
  }
  
  void print_stats(){
    cout << "FINAL GSHARE CONTENTS\n";
    for (it=p_table.begin(); it!=p_table.end(); ++it) 
      cout << it->first << "\t" << it->second << "\n";
  }
  
  int is_taken(ulong address){
    get_index(address);
    return p_table.at(index) >=4 ? TAKEN : NOTTAKEN;
  }

  void print_output(){
    cout << "OUTPUT\nNumber of predictions: " << predictions << "\nNumber of mispredictions: " << mispredictions << fixed << setprecision(2) << "\nMisprediction rate: " << (double)mispredictions*100/predictions << "%\n";
  }
 
};

