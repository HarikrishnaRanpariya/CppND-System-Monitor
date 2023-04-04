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
        if (key.compare(kPrettyName) == 0) {
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

float LinuxParser::MemoryUtilization() {
  std::ifstream memFile;
  string value, title, totalMem, freeMem;
  float res =0.0;
                                        
  memFile.open(LinuxParser::kProcDirectory + LinuxParser::kMeminfoFilename);
  
  int lineCntr = 0;
  if(memFile.is_open()) {
    while(!memFile.eof()) {
      lineCntr++;
      getline(memFile, totalMem, ' ');

      if(lineCntr == TOTAL_MEM_INFO_NUM) break;
    }
    getline(memFile, freeMem);
    
    lineCntr =0;
    while(!memFile.eof()) {
      lineCntr++;
      getline(memFile, freeMem, ' ');
  
      if(lineCntr == FREE_MEM_INFO_NUM) break;
    }
  
    res = (stof(totalMem)-stof(freeMem) )/stof(totalMem);
    memFile.close();
  }

   return res;                                       
}

long LinuxParser::UpTime() {
  std::ifstream uptimeFile;
  string systemUptime ;
  uptimeFile.open(kProcDirectory + kUptimeFilename);
  
  if(uptimeFile.is_open()) {
    getline(uptimeFile, systemUptime,' ');

    uptimeFile.close();
  }
  
  return (long int)stoi(systemUptime);
}

long LinuxParser::Jiffies() { 
  
    return UpTime() * sysconf(_SC_CLK_TCK);
}

long LinuxParser::ActiveJiffies(int pid) { 

  std::ifstream statFile;
  string data ="", value="";
  long activeJiffies = 0;

  statFile.open(kProcDirectory + to_string(pid) + kStatFilename);

  if(statFile.is_open()) {
    getline(statFile, data);

     std::istringstream linestream(data);
     for (int i = 0; i < MAX_JEFY_INDEX; i++) {
        linestream >> value;
        if (i >= JEFY_START_INDEX) {
         activeJiffies += std::stol(value, NULL, 0);
        }
     }
    statFile.close();
  }

  return activeJiffies;
}

long LinuxParser::ActiveJiffies() { 
  std::ifstream statFile;
  string totalProcesses, value;
  statFile.open(kProcDirectory + kStatFilename);
  long activeJiffies = 0;
  
  if(statFile.is_open()) {
    while(!statFile.eof()) {
        getline(statFile, totalProcesses);
        break;
    }
    std::istringstream linestream(totalProcesses);
    linestream >> value;
    for (int i = 1; i < 10; i++) {
        linestream >> value;
        if (   (i != 4)
            && (i != 5)) {
         activeJiffies += std::stol(value, NULL, 0);
        }
      }
    statFile.close();
  }

  return activeJiffies;

}

long LinuxParser::IdleJiffies() { 
  std::ifstream statFile;
  string totalProcesses, value;
  statFile.open(kProcDirectory + kStatFilename);
  long idleJiffies = 0;
  
  if(statFile.is_open()) {
    while(!statFile.eof()) {
        getline(statFile, totalProcesses);
        break;
    }

    std::istringstream linestream(totalProcesses);
    linestream >> value;
    for (int i = 1; i < 10; i++) {
        linestream >> value;
        if (   (i == 4)
            || (i == 5)) {

         idleJiffies += std::stol(value, NULL, 0);
        }

      }
    statFile.close();
  }

  return idleJiffies;
}

vector<string> LinuxParser::CpuUtilization() { 

  std::ifstream statFile;
  string totalProcesses, cpuName, value;
  statFile.open(kProcDirectory + kStatFilename);
  vector<string> cpu_ut;
  
  if(statFile.is_open()) {
    while(!statFile.eof()) {
        getline(statFile, totalProcesses);
        break;
    }
    std::istringstream linestream(totalProcesses);
    linestream >> cpuName;
    
    while(linestream >> value)
    {
      cpu_ut.push_back(value);
    }
  }
  
  return cpu_ut;
}

int LinuxParser::TotalProcesses() { 
  std::ifstream statFile;
  string totalProcesses;
  statFile.open(kProcDirectory + kStatFilename);
  
  if(statFile.is_open()) {
    while(!statFile.eof()) {
        getline(statFile, totalProcesses, ' ');
        if(totalProcesses.compare(kProcess) == 0) break;

        getline(statFile, totalProcesses);
    }
    getline(statFile, totalProcesses, ' ');
    statFile.close();
  }
  
  return stoi(totalProcesses);
}

int LinuxParser::RunningProcesses() { 
  std::ifstream statFile;
  string procsRunning="";
  statFile.open(kProcDirectory + kStatFilename);
  
  if(statFile.is_open()) {
    while(!statFile.eof()) {
        getline(statFile, procsRunning, ' ');

        if(procsRunning.compare(kProcessRunning) == 0) break;

        getline(statFile, procsRunning);
    }
  
    getline(statFile, procsRunning, ' ');
    statFile.close();
  }
    
  return stoi(procsRunning);
}

string LinuxParser::Command(int pid) { 
  std::ifstream cmdFile;
  string cmd ="";

  cmdFile.open(kProcDirectory + to_string(pid) + kCmdlineFilename);

  if(cmdFile.is_open()) {
    getline(cmdFile, cmd);

    cmdFile.close();
  }
  
   return cmd;
}

string LinuxParser::Ram(int pid) { 
  std::ifstream statusFile;
  string startTime="", value="", title="";
  float res =0.0;
  int lineCntr = 0;

  statusFile.open(kProcDirectory + to_string(pid) + kStatusFilename);

  if(statusFile.is_open()) {
    while(!statusFile.eof()) {
      lineCntr++;
      getline(statusFile, startTime);

      if(lineCntr == VMSIZE_ROW_INDEX) break;
    }
   
    std::istringstream linestream(startTime);
    linestream >> title >> value;

    res = (stoi(value)/1024);
    
    statusFile.close();
  }
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << res;

  return stream.str();; 
}

string LinuxParser::Uid(int pid) { 
  std::ifstream statusFile;
  string title="", uidData="", uid="";
  int lineCntr = 0;
  
  statusFile.open(kProcDirectory + to_string(pid) + kStatusFilename);

  if(statusFile.is_open()) {
    while(!statusFile.eof()) {
      lineCntr++;
      getline(statusFile, uidData);

      if(lineCntr == UID_ROW_INDEX) break;
    }

    std::istringstream linestream(uidData);
    linestream >> title >> uid;

    statusFile.close();
  }
  
  return uid;
}

string LinuxParser::User(int pid) { 
  std::ifstream passwdFile;
  string pUserName="";
  string cUid="", title="", rawData="", tmp="", tUid="";
  int lineCntr = 0;

  cUid = LinuxParser::Uid(pid);
  
  passwdFile.open(kPasswordPath);

  lineCntr = 0;
  if(passwdFile.is_open()) {
    while(!passwdFile.eof()) {
      lineCntr++;
      
      getline(passwdFile, rawData);
      replace(rawData.begin(), rawData.end(), ':', ' ');
      
      std::istringstream linestream(rawData);
      linestream >> title >> tmp >> tUid;
      
      if(tUid.compare(cUid) == 0) {
        pUserName = title;
        break;
      }  
    }
    passwdFile.close();
  }

  return pUserName;
}

long LinuxParser::UpTime(int pid) { 
  std::ifstream uptimeFile, statFile;
  string systemUptime="",startTime = "",totalProcesses="";
  int lineCntr = 0;

  statFile.open(kProcDirectory + to_string(pid) + kStatFilename);

  if(statFile.is_open()) {
    while(!statFile.eof()) {
      lineCntr++;
      getline(statFile, startTime, ' ');

      if(lineCntr == STARTTIME_ROW_INDEX) break;
    }
    statFile.close();
  }

  uptimeFile.open(kProcDirectory + kUptimeFilename);

  if(uptimeFile.is_open()) {
    getline(uptimeFile, systemUptime,' ');
    uptimeFile.close();
  }

  long  res = (long )(stoi(systemUptime) - (stoi(startTime)/ sysconf(_SC_CLK_TCK)));

  if(res < 0) {
     return (-1)*res;
  }
  else {
    return res;
  }
}
