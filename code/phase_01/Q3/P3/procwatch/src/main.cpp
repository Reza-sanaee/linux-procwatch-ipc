#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstring>
#include <map>
#include <vector>
#include <fstream>
#include <csignal>
#include <atomic>
#include "../include/sysinfo.h"


std::atomic<bool> keep_running(true);


void signalHandler(int signum) {
    if (signum == SIGTERM) {
        std::cout << "[Procwatch] SIGTERM received. Graceful shutdown initiated..." << std::endl;
        keep_running = false;
    }
}


struct Config {
    int interval = 5;
    int n = 5;
    bool simulate_error = false;
};

Config parseConfig(const std::string& path) {
    Config cfg;
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("interval=") == 0) cfg.interval = std::stoi(line.substr(9));
        else if (line.find("n=") == 0) cfg.n = std::stoi(line.substr(2));
        else if (line.find("simulate_error=") == 0) cfg.simulate_error = (line.substr(15) == "true");
    }
    return cfg;
}

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

int main(int argc, char* argv[]) {
   
    signal(SIGTERM, signalHandler);

    if (argc > 1 && std::strcmp(argv[1], "interactive") == 0) {
        CpuData prevCpu = readCpuStats();
        std::map<int, unsigned long long> prevProcessStats;
        getTopProcesses(5, prevProcessStats);

        while (keep_running) {
          
            for(int i=0; i<20 && keep_running; ++i) std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!keep_running) break;
            
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
            std::cout << std::left << std::setw(10) << "PID" << std::setw(20) << "NAME" 
                      << std::setw(15) << "CPU ACTIVITY" << std::setw(15) << "RSS (KB)" << "\n";
            
            for (const auto& p : topProcesses) {
                std::cout << std::left << std::setw(10) << p.pid << std::setw(20) << p.name 
                          << std::setw(15) << p.cpu_diff << std::setw(15) << p.rss_kb << "\n";
            }
            std::cout << "======================================================\n";
            std::cout << "Press Ctrl+C to exit.\n";
        }
        return 0;

    } else if (argc > 2 && std::strcmp(argv[1], "background") == 0) {
        std::string configPath = argv[2];
        Config cfg = parseConfig(configPath);
        
        std::cout << "[Procwatch] Service Started. Config -> Interval: " 
                  << cfg.interval << "s, N: " << cfg.n << std::endl;

        std::map<int, unsigned long long> prevProcessStats;
        getTopProcesses(cfg.n, prevProcessStats);

        int loop_count = 0;

        while (keep_running) {
            for(int i = 0; i < cfg.interval * 10 && keep_running; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (!keep_running) break;

            std::vector<ProcessInfo> topProcesses = getTopProcesses(cfg.n, prevProcessStats);
            
            std::cout << "[Procwatch] Top " << cfg.n << " CPU Consumers: ";
            for (size_t i = 0; i < topProcesses.size(); ++i) {
                std::cout << topProcesses[i].name << "(PID:" << topProcesses[i].pid << ") ";
            }
            std::cout << std::endl;


            loop_count++;
            if (cfg.simulate_error && loop_count >= 2) {
                std::cerr << "[Procwatch] CRITICAL: Simulated intentional error triggered! Exiting with code 1." << std::endl;
                exit(1); 
            }
        }
        std::cout << "[Procwatch] Exiting cleanly with code 0." << std::endl;
        return 0; 

    } else {
        std::cout << "Usage:\n";
        std::cout << "  " << argv[0] << " interactive\n";
        std::cout << "  " << argv[0] << " background <config_file_path>\n";
    }
    return 0;
}
