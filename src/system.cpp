#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <regex>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using namespace std; 

#define OS_NAME_LINE_NUM     5
#define KERNEL_VERSION_NUM   3

static vector<Process> pProc;

/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

Processor& System::Cpu() { return cpu_; }

bool myfunction (Process i,Process j) { return (i<j); }

vector<Process>& System::Processes() { 
   
  vector<int> pids = LinuxParser::Pids();
  
  for(int i : pids) {
     pProc.emplace_back(to_string(i));
  }
  std::sort (pProc.begin(), pProc.end(), myfunction);
    
  return pProc;
}

std::string System::Kernel() { 
  ifstream versionFile;
  string tmp;
  
  versionFile.open(LinuxParser::kProcDirectory + LinuxParser::kVersionFilename);
  
  int lineCntr = 0;
  if(versionFile.is_open()) {
    while(!versionFile.eof()) {
        lineCntr++;
        getline(versionFile, tmp, ' ');
      if(lineCntr == KERNEL_VERSION_NUM) break;
    }
    versionFile.close();
  }

  return tmp; 
}

float System::MemoryUtilization() { 

  return  LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() { 
  ifstream osFile;
  string tmp, title, value;
  
  osFile.open(LinuxParser::kOSPath);
  
  int lineCntr = 0;
  if(osFile.is_open()) {
    while(!osFile.eof()) {
      lineCntr++;
      getline(osFile, tmp);
      replace(tmp.begin(), tmp.end(), ' ', '_');
      replace(tmp.begin(), tmp.end(), '=', ' ');
      replace(tmp.begin(), tmp.end(), '*', ' ');
      if(lineCntr == OS_NAME_LINE_NUM) break;
    }
     
    istringstream linestream(tmp);
    linestream >> title >> value;
    replace(value.begin(), value.end(), '_', ' ');

    osFile.close();
  }
    
  return value;
}

int System::RunningProcesses() {  

   return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() { 

  return LinuxParser::TotalProcesses();
}

long int System::UpTime() {  
  std::ifstream uptimeFile;
  string systemUptime;
  uptimeFile.open(LinuxParser::kProcDirectory + LinuxParser::kUptimeFilename);
  
  if(uptimeFile.is_open()) {
    getline(uptimeFile, systemUptime,' ');
    uptimeFile.close();
  }
  
  return (long int)stoi(systemUptime);

}
