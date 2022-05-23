#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;
using std::istringstream;

//return Uptime(starttime)
int LinuxParser::clkTPS(){
	return sysconf(_SC_CLK_TCK);
}

vector<string> StringToVector(string filename){
	vector<string> result;
  	string line, value;
  	ifstream f_stream(filename);
  	if(f_stream){
    	getline(f_stream, line);
      	istringstream linestream(line);
      	while(linestream >> value)
          result.push_back(value);
    }
  	f_stream.close();
  	return result;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
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
  string os, version, kernel;
  string line;
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
  string line, key, value;
  string memTotal, memFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::stringstream linestream(line);
      while(linestream >> key >> value){
        if (key == "MemTotal:"){
          memTotal = value;
        }
        if(key == "MemFree:"){
          memFree = value;
        }
      }
    }
  }
  return (stof(memTotal)-stof(memFree)) / stof(memTotal);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, str_value;
  	long value = 0;
  	
  	ifstream filestream(kProcDirectory + kUptimeFilename);
  	
  	if(filestream.is_open()){
    	getline(filestream, line);
      	istringstream linestream(line);
      	linestream >> str_value;
      	value = stol(str_value);
    }
  	filestream.close();
  	return value;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return (ActiveJiffies() - IdleJiffies()); 

 }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string filename = kProcDirectory + to_string(pid) + kStatFilename;
  	vector<string> vstr = StringToVector(filename);
  	if(vstr.size() >= 21)
      return (stol(vstr[13]) + stol(vstr[14]) + stol(vstr[15]) + stol(vstr[16]));
  	return 0;
 }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  const auto cpu_stats = LinuxParser::CpuUtilization();
  	const auto user = stol(cpu_stats[LinuxParser::kUser_]);
  	const auto nice = stol(cpu_stats[LinuxParser::kNice_]);
  	const auto system = stol(cpu_stats[LinuxParser::kSystem_]);
  	const auto idle = stol(cpu_stats[LinuxParser::kIdle_]);
  	const auto iowait = stol(cpu_stats[LinuxParser::kIOwait_]);
  	const auto irq = stol(cpu_stats[LinuxParser::kIRQ_]);
  	const auto softirq = stol(cpu_stats[LinuxParser::kSoftIRQ_]);
  	const auto steal = stol(cpu_stats[LinuxParser::kSteal_]);

  return (user + nice + system + idle + iowait + irq + softirq + steal);
 }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  const auto cpu_stats = LinuxParser::CpuUtilization();
  	const auto idle = stol(cpu_stats[LinuxParser::kIdle_]);
  	const auto iowait = stol(cpu_stats[LinuxParser::kIOwait_]);
  
  return (idle + iowait);
 }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> result;
  	string line, val;
  	
  	ifstream stream (kProcDirectory + kStatFilename);
  	if(stream){
    	getline(stream, line);
      	istringstream linestream(line);
      	linestream >> val;
      	
      	for(int ii = 0; ii < CPUStates::kGuestNice_; ii++){
        	linestream >> val;
          	result.push_back(val);
        }
    }
  	stream.close();
  	return result; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key;
  int value;
  std::ifstream f_stream(kProcDirectory + kStatFilename);

  if(f_stream){
    while(getline(f_stream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "processes")\
          return value;
      }
    }
  } 
  f_stream.close(); 
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key;
  int value;
  std::ifstream f_stream(kProcDirectory + kStatFilename);
  
  if(f_stream){
    while(getline(f_stream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "procs_running")
          return value;
      }
    }
  }
  f_stream.close();
  return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;

  	ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

  	if (filestream) 
    	getline(filestream, line);

  	filestream.close();
  return line;
 }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  const auto mbMem = (ReadProcPID(pid, "VmData"/*"VmRSS"*/) / 1024 /*KB2MB*/);
  return to_string(mbMem);
 }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  const auto uid = ReadProcPID(pid, "Uid");
  return to_string(uid);
 }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string key = LinuxParser::Uid(pid);
  	string line, uid_str, user;

    ifstream f_stream(kPasswordPath);

  if (f_stream) {
    while (getline(f_stream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      istringstream linestream(line);

      while (linestream >> user >> uid_str) {
        if (uid_str == key) return user;
      }
    }
  }

  f_stream.close();
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  vector<string> result = StringToVector(kProcDirectory + to_string(pid) + kStatFilename);

  if (result.size() >= 21) {
    const auto starttime = stol(result[21]);
    return UpTime() - (starttime / clkTPS());
  }

  return 0;
 }

long ReadProcInfo(const string filename, const string &search_key) {
  string line, key, str_value;
  ifstream f_stream(filename);

  if (f_stream) {
    while (getline(f_stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      istringstream linestream(line);

      while (linestream >> key >> str_value) {
        if (key == search_key) return stol(str_value);
      }
    }
  }
  f_stream.close();
  return 0;
}

long LinuxParser::ReadProcPID(const int &pid, const string &search_key) {
  string filename = kProcDirectory + std::to_string(pid) + kStatusFilename;
  return ReadProcInfo(filename, search_key);
}