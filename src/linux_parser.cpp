#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <string>
#include <vector>
#include <iomanip>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

#define UID_ROW_INDEX         9
#define VMSIZE_ROW_INDEX      18 
#define JEFY_START_INDEX      13
#define MAX_JEFY_INDEX        17
#define STARTTIME_ROW_INDEX   22 //#22 starttime - Time when the process started, measured in clock ticks
#define TOTAL_MEM_INFO_NUM    9
#define FREE_MEM_INFO_NUM     10


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line="", key="", value="";
  
  std::ifstream filestream(kOSPath);
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);

      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }

  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os="", kernel="", version="", line="";
  std::ifstream stream(kProcDirectory + kVersionFilename);
  
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }

  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;

  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }

  closedir(directory);
  
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream mem_file;
  string value, title, totalMem, free_mem;
  float res =0.0;
                                        
  mem_file.open(LinuxParser::kProcDirectory + LinuxParser::kMeminfoFilename);
  
  int line_cntr = 0;
  if(mem_file.is_open()) {
    while(!mem_file.eof()) {
      line_cntr++;
      getline(mem_file, totalMem, ' ');

      if(line_cntr == TOTAL_MEM_INFO_NUM) break;
    }
    getline(mem_file, free_mem);
    
    line_cntr =0;
    while(!mem_file.eof()) {
      line_cntr++;
      getline(mem_file, free_mem, ' ');
  
      if(line_cntr == FREE_MEM_INFO_NUM) break;
    }
  
    res = (stof(totalMem)-stof(free_mem) )/stof(totalMem);    
    mem_file.close();
  }

   return res;                                       
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream uptime_file;
  string system_uptime ;
  uptime_file.open(kProcDirectory + kUptimeFilename);
  
  if(uptime_file.is_open()) {
    getline(uptime_file, system_uptime,' ');

    uptime_file.close();
  }
  
  return (long int)stoi(system_uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  
    return UpTime() * sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 

  std::ifstream stat_file;
  string data ="", value="";
  long active_jiffies = 0;

  stat_file.open(kProcDirectory + to_string(pid) + kStatFilename);

  if(stat_file.is_open()) {
    getline(stat_file, data);

     std::istringstream linestream(data);
     for (int i = 0; i < MAX_JEFY_INDEX; i++) {
        linestream >> value;
        if (i >= JEFY_START_INDEX) {
         active_jiffies += std::stol(value, NULL, 0);
        }
     }
    stat_file.close();
  }

  return active_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::ifstream stat_file;
  string total_processes, value;
  stat_file.open(kProcDirectory + kStatFilename);
  long active_jiffies = 0;
  
  if(stat_file.is_open()) {
    while(!stat_file.eof()) {
    	getline(stat_file, total_processes);
        break;
    }
    std::istringstream linestream(total_processes);
    linestream >> value;
    for (int i = 1; i < 10; i++) {
        linestream >> value;
        if (   (i != 4)
            && (i != 5)) {
         active_jiffies += std::stol(value, NULL, 0);
        }
      }
    stat_file.close();
  }

  return active_jiffies;

}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::ifstream stat_file;
  string total_processes, value;
  stat_file.open(kProcDirectory + kStatFilename);
  long idle_jiffies = 0;
  
  if(stat_file.is_open()) {
    while(!stat_file.eof()) {
    	getline(stat_file, total_processes);
        break;
    }

    std::istringstream linestream(total_processes);
    linestream >> value;
    for (int i = 1; i < 10; i++) {
        linestream >> value;
        if (   (i == 4)
            || (i == 5)) {

         idle_jiffies += std::stol(value, NULL, 0);
        }

      }
    stat_file.close();
  }

  return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 

  std::ifstream stat_file;//   
  string total_processes, cpu_name, value;
  stat_file.open(kProcDirectory + kStatFilename);
  vector<string> cpu_ut;
  
  if(stat_file.is_open()) {
    while(!stat_file.eof()) {
    	getline(stat_file, total_processes);
        break;
    }
    std::istringstream linestream(total_processes);
    linestream >> cpu_name;
    
    while(linestream >> value)
    {
      cpu_ut.push_back(value);
    }
  }
  
  return cpu_ut;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream stat_file;
  string total_processes;
  stat_file.open(kProcDirectory + kStatFilename);
  
  if(stat_file.is_open()) {
    while(!stat_file.eof()) {
    	getline(stat_file, total_processes, ' ');
        if(total_processes.compare("processes") == 0) break;

        getline(stat_file, total_processes);    
    }
    getline(stat_file, total_processes, ' ');
    stat_file.close();
  }
  
  return stoi(total_processes);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  std::ifstream stat_file;
  string procs_running;
  stat_file.open(kProcDirectory + kStatFilename);
  
  if(stat_file.is_open()) {
    while(!stat_file.eof()) {
    	getline(stat_file, procs_running, ' ');

        if(procs_running.compare("procs_running") == 0) break;

        getline(stat_file, procs_running);
    }
  
    getline(stat_file, procs_running, ' ');
    stat_file.close();
  }
    
  return stoi(procs_running);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  std::ifstream cmd_file;
  string cmd ="";

  cmd_file.open(kProcDirectory + to_string(pid) + kCmdlineFilename);

  if(cmd_file.is_open()) {
    getline(cmd_file, cmd);

    cmd_file.close();
  }
  
   return cmd;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::ifstream status_file;
  string start_time="", value="", title="";
  float res =0.0;
  int line_cntr = 0;

  status_file.open(kProcDirectory + to_string(pid) + kStatusFilename);

  if(status_file.is_open()) {
    while(!status_file.eof()) {
      line_cntr++;
      getline(status_file, start_time);

      if(line_cntr == VMSIZE_ROW_INDEX) break;
    }
   
    std::istringstream linestream(start_time);
    linestream >> title >> value;

    res = (stoi(value)/1024);
    
    status_file.close();
  }
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << res;

  return stream.str();; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::ifstream status_file;
  string title="", uid_data="", uid="";
  int line_cntr = 0;
  
  status_file.open(kProcDirectory + to_string(pid) + kStatusFilename);

  if(status_file.is_open()) {
    while(!status_file.eof()) {
      line_cntr++;
      getline(status_file, uid_data);

      if(line_cntr == UID_ROW_INDEX) break;
    }

    std::istringstream linestream(uid_data);
    linestream >> title >> uid;

    status_file.close();
  }
  
  return uid;

}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  std::ifstream passwd_file;
  string p_user_name="";
  string c_uid="", title="", raw_data="", tmp="", t_uid="";
  float res =0.0;
  int line_cntr = 0;

  c_uid = LinuxParser::Uid(pid);
  
  passwd_file.open(kPasswordPath);

  line_cntr = 0;
  if(passwd_file.is_open()) {
    while(!passwd_file.eof()) {
      line_cntr++;
      
      getline(passwd_file, raw_data);
      replace(raw_data.begin(), raw_data.end(), ':', ' ');
      
      std::istringstream linestream(raw_data);
      linestream >> title >> tmp >> t_uid;
      
      if(t_uid.compare(c_uid) == 0) {
        p_user_name = title;
        break;
      }  
    }
    passwd_file.close();
  }

  return p_user_name; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  std::ifstream uptime_file, stat_file;
  string system_uptime="",start_time = "",total_processes="";
  int line_cntr = 0;

  stat_file.open(kProcDirectory + to_string(pid) + kStatFilename);

  if(stat_file.is_open()) {
    while(!stat_file.eof()) {
      line_cntr++;
      getline(stat_file, start_time, ' ');

      if(line_cntr == STARTTIME_ROW_INDEX) break;
    }
    stat_file.close();
  }

  uptime_file.open(kProcDirectory + kUptimeFilename);  

  if(uptime_file.is_open()) {
    getline(uptime_file, system_uptime,' ');
    uptime_file.close();
  }

  long  res = (long )(stoi(system_uptime) - stoi(start_time));

  if(res < 0) {
     return (-1)*res;
  }
  else {
    return res;
  }
}
