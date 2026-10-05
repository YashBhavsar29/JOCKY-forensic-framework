#pragma once

#ifdef _WIN32

void jocky_print(
    const char* message
);

void jocky_system_info();

void jocky_process_list();

void jocky_network_info();

void jocky_file_scan(
    const char* path
);

void jocky_event_log();

#else

void jocky_print(
    const char* message
);

void jocky_system_info();

void jocky_process_list();

void jocky_network_info();

void jocky_file_scan(
    const char* path
);

void jocky_event_log();

#endif