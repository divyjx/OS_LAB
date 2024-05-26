#include<cassert>
#include<fstream>
#include<iostream>
#include<queue>
#include<sstream>
#include<string>
#include<vector>
#include<utility>


#define quantum 3


enum ProcStatus {
    OUT,       // not arrived yet.
    WAITING,   // waiting in ready queue.
    RUNNING,   // running on cpu
    IO_WAIT,   // io
    COMPLETED  // process execution is done.
};

struct Process {
    int proc_no;  // process number.
    int arrival_time;
    int todo;
    int done = 0;
    std::vector<int> burst_times; // alternating CPU, IO, CPU..
    ProcStatus status;

    void print_() {     /* USED FOR DEBUGGING  */
        std::cout << "proc_no: " << proc_no << std::endl;
        std::cout << "arrival_time: " << arrival_time << std::endl;
        std::cout << "todo: " << todo << std::endl;
        std::cout << "done: " << done << std::endl;
        std::cout << "status: " << status << std::endl;
        std::cout << "burst_times: ";
        for(const auto& t : burst_times) {
            std::cout << t << " ";
        }
        std::cout << std::endl;
    }
};

struct Metrics {
    int last_arrival_time = -1;  // to ready queue
    int waiting_time = 0;
    int turnaround_time = 0;
    double burst = 0;
    double penalty_ratio = 0;
};

class RoundRobinSimulation {
private:
    int clock_time;
    int total_proc;
    int completed_proc;
    Process* cpu_proc;
    Process* io_proc;
    std::queue<Process*> ready_queue;
    std::queue<Process*> io_waiting_queue;

    std::vector<Metrics> metrics;

    void handle_new_proc() {

        // proc_queue.front()->print_();
        /* add newly arrived process to the ready queue */
        while(!proc_queue.empty() && proc_queue.front()->arrival_time == clock_time) {

            ready_queue.push(proc_queue.front());
            proc_queue.pop();

            std::cout << "Added process: " << ready_queue.back()->proc_no << " to the ready queue" <<  std::endl;   /* COMMENT THIS LINE */
            
            metrics[ready_queue.back()->proc_no].last_arrival_time = clock_time;

            ready_queue.back()->status = WAITING;
        }
    }

    void do_preempt() {
        /* move process to the back of ready queue */
        metrics[cpu_proc->proc_no].last_arrival_time = clock_time;

        std::cout << "preempted process " << cpu_proc->proc_no << std::endl;
        ready_queue.push(cpu_proc);
        cpu_proc->status = WAITING;
        cpu_proc = NULL;
    }

    void show_curr_exec() {
        std::cout << "CPU process: " << (cpu_proc ? cpu_proc->proc_no : -1) << std::endl;
        std::cout << "IO process: " << (io_proc ? io_proc->proc_no : -1) << std::endl;
    }

public:
    RoundRobinSimulation() : clock_time(0), completed_proc(0), cpu_proc(NULL), io_proc(NULL) {}
    
    std::queue<Process*> proc_queue;   /* MAKE THIS PRIVATE LATER */

    void show_metrics() {
        
        // std::cerr << "pid,TAtime,Wtime,penalty" << std::endl; 

        double avg_wait_time = 0;
        double avg_turnaround_time = 0;
        double avg_pen_ratio = 0;
        for(int i = 0; i < total_proc; i++) {
            // std::cerr << i << ",";
            // std::cerr << metrics[i].turnaround_time << ",";
            // std::cerr << metrics[i].waiting_time << ",";
            double pen_ratio = metrics[i].turnaround_time / metrics[i].burst;
            // std::cerr << pen_ratio << std::endl;
            avg_wait_time += metrics[i].waiting_time;
            avg_turnaround_time += metrics[i].turnaround_time;
            avg_pen_ratio += pen_ratio;
        }

        avg_wait_time /= total_proc;
        avg_turnaround_time /= total_proc;
        avg_pen_ratio /= total_proc; 

        std::cout << std::endl;
        std::cout << "AVERAGE WAITING TIME IS: " << avg_wait_time << std::endl;
        std::cout << "AVERAGE TURNAROUND TIME IS: " << avg_turnaround_time << std::endl;
        std::cout << "AVERAGE PENALTY RATIO IS: " << avg_pen_ratio << std::endl;
        std::cout << "SYS THROUGHPUT IS: " << (double)total_proc / (clock_time - 1) << std::endl;

    }

