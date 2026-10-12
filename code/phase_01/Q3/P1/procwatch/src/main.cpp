#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstring>
#include "../include/sysinfo.h"

void clearScreen() {

    std::cout << "\033[2J\033[1;1H";
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::strcmp(argv[1], "interactive") == 0) {
        CpuData prevCpu = readCpuStats();
        
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            CpuData currCpu = readCpuStats();
            double cpuUsage = calculateCpuUsage(prevCpu, currCpu);
            prevCpu = currCpu; // Store current state for the next calculation

            SystemMemory mem = getMemoryInfo();
            std::string kernel = getKernelVersion();
            int cores = getCpuCores();
            std::string loadAvg = getLoadAverage();

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
            std::cout << "======================================================\n";
            std::cout << "Press Ctrl+C to exit.\n";
        }
    } else {
        std::cout << "Usage: " << argv[0] << " interactive\n";
        std::cout << "(Background mode is not yet implemented in this view)\n";
    }
    return 0;
}
