#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include<map>

#include "bimodal.h"
#include "gshare.h"


typedef unsigned long ulong;

class hybrid : public bimodal, public gshare {
	
 protected:
  map<int, int> chooser_table;
  ulong index, predictions, mispredictions;
  bool g_predict, b_predict;
  int mask, p_branch, size;
  map<int, int>::iterator it;
  bimodal * b;
  gshare * g;
	
 public:
  void init(int m1, int m2, int h, int k) {
    b = new bimodal;
    b->init(m1);
	g = new gshare;
    g->init(m2,h);
	
    index = predictions = mispredictions = 0; //Initializes the index, predicitons, mispredicitons to zero
    size = (int) pow(2, (double) k); //Initializes the size to 2 the power of m
	mask = size - 1;
    for (int i = 0; i < size; i++) chooser_table[i] = 1; 
  }

  void if_misprediction(bool prediction, int a_branch){
    if ((a_branch == TAKEN && !prediction) || (a_branch == NOTTAKEN && prediction))
      mispredictions++; 
  }

  bool if_correct_prediction(bool prediction, int a_branch){
    return (((a_branch == NOTTAKEN && !prediction) || (a_branch == TAKEN && prediction)));    
  }
  
void get_index(ulong address){
    index = address >> 2; //Right shift the address with 2
    index = index & mask;    
  }

  void print_output(){
    cout << "OUTPUT\nnumber of predictions: " << predictions << "\nnumber of mispredictions: " << mispredictions << fixed << setprecision(2) << "\nMisprediction rate: " << (double)mispredictions*100/predictions << "%\n";    
  }
  
  int access(ulong address, int a_branch){
    predictions++;
	b_predict = b->is_taken(address);
    get_index(address);
    g_predict = g->is_taken(address);
	
    if(chooser_table.at(index) < 2){
      if_misprediction(b_predict, a_branch);
      g->update_gbhr(a_branch);
      b->access(address, a_branch);
    } else {
      if_misprediction(g_predict, a_branch);
      g->access(address, a_branch); 
    }

    bool b_correct = if_correct_prediction(b_predict, a_branch);
    bool g_correct = if_correct_prediction(g_predict, a_branch);

    if(b_correct && !g_correct)
      if(chooser_table.at(index) > 0)
			chooser_table.at(index)--;

    if(g_correct && !b_correct)
      if(chooser_table.at(index) < 3)
			chooser_table.at(index)++;    

    return 1;
  }

  void print_stats(){
    cout << "FINAL CHOOSER CONTENTS\n";
    for (it=chooser_table.begin(); it!=chooser_table.end(); ++it) 
      cout << it->first << "\t" << it->second << "\n";
   g->print_stats();
   b->print_stats();
  }
  
};