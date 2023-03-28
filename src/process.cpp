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

// TODO: Return this process's ID
int Process::Pid() { return this->p_pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
  std::ifstream stat_file;
  string data ="", value="";

    
  long active_j = LinuxParser::ActiveJiffies();
  long idle_j = LinuxParser::IdleJiffies();
  
  stat_file.open(LinuxParser::kProcDirectory + to_string(this->p_pid) + LinuxParser::kStatFilename);
  int line_cntr = 0;
  if(stat_file.is_open()) {
     while(!stat_file.eof()) {
      line_cntr++;
      getline(stat_file, data,' ');

      if(line_cntr == STARTTIME_ROW_INDEX) break;
     }
    stat_file.close();
  }
  return (LinuxParser::ActiveJiffies(this->p_pid)) /(active_j +idle_j); //std::stof(data);// 
   
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
  return LinuxParser::UpTime(this->p_pid);
 }

// TODO: Return this process's memory utilization
string Process::Ram() const { 
  return  LinuxParser::Ram(this->p_pid);
}

// TODO: Return the command that generated this process
string Process::Command() { 
  return  LinuxParser::Command(this->p_pid); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
  this->p_user_name =  LinuxParser::User(this->p_pid);

  return this->p_user_name;
}

Process::Process(std::string t_path) {
  this->p_pid = stoi(t_path);
  this->path = LinuxParser::kProcDirectory + t_path+"//";
}
// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  if(this->CpuUtilization() < a.CpuUtilization()) {
    return true; 
  }
  else {
    return false;
  }
}