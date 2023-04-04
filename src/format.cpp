
#include <unistd.h>
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include "format.h"


using std::string;

using namespace std;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  int mins = (int) (seconds/60)%60;
  int hours = (int) ((seconds/60)/60)%24;
  int secs = (int) seconds%60;
  
   char res_str[25];
  sprintf(res_str, "%02d:%02d:%02d", hours, mins, secs);
  
  return res_str;
}