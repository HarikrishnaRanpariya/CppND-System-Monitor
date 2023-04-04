#include <iostream>
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <fstream> 
#include <algorithm>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using namespace std;
using std::string;
using std::to_string;
using std::vector;

#define UTIME_ROW_INDEX       14 //#14 utime - CPU time spent in user code, measured in clock ticks
#define STARTTIME_ROW_INDEX   22

int Process::Pid() { return this->p_pid; }

float Process::CpuUtilization() const {
  std::ifstream statFile;
  string data ="", value="";

    
  long activeJiff = LinuxParser::ActiveJiffies();
  long idleJiff = LinuxParser::IdleJiffies();
  
  statFile.open(LinuxParser::kProcDirectory + to_string(this->p_pid) + LinuxParser::kStatFilename);
  int line_cntr = 0;
  if(statFile.is_open()) {
     while(!statFile.eof()) {
      line_cntr++;
      getline(statFile, data,' ');

      if(line_cntr == STARTTIME_ROW_INDEX) break;
     }
    statFile.close();
  }
  return (LinuxParser::ActiveJiffies(this->p_pid)) /(activeJiff + idleJiff);
   
}

long int Process::UpTime() { 
  return LinuxParser::UpTime(this->p_pid);
 }

string Process::Ram() const { 
  return  LinuxParser::Ram(this->p_pid);
}

string Process::Command() { 
  return  LinuxParser::Command(this->p_pid); 
}

string Process::User() { 
  this->p_user_name =  LinuxParser::User(this->p_pid);

  return this->p_user_name;
}

Process::Process(std::string t_path) {
  this->p_pid = stoi(t_path);
  this->path = LinuxParser::kProcDirectory + t_path+"//";
}
bool Process::operator<(Process const& a) const { 
  if(this->CpuUtilization() < a.CpuUtilization()) {
    return true; 
  }
  else {
    return false;
  }
}