#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

#define MAX_TASKS 100
#define MAX_MEMORY_BLOCKS 100
#define MAX_MEMORY_SIZE 1024
#define BLOCK_SIZE 32
#define MAX_FILES 100
#define MAX_FILENAME_LEN 50

// 处理机管理模块
typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
} PCB;

vector<PCB> task_queue;
int task_count = 0;

bool sortByArrivalTime(const PCB &a, const PCB &b) {
    return a.arrival_time < b.arrival_time;
}

bool sortByBurstTime(const PCB &a, const PCB &b) {
    return a.burst_time < b.burst_time;
}

void execute_task(const PCB &task) {
    cout << "Executing task " << task.pid << " with burst time " << task.burst_time << endl;
}

bool tasks_remaining(vector<PCB> &tasks) {
    for (auto &task: tasks) {
        if (task.remaining_time > 0) {
            return true;
        }
    }
    return false;
}

void FCFS() {
    sort(task_queue.begin(), task_queue.end(), sortByArrivalTime);
    for (auto &task: task_queue) {
        execute_task(task);
    }
}

void SJF() {
    sort(task_queue.begin(), task_queue.end(), sortByBurstTime);
    for (auto &task: task_queue) {
        execute_task(task);
    }
}

void RR(int quantum) {
    int time = 0;
    while (tasks_remaining(task_queue)) {
        for (auto &task: task_queue) {
            if (task.remaining_time > 0) {
                if (task.remaining_time > quantum) {
                    cout << "Executing task " << task.pid << " for " << quantum << " time units" << endl;
                    task.remaining_time -= quantum;
                    time += quantum;
                } else {
                    cout << "Executing task " << task.pid << " for " << task.remaining_time << " time units" << endl;
                    time += task.remaining_time;
                    task.remaining_time = 0;
                }
            }
        }
    }
}

// 内存管理模块
typedef struct {
    int start_address;
    int size;
    bool is_free;
} MemoryBlock;

vector<MemoryBlock> memory;
int memory_size = MAX_MEMORY_SIZE;

void initialize_memory() {
    MemoryBlock initial_block = {0, MAX_MEMORY_SIZE, true};
    memory.push_back(initial_block);
}

void allocate_memory(int index, int size) {
    MemoryBlock &block = memory[index];
    if (block.size == size) {
        block.is_free = false;
    } else {
        MemoryBlock new_block = {block.start_address + size, block.size - size, true};
        block.size = size;
        block.is_free = false;
        memory.insert(memory.begin() + index + 1, new_block);
    }
    cout << "Allocated " << size << " units of memory at address " << block.start_address << endl;
}

void FirstFit(int size) {
    for (int i = 0; i < memory.size(); i++) {
        if (memory[i].is_free && memory[i].size >= size) {
            allocate_memory(i, size);
            return;
        }
    }
    cout << "No sufficient memory available!\n";
}

void BestFit(int size) {
    int best_index = -1;
    int min_size = MAX_MEMORY_SIZE + 1;
    for (int i = 0; i < memory.size(); i++) {
        if (memory[i].is_free && memory[i].size >= size && memory[i].size < min_size) {
            best_index = i;
            min_size = memory[i].size;
        }
    }
    if (best_index != -1) {
        allocate_memory(best_index, size);
    } else {
        cout << "No sufficient memory available!\n";
    }
}

// 文件管理模块
typedef struct {
    char filename[MAX_FILENAME_LEN];
    int size;
    int start_block;
    int block_count;
} FCB;

vector<FCB> file_directory;
int file_count = 0;
vector<bool> disk_blocks(MAX_MEMORY_SIZE / BLOCK_SIZE, true);

int allocate_blocks(int size) {
    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int start_block = -1;
    int count = 0;
    for (int i = 0; i < disk_blocks.size(); i++) {
        if (disk_blocks[i]) {
            if (start_block == -1) start_block = i;
            count++;
            if (count == blocks_needed) {
                for (int j = start_block; j < start_block + blocks_needed; j++) {
                    disk_blocks[j] = false;
                }
                return start_block;
            }
        } else {
            start_block = -1;
            count = 0;
        }
    }
    return -1; // Not enough blocks
}

void create_file(const char *filename, int size) {
    if (file_count < MAX_FILES) {
        int start_block = allocate_blocks(size);
        if (start_block == -1) {
            cout << "No sufficient disk space available!\n";
            return;
        }
        FCB file;
        strncpy(file.filename, filename, MAX_FILENAME_LEN);
        file.size = size;
        file.start_block = start_block;
        file.block_count = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        file_directory.push_back(file);
        file_count++;
        cout << "Created file " << filename << " of size " << size << " at block " << start_block << endl;
    } else {
        cout << "Maximum file limit reached!\n";
    }
}

int find_file(const char *filename) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_directory[i].filename, filename) == 0) {
            return i;
        }
    }
    return -1; // File not found
}

void read_blocks(int start_block, int block_count) {
    cout << "Reading " << block_count << " blocks starting from block " << start_block << endl;
}

void read_file(const char *filename) {
    int index = find_file(filename);
    if (index != -1) {
        read_blocks(file_directory[index].start_block, file_directory[index].block_count);
    } else {
        cout << "File not found!\n";
    }
}

int main() {
    // 初始化任务队列
    task_queue = {
            {1, 0, 5, 1, 5},
            {2, 1, 3, 1, 3},
            {3, 2, 8, 1, 8},
            {4, 3, 6, 1, 6}
    };
    task_count = task_queue.size();

    cout << "FCFS Scheduling:\n";
    FCFS();

    cout << "\nSJF Scheduling:\n";
    SJF();

    cout << "\nRR Scheduling (Quantum = 2):\n";
    RR(2);

    // 初始化内存
    initialize_memory();
    cout << "\nMemory Allocation (First Fit):\n";
    FirstFit(100);
    FirstFit(200);
    FirstFit(300);

    cout << "\nMemory Allocation (Best Fit):\n";
    BestFit(150);
    BestFit(50);
    BestFit(250);

    // 文件管理
    cout << "\nFile Management:\n";
    create_file("file1.txt", 100);
    create_file("file2.txt", 200);
    read_file("file1.txt");
    read_file("file2.txt");

    return 0;
}
