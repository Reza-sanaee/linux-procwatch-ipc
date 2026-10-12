#include "../include/sysinfo.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <cctype>

std::string getKernelVersion() {
    std::ifstream file("/proc/version");
    std::string line, version = "Unknown";
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string os, word;
        iss >> os >> word >> version;
    }
    return version;
}

int getCpuCores() {
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    int cores = 0;
    while (std::getline(file, line)) {
        if (line.find("processor") == 0) cores++;
    }
    return cores;
}

SystemMemory getMemoryInfo() {
    std::ifstream file("/proc/meminfo");
    std::string line, key;
    long value;
    std::string unit;
    SystemMemory mem = {0, 0, 0, 0};

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> key >> value >> unit;
        if (key == "MemTotal:") mem.total = value;
        else if (key == "MemFree:") mem.free = value;
        else if (key == "MemAvailable:") mem.available = value;
    }
    mem.used = mem.total - mem.available; 
    return mem;
}

std::string getLoadAverage() {
    std::ifstream file("/proc/loadavg");
    std::string line, load1, load5, load15;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> load1 >> load5 >> load15;
    }
    return load1 + ", " + load5 + ", " + load15;
}

CpuData readCpuStats() {
    std::ifstream file("/proc/stat");
    std::string line, cpu_label;
    CpuData data = {0, 0};

    if (std::getline(file, line)) {
        std::istringstream iss(line);
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
        iss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
        
        data.idle = idle + iowait;
        unsigned long long non_idle = user + nice + system + irq + softirq + steal;
        data.total = data.idle + non_idle;
    }
    return data;
}

double calculateCpuUsage(const CpuData& prev, const CpuData& curr) {
    unsigned long long totald = curr.total - prev.total;
    unsigned long long idled = curr.idle - prev.idle;
    if (totald == 0) return 0.0;
    return (double)(totald - idled) / totald * 100.0;
}


bool isNumeric(const std::string& str) {
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

std::vector<ProcessInfo> getTopProcesses(int n, std::map<int, unsigned long long>& prev_process_stats) {
    std::vector<ProcessInfo> processes;
    std::map<int, unsigned long long> current_stats;
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024; 

    DIR* dir = opendir("/proc");
    if (dir == nullptr) return processes;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR && isNumeric(entry->d_name)) {
            int pid = std::stoi(entry->d_name);
            std::string pid_str = entry->d_name;

       
            std::ifstream stat_file("/proc/" + pid_str + "/stat");
            if (!stat_file.is_open()) continue; 

            std::string line;
            if (!std::getline(stat_file, line)) continue;

        
            std::string name = "Unknown";
            size_t start_pos = line.find('(');
            size_t end_pos = line.rfind(')');
            unsigned long long utime = 0, stime = 0;

            if (start_pos != std::string::npos && end_pos != std::string::npos) {
                name = line.substr(start_pos + 1, end_pos - start_pos - 1);
                std::string rest = line.substr(end_pos + 2);
                std::istringstream rest_iss(rest);
                
                char state;
                int ppid, pgrp, session, tty_nr, tpgid;
                unsigned int flags;
                unsigned long minflt, cminflt, majflt, cmajflt; 
                rest_iss >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags 
                         >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime;
            } else {
                continue;
            }

            unsigned long long total_time = utime + stime;
            current_stats[pid] = total_time;

            unsigned long long cpu_diff = 0;
            if (prev_process_stats.find(pid) != prev_process_stats.end()) {
                if (total_time > prev_process_stats[pid]) {
                    cpu_diff = total_time - prev_process_stats[pid];
                }
            }

          
            std::ifstream statm_file("/proc/" + pid_str + "/statm");
            long rss = 0;
            if (statm_file.is_open() && std::getline(statm_file, line)) {
                std::istringstream iss_statm(line);
                long size;
                iss_statm >> size >> rss; 
            }
            long rss_kb = rss * page_size_kb;

            processes.push_back({pid, name, cpu_diff, rss_kb});
        }
    }
    closedir(dir);


    prev_process_stats = current_stats;


    std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.cpu_diff > b.cpu_diff;
    });

    if (processes.size() > (size_t)n) {
        processes.resize(n);
    }

    return processes;
}
