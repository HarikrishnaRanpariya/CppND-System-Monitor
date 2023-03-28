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
using std::set;
using std::size_t;
using std::string;
using std::vector;

#define OS_NAME_LINE_NUM     5
#define KERNEL_VERSION_NUM   3

static vector<Process> p_proc;
static Processor p_processor;

/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

bool myfunction (Process i,Process j) { return (i<j); }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
   
//     DIR *dir_ptr;
//     struct dirent *diread;
//     vector<char *> filenames;
//     if ((dir_ptr = opendir("//proc//")) != nullptr)
//     {
//         while ((diread = readdir(dir_ptr)) != nullptr) 
//         {
//             filenames.push_back(diread->d_name);
//         	// Defining a regular expression.
//           	string s = diread->d_name;
//     		std::regex r("\\s+");

// 		    // Using the regular expression to remove whitespaces.
//     		s = std::regex_replace(s, r, "");
// 			if(!s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end())
//             {	
//               //std::cout<<" filename = "<<s ;
//               Process tmp(s);
//              p_proc.push_back(tmp);
//             }
//         }
//         closedir(dir_ptr);
//         std::cout <<"\n";
//     }
//   	else 
//     {
//         perror("fail");
//         return p_proc;
//     }

  vector<int> pids = LinuxParser::Pids();
  
  for(int i : pids) {
     Process tmp(to_string(i));
     p_proc.push_back(tmp);
  }
  std::sort (p_proc.begin(), p_proc.end(), myfunction);
    
  return p_proc; 
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { 
  ifstream version_file;
  string tmp;
  
  version_file.open(LinuxParser::kProcDirectory + LinuxParser::kVersionFilename);
  
  int line_cntr = 0;
  if(version_file.is_open()) {
    while(!version_file.eof()) {
        line_cntr++;
    	getline(version_file, tmp, ' ');
      if(line_cntr == KERNEL_VERSION_NUM) break;
    }
    version_file.close();
  }

  return tmp; 
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { 

  return  LinuxParser::MemoryUtilization();
}

// TODO: Return the operating system name
std::string System::OperatingSystem() { 
  ifstream os_file;
  string tmp, title, value;
  
  os_file.open(LinuxParser::kOSPath);
  
  int line_cntr = 0;
  if(os_file.is_open()) {
    while(!os_file.eof()) {
      line_cntr++;
      getline(os_file, tmp);
      replace(tmp.begin(), tmp.end(), ' ', '_');
      replace(tmp.begin(), tmp.end(), '=', ' ');
      replace(tmp.begin(), tmp.end(), '*', ' ');
      if(line_cntr == OS_NAME_LINE_NUM) break;
    }
     
    istringstream linestream(tmp);
    linestream >> title >> value;
    replace(value.begin(), value.end(), '_', ' ');

    os_file.close();
  }
    
  return value;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() {  

   return LinuxParser::RunningProcesses();
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { 

  return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() {  
  std::ifstream uptime_file;
  string system_uptime ;
  uptime_file.open(LinuxParser::kProcDirectory + LinuxParser::kUptimeFilename);
  
  if(uptime_file.is_open()) {
    getline(uptime_file, system_uptime,' ');
    uptime_file.close();
  }
  
  return (long int)stoi(system_uptime);

}
