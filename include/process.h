#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization() const;            // TODO: See src/process.cpp
  std::string Ram() const;                 // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  Process(std::string t_path);

  // TODO: Declare any necessary private members
 private:
  int p_pid = 0;
  std::string p_user_name = "";
  std::string p_cmd = "";
  float p_cpu_u = 0.0;
  std::string p_ram;
  long int p_uptime = 0;
  bool p_op = false;
  std::string path;
};

#endif