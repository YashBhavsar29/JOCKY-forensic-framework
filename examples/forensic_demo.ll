; ModuleID = 'forensic_demo'
source_filename = "forensic_demo"

@jocky_system_title = private unnamed_addr constant [27 x i8] c"[JOCKY] SYSTEM INFORMATION\00", align 1
@jocky_system_separator = private unnamed_addr constant [27 x i8] c"--------------------------\00", align 1
@jocky_system_command = private unnamed_addr constant [11 x i8] c"systeminfo\00", align 1
@jocky_process_title = private unnamed_addr constant [36 x i8] c"[JOCKY] PROCESS FORENSIC COLLECTION\00", align 1
@jocky_process_separator = private unnamed_addr constant [27 x i8] c"--------------------------\00", align 1
@jocky_process_command = private unnamed_addr constant [19 x i8] c"tasklist /FO TABLE\00", align 1
@jocky_network_title = private unnamed_addr constant [36 x i8] c"[JOCKY] NETWORK FORENSIC COLLECTION\00", align 1
@jocky_network_separator = private unnamed_addr constant [27 x i8] c"--------------------------\00", align 1
@jocky_network_command = private unnamed_addr constant [14 x i8] c"ipconfig /all\00", align 1
@jocky_file_title = private unnamed_addr constant [34 x i8] c"[JOCKY] FILE SYSTEM FORENSIC SCAN\00", align 1
@jocky_file_separator = private unnamed_addr constant [33 x i8] c"--------------------------------\00", align 1
@jocky_file_path_label = private unnamed_addr constant [11 x i8] c"Scan Path:\00", align 1
@jocky_file_command = private unnamed_addr constant [126 x i8] c"powershell.exe -NoProfile -Command \22Get-ChildItem -Force -Recurse -File . | Select-Object -First 15 -ExpandProperty FullName\22\00", align 1
@jocky_event_title = private unnamed_addr constant [27 x i8] c"[JOCKY] EVENT LOG ANALYSIS\00", align 1
@jocky_event_separator = private unnamed_addr constant [26 x i8] c"-------------------------\00", align 1
@jocky_event_mode = private unnamed_addr constant [48 x i8] c"Collection : Read-only Windows System Event Log\00", align 1
@jocky_event_command = private unnamed_addr constant [42 x i8] c"wevtutil qe System /c:20 /rd:true /f:text\00", align 1
@jocky_user_string = private unnamed_addr constant [32 x i8] c"=== JOCKY DIGITAL FORENSICS ===\00", align 1
@jocky_scan_path = private unnamed_addr constant [2 x i8] c".\00", align 1
@jocky_user_string.1 = private unnamed_addr constant [37 x i8] c"=== FORENSIC COLLECTION COMPLETE ===\00", align 1

declare i32 @puts(ptr)

declare i32 @system(ptr)

define void @jocky_print(ptr %message) {
entry:
  %0 = call i32 @puts(ptr %message)
  ret void
}

define void @jocky_system_info() {
entry:
  %0 = call i32 @puts(ptr @jocky_system_title)
  %1 = call i32 @puts(ptr @jocky_system_separator)
  %2 = call i32 @system(ptr @jocky_system_command)
  ret void
}

define void @jocky_process_list() {
entry:
  %0 = call i32 @puts(ptr @jocky_process_title)
  %1 = call i32 @puts(ptr @jocky_process_separator)
  %2 = call i32 @system(ptr @jocky_process_command)
  ret void
}

define void @jocky_network_info() {
entry:
  %0 = call i32 @puts(ptr @jocky_network_title)
  %1 = call i32 @puts(ptr @jocky_network_separator)
  %2 = call i32 @system(ptr @jocky_network_command)
  ret void
}

define void @jocky_file_scan(ptr %path) {
entry:
  %0 = call i32 @puts(ptr @jocky_file_title)
  %1 = call i32 @puts(ptr @jocky_file_separator)
  %2 = call i32 @puts(ptr @jocky_file_path_label)
  call void @jocky_print(ptr %path)
  %3 = call i32 @system(ptr @jocky_file_command)
  ret void
}

define void @jocky_event_log() {
entry:
  %0 = call i32 @puts(ptr @jocky_event_title)
  %1 = call i32 @puts(ptr @jocky_event_separator)
  %2 = call i32 @puts(ptr @jocky_event_mode)
  %3 = call i32 @system(ptr @jocky_event_command)
  ret void
}

define i32 @main() {
entry:
  call void @jocky_print(ptr @jocky_user_string)
  call void @jocky_system_info()
  call void @jocky_process_list()
  call void @jocky_network_info()
  call void @jocky_file_scan(ptr @jocky_scan_path)
  call void @jocky_event_log()
  call void @jocky_print(ptr @jocky_user_string.1)
  ret i32 0
}
