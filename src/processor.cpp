#include <iostream>
#include <fstream>
#include <string>
#include "linux_parser.h"
#include "processor.h"
#include <bits/stdc++.h>

using namespace std;

float Processor::Utilization() { 
 // NonIdle = user + nice + system + irq + softirq + steal
  float activeTime = LinuxParser::ActiveJiffies();
  // Idle = idle + iowait
  float idleTime = LinuxParser::IdleJiffies();
  // Total = Idle + NonIdle
  // CPU_Percentage = (Total - idled)/totald
  if ((activeTime + idleTime) == 0) {
    return 0.0;
  }
  return (activeTime / (activeTime + idleTime));
}