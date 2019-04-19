#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iomanip>
#include <cassert>
#include <map>
#include "hybrid.h"

int PREDICTOR_TYPE;

typedef unsigned long ulong;
using namespace std;

bool read_command_line_input (int argc, char *argv[], int *m1, int *m2, int *h,int *k, FILE ** input_file){
  char * file_name;
  if((argc > 7) || (argc < 4)) {
    cout << "Enter valid number of arguments." << endl;
    cout << "\nUsage:\n\t\t./sim bimodal <M2> <tracefile>\nor\t\t./sim gshare <M1> <N> <tracefile>\nor\t\t./sim hybrid <K> <M1> <N> <M2> <tracefile>\n\n";
    return false;
  }

  if(strcmp(argv[1],"bimodal") == 0) PREDICTOR_TYPE = BIMODAL;
  else if(strcmp(argv[1],"gshare") == 0) PREDICTOR_TYPE = GSHARE;
  else if(strcmp(argv[1],"hybrid") == 0) PREDICTOR_TYPE = HYBRID;

  switch(PREDICTOR_TYPE){
  case GSHARE:
	*h = atoi(argv[3]); //Converts the string from command line to integer pointer
    *m2 = atoi(argv[2]); //Converts the string from command line to integer pointer
    if (*h > *m2){ 
      cout << "Value of h is greater than m2" << endl;
      return false;
    }
    file_name = argv[4];
    cout << "COMMAND\n./sim " << argv[1] << " " << *m2 << " " << *h  << " " << file_name << endl;
    break;
	
  case HYBRID:
    *k = atoi(argv[2]); //Converts the string from command line to integer pointer
	*h = atoi(argv[4]); //Converts the string from command line to integer pointer
	*m1 = atoi(argv[5]); //Converts the string from command line to integer pointer
    *m2 = atoi(argv[3]); //Converts the string from command line to integer pointer
    if( *h > *m2){ 
      cout << "Value of h is greater than m2" << endl;
      return false;
    }
    file_name = argv[6];
    cout << "COMMAND\n./sim " << argv[1] << " "<< *k << " " << *m2 << " " << *h  << " " << *m1  << " "<< file_name << endl;
    break;
	
  case BIMODAL:
	file_name = argv[3];
    *m1 = atoi(argv[2]); //Converts the string from command line to integer pointer
    cout << "COMMAND\n./sim " << argv[1] << " " <<  *m1 << " " << file_name << endl; 
    break;

  default:
    cout << "\nEnter valid predictor type." << endl;
    return false;
  }


  assert(file_name);
  *input_file = fopen(file_name, "r+"); //Reads the input file
  if(*input_file == 0) {
    cout << "\nError occured in reading the trace file." <<endl;
    return false;
  }
  return true;
}

int branch_addr(char a_branch){
  if(a_branch == 't') return TAKEN;
  else return NOTTAKEN;
}

int main(int argc, char *argv[]){
  FILE * input_file;
  ulong address;
  int m1, m2, h, k;
  char a_branch;
  
  bool is_success = read_command_line_input(argc, argv, &m1, &m2, &h, &k, &input_file);

  if(!is_success) return 0;
  hybrid * H = new hybrid;
  bimodal * b = new bimodal;
  gshare * g = new gshare;

  assert(PREDICTOR_TYPE <= 2);

  switch(PREDICTOR_TYPE){
	  
  case GSHARE:
      g->init(m2,h);  //Calls for a mehtod in gshare.h
      while(1) {
	if(fscanf(input_file, "%lx %c", &address, &a_branch) != EOF)
	  g->access(address, branch_addr(a_branch)); //Calls for a mehtod in gshare.h
	else break;
      }
      g->print_output(); //Calls for a mehtod in gshare.h
      g->print_stats(); //Calls for a mehtod in gshare.h
      break;
	  
  case HYBRID:
      H->init(m1, m2, h, k); //Calls for a mehtod in hybrid.h
      while(1) {
	if(fscanf(input_file, "%lx %c", &address, &a_branch) != EOF)
	  H->access(address, branch_addr(a_branch)); //Calls for a mehtod in hybrid.h
	else break;
      }
     H->print_output(); //Calls for a mehtod in hybrid.h
      H->print_stats(); //Calls for a mehtod in hybrid.h
      break;
  
  case BIMODAL: 
    b->init(m1); //Calls for a mehtod in bimodal.h
    while(1) {
      if(fscanf(input_file, "%lx %c", &address, &a_branch) != EOF)
	  b->access(address, branch_addr(a_branch)); //Calls for a mehtod in bimodal.h
	else break;
      }
      b->print_output(); //Calls for a mehtod in bimodal.h
      b->print_stats(); //Calls for a mehtod in bimodal.h
      break;
	  
  default:
      cout << "Not a valid predictor type" << endl;
    
    }
    return 1;
  }