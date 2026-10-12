#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstring>
#include <map>
#include <vector>
#include "../include/sysinfo.h"

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::strcmp(argv[1], "interactive") == 0) {
        CpuData prevCpu = readCpuStats();
        std::map<int, unsigned long long> prevProcessStats;
        
 
        getTopProcesses(5, prevProcessStats);

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            CpuData currCpu = readCpuStats();
            double cpuUsage = calculateCpuUsage(prevCpu, currCpu);
            prevCpu = currCpu;

            SystemMemory mem = getMemoryInfo();
            std::string kernel = getKernelVersion();
            int cores = getCpuCores();
            std::string loadAvg = getLoadAverage();

        
            std::vector<ProcessInfo> topProcesses = getTopProcesses(5, prevProcessStats);

            clearScreen();
            std::cout << "================ LOCAL SYSTEM MONITOR ================\n";
            std::cout << std::left << std::setw(25) << "Kernel Version:" << kernel << "\n";
            std::cout << std::left << std::setw(25) << "CPU Cores:" << cores << "\n";
            std::cout << std::left << std::setw(25) << "Load Average:" << loadAvg << "\n";
            std::cout << std::left << std::setw(25) << "Overall CPU Usage:" << std::fixed << std::setprecision(2) << cpuUsage << " %\n";
            std::cout << "------------------------------------------------------\n";
            std::cout << "MEMORY STATUS (KB):\n";
            std::cout << std::left << std::setw(15) << "Total" << std::setw(15) << "Used" 
                      << std::setw(15) << "Free" << std::setw(15) << "Available" << "\n";
            std::cout << std::left << std::setw(15) << mem.total << std::setw(15) << mem.used 
                      << std::setw(15) << mem.free << std::setw(15) << mem.available << "\n";
            std::cout << "------------------------------------------------------\n";
            std::cout << "TOP 5 PROCESSES (By CPU Activity):\n";
            std::cout << std::left << std::setw(10) << "PID" 
                      << std::setw(20) << "NAME" 
                      << std::setw(15) << "CPU ACTIVITY" 
                      << std::setw(15) << "RSS (KB)" << "\n";
            
            for (const auto& p : topProcesses) {
                std::cout << std::left << std::setw(10) << p.pid 
                          << std::setw(20) << p.name 
                          << std::setw(15) << p.cpu_diff 
                          << std::setw(15) << p.rss_kb << "\n";
            }
            std::cout << "======================================================\n";
            std::cout << "Press Ctrl+C to exit.\n";
        }
    } else {
        std::cout << "Usage: " << argv[0] << " interactive\n";
        std::cout << "(Background mode will be implemented using systemd)\n";
    }
    return 0;
}
