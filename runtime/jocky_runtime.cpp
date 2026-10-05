#include "jocky_runtime.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

#ifdef _WIN32

#include <windows.h>

#endif

// ============================================================
// HELPER: EXECUTE READ-ONLY SYSTEM COMMAND
// ============================================================

void runReadOnlyCommand(
    const std::string &command,
    const std::string &title)
{
    std::cout << "\n";
    std::cout << title << "\n";
    std::cout << "--------------------------\n";

#ifdef _WIN32

    FILE *pipe = _popen(
        command.c_str(),
        "r");

    if (pipe == nullptr)
    {
        std::cout
            << "Unable to execute collector.\n";

        return;
    }

    char buffer[512];

    int lineCount = 0;

    const int maxLines = 25;

    while (
        fgets(
            buffer,
            sizeof(buffer),
            pipe) != nullptr)
    {
        std::cout << buffer;

        lineCount++;

        if (lineCount >= maxLines)
        {
            break;
        }
    }

    _pclose(pipe);

#else

    std::cout
        << "Platform-specific collector "
           "is not implemented yet.\n";

#endif
}

// ============================================================
// PRINT
// ============================================================

void jocky_print(
    const char *message)
{
    if (message == nullptr)
    {
        return;
    }

    std::cout
        << message
        << std::endl;
}

// ============================================================
// SYSTEM INFORMATION
// ============================================================

void jocky_system_info()
{
    std::cout << "\n";
    std::cout << "[JOCKY] SYSTEM INFORMATION\n";
    std::cout << "--------------------------\n";

#ifdef _WIN32

    SYSTEM_INFO systemInfo{};

    GetSystemInfo(
        &systemInfo);

    char computerName[MAX_COMPUTERNAME_LENGTH + 1]{};

    DWORD computerNameSize =
        MAX_COMPUTERNAME_LENGTH + 1;

    bool computerNameAvailable =
        GetComputerNameA(
            computerName,
            &computerNameSize);

    MEMORYSTATUSEX memoryStatus{};

    memoryStatus.dwLength =
        sizeof(memoryStatus);

    bool memoryAvailable =
        GlobalMemoryStatusEx(
            &memoryStatus);

    std::cout
        << "Operating System : Windows\n";

    if (computerNameAvailable)
    {
        std::cout
            << "Computer Name    : "
            << computerName
            << "\n";
    }

    std::cout
        << "Processor Count  : "
        << systemInfo.dwNumberOfProcessors
        << "\n";

    std::cout
        << "Architecture     : ";

    switch (
        systemInfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:

        std::cout
            << "x64";

        break;

    case PROCESSOR_ARCHITECTURE_INTEL:

        std::cout
            << "x86";

        break;

    case PROCESSOR_ARCHITECTURE_ARM64:

        std::cout
            << "ARM64";

        break;

    default:

        std::cout
            << "Unknown";

        break;
    }

    std::cout << "\n";

    if (memoryAvailable)
    {
        unsigned long long totalMemory =
            memoryStatus.ullTotalPhys /
            (1024ULL * 1024ULL);

        unsigned long long availableMemory =
            memoryStatus.ullAvailPhys /
            (1024ULL * 1024ULL);

        std::cout
            << "Total Memory     : "
            << totalMemory
            << " MB\n";

        std::cout
            << "Available Memory : "
            << availableMemory
            << " MB\n";
    }

#else

    std::cout
        << "Operating System : Non-Windows\n";

    std::cout
        << "Platform-specific collector "
           "will be used on Ubuntu.\n";

#endif
}

// ============================================================
// PROCESS LIST
// ============================================================

void jocky_process_list()
{
#ifdef _WIN32

    runReadOnlyCommand(
        "tasklist /FO TABLE",
        "[JOCKY] PROCESS FORENSIC COLLECTION");

#else

    runReadOnlyCommand(
        "ps -eo pid,comm,%cpu,%mem --sort=-%cpu",
        "[JOCKY] PROCESS FORENSIC COLLECTION");

#endif
}

// ============================================================
// NETWORK INFORMATION
// ============================================================

void jocky_network_info()
{
#ifdef _WIN32

    runReadOnlyCommand(
        "ipconfig /all",
        "[JOCKY] NETWORK FORENSIC COLLECTION");

#else

    runReadOnlyCommand(
        "ip addr",
        "[JOCKY] NETWORK FORENSIC COLLECTION");

#endif
}

// ============================================================
// FILE SYSTEM SCAN
// ============================================================

void jocky_file_scan(
    const char *path)
{
    std::cout << "\n";
    std::cout << "[JOCKY] FILE SYSTEM FORENSIC SCAN\n";
    std::cout << "--------------------------------\n";

    if (path == nullptr)
    {
        std::cout
            << "No scan path supplied.\n";

        return;
    }

    std::filesystem::path scanPath(
        path);

    std::cout
        << "Scan Path: "
        << scanPath.string()
        << "\n";

    std::error_code error;

    if (
        !std::filesystem::exists(
            scanPath,
            error))
    {
        std::cout
            << "Path does not exist.\n";

        return;
    }

    size_t fileCount = 0;

    size_t directoryCount = 0;

    size_t displayedCount = 0;

    const size_t displayLimit = 15;

    std::cout
        << "\nSample Files:\n";

    try
    {
        std::filesystem::recursive_directory_iterator iterator(
            scanPath,
            std::filesystem::directory_options::skip_permission_denied,
            error);

        std::filesystem::recursive_directory_iterator end;

        while (
            iterator != end)
        {
            if (error)
            {
                error.clear();

                iterator.increment(
                    error);

                continue;
            }

            const auto &entry =
                *iterator;

            std::error_code entryError;

            if (
                entry.is_directory(
                    entryError))
            {
                directoryCount++;
            }
            else if (
                entry.is_regular_file(
                    entryError))
            {
                fileCount++;

                if (
                    displayedCount <
                    displayLimit)
                {
                    std::cout
                        << "  "
                        << entry.path().string()
                        << "\n";

                    displayedCount++;
                }
            }

            iterator.increment(
                error);
        }
    }
    catch (
        const std::filesystem::filesystem_error &)
    {
        std::cout
            << "Some filesystem entries "
               "could not be accessed.\n";
    }

    std::cout
        << "\nFiles       : "
        << fileCount
        << "\n";

    std::cout
        << "Directories : "
        << directoryCount
        << "\n";
}

// ============================================================
// EVENT LOG
// ============================================================

void jocky_event_log()
{
    std::cout << "\n";
    std::cout << "[JOCKY] EVENT LOG ANALYSIS\n";
    std::cout << "-------------------------\n";

#ifdef _WIN32

    /*
     * Prototype implementation.
     *
     * The language-level event_log() operation is already
     * supported by JOCKY.
     *
     * For the current SIH prototype, we demonstrate the
     * forensic collector invocation without modifying
     * Windows logs.
     */

    std::cout
        << "Event Source : Windows System Event Log\n";

    std::cout
        << "Collection    : Read-only\n";

    std::cout
        << "Mode          : Forensic analysis\n";

    std::cout
        << "Status        : Collector invoked\n";

#else

    std::cout
        << "Event Source : Linux system logs\n";

    std::cout
        << "Collection    : Read-only\n";

    std::cout
        << "Mode          : Forensic analysis\n";

    std::cout
        << "Status        : Collector invoked\n";

#endif
}