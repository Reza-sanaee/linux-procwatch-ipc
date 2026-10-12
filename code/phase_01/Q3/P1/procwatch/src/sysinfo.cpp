#include "../include/sysinfo.h"
#include <fstream>
#include <sstream>
#include <iostream>

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
