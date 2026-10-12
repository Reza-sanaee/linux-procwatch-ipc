#ifndef SYSINFO_H
#define SYSINFO_H

#include <string>
#include <vector>
#include <map>

struct SystemMemory {
    long total;
    long free;
    long available;
    long used;
};

struct CpuData {
    unsigned long long idle;
    unsigned long long total;
};

struct ProcessInfo {
    int pid;
    std::string name;
    unsigned long long cpu_diff;
    long rss_kb;
};

std::string getKernelVersion();
int getCpuCores();
SystemMemory getMemoryInfo();
std::string getLoadAverage();
CpuData readCpuStats();
double calculateCpuUsage(const CpuData& prev, const CpuData& curr);


std::vector<ProcessInfo> getTopProcesses(int n, std::map<int, unsigned long long>& prev_process_stats);

#endif