    void start() {

        total_proc = proc_queue.size();
        metrics.resize(total_proc);

        while(completed_proc < total_proc) {
            
            std::cout << "CLOCK: " << clock_time << std::endl;  /* The current value of clock time is completed e.g 0s has lapsed  */
            
            /* what has the cpu / io done till this time */
            /* following is the code for that */

            handle_new_proc();

            if(cpu_proc != NULL) {
                /* cpu task */
                cpu_proc->done++;
                metrics[cpu_proc->proc_no].burst++;
                assert(cpu_proc->status == RUNNING);
            }

            if(io_proc != NULL) {
                /* io task */
                io_proc->done++;
                metrics[io_proc->proc_no].burst++;
                assert(io_proc->status == IO_WAIT);
            }

            show_curr_exec();

            /* check if the cpu burst is over */
            if(cpu_proc != NULL && cpu_proc->burst_times[cpu_proc->todo] == cpu_proc->done) {
                cpu_proc->todo++;
                if(cpu_proc->burst_times[cpu_proc->todo] == -1) {
                    /* this process is completed its exection */

                    metrics[cpu_proc->proc_no].turnaround_time = clock_time - cpu_proc->arrival_time;

                    std::cout << "Process " << cpu_proc->proc_no << " is completed !!!!!!" << std::endl;
                    completed_proc++;
                    cpu_proc->status = COMPLETED;
                    cpu_proc = NULL;
                }
                else {
                    /* else move to IO */
                    cpu_proc->status = IO_WAIT;
                    cpu_proc->done = 0;
                    io_waiting_queue.push(cpu_proc);
                    std::cout << "Removed process " << cpu_proc->proc_no << " from CPU and moved to IO queue" << std::endl; 
                    cpu_proc = NULL;
                }
            }

            /* check if the io burst is over */
            if(io_proc != NULL && io_proc->burst_times[io_proc->todo] == io_proc->done) {
                io_proc->todo++;
                if(io_proc->burst_times[io_proc->todo] == -1) {
                    /* this process is completed its exection */

                    metrics[io_proc->proc_no].turnaround_time = clock_time - io_proc->arrival_time;


                    std::cout << "Process " << io_proc->proc_no << " is completed !!!!!!" << std::endl;
                    completed_proc++;
                    io_proc->status = COMPLETED;
                    io_proc = NULL;
                }
                else {
                    /* else move to ready queue */

                    metrics[io_proc->proc_no].last_arrival_time = clock_time;

                    io_proc->status = WAITING;
                    io_proc->done = 0;
                    ready_queue.push(io_proc);
                    std::cout << "Removed process " << io_proc->proc_no << " from IO and moved to ready queue" << std::endl; 
                    io_proc = NULL;
                }
            }
            
            if(cpu_proc != NULL && clock_time % quantum == 0) {
                /* preempt this process from cpu */
                do_preempt();
            }
            
            /* give cpu to next process */
            if(cpu_proc == NULL && !ready_queue.empty()) {
                cpu_proc = ready_queue.front();
                std::cout << "CPU given to process " << cpu_proc->proc_no << std::endl;
                ready_queue.pop();
                cpu_proc->status = RUNNING;

                assert(metrics[cpu_proc->proc_no].last_arrival_time != -1);
                metrics[cpu_proc->proc_no].waiting_time += clock_time - metrics[cpu_proc->proc_no].last_arrival_time;
                metrics[cpu_proc->proc_no].last_arrival_time = -1;
            }

            /* give io to next process */
            if(io_proc == NULL && !io_waiting_queue.empty()) {
                io_proc = io_waiting_queue.front();
                std::cout << "IO given to process " << io_proc->proc_no << std::endl;
                io_waiting_queue.pop();
                io_proc->status = IO_WAIT;
            }

            ++clock_time;
            std::cout << std::endl;

        }

    }
};

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << "path to input file" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);

    if (!inputFile.is_open()) {
        std::cerr << "Couldn't open the input file." << std::endl;
        return 1;
    }

    std::string line;  // arrival, CPU, IO, CPU, IO, ... -1
    int val;
    int proc_no = 0;
    RoundRobinSimulation RR;


    while(std::getline(inputFile, line)) {
        std::stringstream ss(line);

        Process *proc = new Process;  // FREE THE MEMORY LATER
        bool arrival = true;

        while(ss >> val) {
            if(ss.fail()) {
                std::cerr << "Error converting string to int on line: " << line << std::endl;
                return 1;
            }
            if(arrival) {
                proc->proc_no = proc_no++;
                proc->arrival_time = val;
                proc->status = OUT;
                proc->todo = proc->done = 0;
                arrival = false;
            }
            else {
                proc->burst_times.emplace_back(val);  // last value will be -1
            }
        }
        RR.proc_queue.push(proc);
    }

    RR.start();
    RR.show_metrics();

    inputFile.close();
    return 0;
}