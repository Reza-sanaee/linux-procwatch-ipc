#ifndef SYSINFO_H
#define SYSINFO_H

#include <string>

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

std::string getKernelVersion();
int getCpuCores();
SystemMemory getMemoryInfo();
std::string getLoadAverage();
CpuData readCpuStats();
double calculateCpuUsage(const CpuData& prev, const CpuData& curr);

#endif
