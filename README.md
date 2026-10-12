# Linux Process Telemetry and Unix-Domain IPC Services

C++ Linux utilities implement a Unix-domain TIME/ECHO service and a process-monitoring daemon using procfs, configuration files, signals, CMake, and systemd. The project demonstrates user-space operating-system interfaces and service lifecycle management.

## Technical highlights

- Implemented C++ Linux system/process telemetry using /proc counters, resident-memory accounting, and configurable sampling.
- Built Unix-domain TIME/ECHO IPC and a systemd-managed monitoring daemon with signal handling and restart tests.

## Explore the project

- [Source and execution guide](code/RUNNING.md)
- [Technical report (PDF)](report/Report_EN.pdf)
- [Technical report (Markdown)](report/Report_EN.md)
- [Assignment/specification (PDF)](assignment/Assignment_EN.pdf)

## Reproduction and attribution

Read the execution guide and report before running the code. Dependencies, input data, licensed tools, and hardware requirements vary by project. Recorded results are distinguished from rerun results in the accompanying documentation. Team and third-party source attribution in the reports and source headers is retained. This portfolio does not claim independent authorship of supplied frameworks or dependencies.
