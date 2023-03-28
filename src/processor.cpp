#include <iostream>
#include <fstream>
#include <string>
#include "linux_parser.h"
#include "processor.h"
#include <bits/stdc++.h>

using namespace std;
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
 // NonIdle = user + nice + system + irq + softirq + steal
  float active_time = LinuxParser::ActiveJiffies();
  // Idle = idle + iowait
  float idle_time = LinuxParser::IdleJiffies();
  // Total = Idle + NonIdle
  // CPU_Percentage = (Total - idled)/totald
  if ((active_time + idle_time) == 0) {
    return 0.0;
  }
  return (active_time / (active_time + idle_time));
}