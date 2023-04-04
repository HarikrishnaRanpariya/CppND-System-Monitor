
#include <unistd.h>
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include "format.h"


using std::string;

using namespace std;

#define MAX_MINS_IN_ONE_HOUR   (60)
#define MAX_SECS_IN_ONE_MIN    (60)
#define MAX_HOURS_IN_ONE_DAY   (24)
#define SECS_PER_MIN           (60)
#define MINS_PER_HOUR          (60)

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int TotalMins = (seconds / SECS_PER_MIN);
  int TotalHours = (TotalMins / MINS_PER_HOUR);

  int mins  = (int) (TotalMins % MAX_MINS_IN_ONE_HOUR);
  int hours = (int) (TotalHours % MAX_HOURS_IN_ONE_DAY);
  int secs  = (int) (seconds % MAX_SECS_IN_ONE_MIN);
  
  char res_str[8];
  sprintf(res_str, "%02d:%02d:%02d", hours, mins, secs);
  
  return res_str;
}