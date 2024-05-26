#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <queue>
#include <iomanip>
#include <math.h>

using namespace std;

int new_pid()
{
    static int pid_init = 0;
    return pid_init++;
}

int countDigit(long long n)
{
    return floor(log10(n) + 1);
}

struct comp : public binary_function<pair<int, int>, pair<int, int>, bool>
{
    _GLIBCXX14_CONSTEXPR
    bool
    operator()(const pair<int, int> &__x, const pair<int, int> &__y) const
    {
        return __x.second > __y.second;
    }
};

class Process
{
public:
    int pid;
    int arrival;
    vector<int> burstTimes;
    vector<int> compTimes;
    vector<int> queueStart;
    int currBurst;
    int waitingTime;
    int execTime;
    int turnAroundTime;
    double penalty;
    Process()
    {
        currBurst = 0;
        waitingTime = 0;
        execTime = 0;
        turnAroundTime = 0;
    }

    ~Process()
    {
        this->burstTimes.clear();
        this->compTimes.clear();
        this->queueStart.clear();
    }
    void print()
    {
        int max_num = compTimes[compTimes.size() - 1];
        int max_width = countDigit(max_num);
        cout << "pid     : " << pid << "\n";
        cout << "arrival : " << arrival
             << "\nTAtime  : " << turnAroundTime
             << "\nWtime   : " << waitingTime
             << "\npenalty : " << penalty
             << "\nBursts  : ";
        for (auto &i : burstTimes)
            cout << setw(max_width) << i << " ";
        cout << "\nCompls  : ";
        for (auto &i : compTimes)
            cout << setw(max_width) << i << " ";
        // cout << "\nQstart : ";
        // for (auto &i : queueStart)
        //     cout << i << " ";

        cout << endl
             << endl;
    }
};

class Device
{
public:
    int runPid; // -1
    int runTime;
    bool isIO;
    int idleTime;
    Device(bool isIO = false)
    {
        isIO = isIO;
        runPid = -1;
        runTime = 0;
        idleTime = 0;
    }
};

int main(int argc, char *argv[])
{

    // Usage ./SJF <data_file> output.txt (default)
    // TODO: Validation

    /* ----------- Parsing Input File -----------*/
    string line;
    ifstream file;
    file.open(argv[1]);

    // file.open("process3.dat");
    vector<Process> pstore;
    // creating processes
    while (getline(file, line))
    {
        if (line[0] < '0' || line[0] > '9')
            continue;

        int it = 0; // used to parse line
        string numstr;
        while (line[it] != ' ')
            numstr.push_back(line[it++]);

        Process proc;
        proc.pid = new_pid();
        proc.arrival = (int)stoi(numstr);
        numstr.clear();
        it++;
        while (line[it] != '-')
        {
            if (line[it] == ' ' && !numstr.empty())
            {
                proc.burstTimes.push_back((int)stoi(numstr));
                numstr.clear();
            }
            else
            {
                numstr.push_back(line[it]);
            }
            it++;
        }
        proc.compTimes = vector<int>(proc.burstTimes.size(), -1);
        proc.queueStart = vector<int>(proc.burstTimes.size(), -1);
        pstore.push_back(proc);
    }

    /* ----------- Initializations ----------- */

    // init devices
    Device cpuDevice;
    Device ioDevice(false);

    // init Queues
    priority_queue<pair<int, int>, vector<pair<int, int>>, comp> readyQ;
    priority_queue<pair<int, int>, vector<pair<int, int>>, comp> blockQ;

    // pstore contains processes in order of arrival
    int pstoreIt = 0;
    int clock = 0;
    int completed = 0;

    // main simulation loop
    do
    {
        /* ----------- Add Arrivals -----------*/
        if (pstoreIt != pstore.size())
            while (pstore[pstoreIt].arrival == clock)
            {
                Process *currP = &pstore[pstoreIt];
                currP->queueStart[currP->currBurst] = clock;
                readyQ.push({currP->pid, currP->burstTimes[0]});
                // Report Metrics

                pstoreIt++;
                if (pstoreIt == pstore.size())
                    break;
            }

        /* ----------- Check CPU -----------*/
        bool pushBlock = false;
        pair<int, int> blockData;

        if (cpuDevice.runPid == -1 && !readyQ.empty())
        {
            // pick top from readyQ and run
            pair<int, int> rproc = readyQ.top();
            readyQ.pop();
            cpuDevice.runPid = rproc.first;
            cpuDevice.runTime = rproc.second;
        }
        // run CPU
        if (cpuDevice.runPid != -1)
        {
            // run current process
            cpuDevice.runTime--;
            if (cpuDevice.runTime == 0)
            {
                // Report Metric and prepare cpu
                Process *fproc = &pstore[cpuDevice.runPid];
                fproc->compTimes[fproc->currBurst] = clock + 1;
                fproc->currBurst++;
                if (fproc->currBurst == fproc->burstTimes.size())
                {
                    // Report Metric cuz process done
                    cout << "Process done pid : " << fproc->pid << endl;
                    completed++;
                }
                else
                {
                    // push after IO run
                    pushBlock = true;
                    blockData = {fproc->pid, fproc->burstTimes[fproc->currBurst]};
                }
                cpuDevice.runPid = -1;
            }
        }
        else
        {
            cpuDevice.idleTime++;
        }

        /* ----------- Check IO -----------*/
        if (ioDevice.runPid == -1 && !blockQ.empty())
        {
            // pick top from blockQ and run
            pair<int, int> bproc = blockQ.top();
            blockQ.pop();
            ioDevice.runPid = bproc.first;
            ioDevice.runTime = bproc.second;
        }
        // run IO
        if (ioDevice.runPid != -1)
        {
            // run current process
            ioDevice.runTime--;
            if (ioDevice.runTime == 0)
            {
                // Report Metric and prepare io
                Process *fproc = &pstore[ioDevice.runPid];
                fproc->compTimes[fproc->currBurst] = clock + 1;
                fproc->currBurst++;
                if (fproc->currBurst == fproc->burstTimes.size())
                {
                    // Report Metric cuz process done
                    cout << "Process done pid : " << fproc->pid << endl;
                    completed++;
                }
                else
                {
                    // push to readyQ (at start of next cycle)
                    fproc->queueStart[fproc->currBurst] = clock + 1;
                    readyQ.push({fproc->pid, fproc->burstTimes[fproc->currBurst]});
                }
                ioDevice.runPid = -1;
            }
        }
        else
        {
            ioDevice.idleTime++;
        }

        if (pushBlock)
        {
            pushBlock = false;
            blockQ.push(blockData);
        }

        clock++;
    } while (completed != pstore.size());

    cout << "\n--------------- Process Metrics ---------------\n";
    for (int i = 0; i < completed; i++)
    {
        // calculating metrics
        for (int j = 0; j < pstore[i].currBurst; j += 2)
        {
            pstore[i].waitingTime += (pstore[i].compTimes[j] - pstore[i].queueStart[j] - pstore[i].burstTimes[j]);
            pstore[i].execTime += pstore[i].burstTimes[j];
        }
        pstore[i].turnAroundTime = pstore[i].compTimes[pstore[i].currBurst - 1] - pstore[i].arrival;
        // pstore[i].penalty = ((pstore[i].waitingTime+pstore[i].execTime)*1.0)/pstore[i].execTime;
        pstore[i].penalty = (pstore[i].turnAroundTime * 1.0) / (pstore[i].execTime * 1.0);
        pstore[i].print();
    }
    cout << "\n--------------- System Metrics ---------------\n";
    double avgWT = 0, avgTT = 0, avgP = 0;
    double Throughput;
    for (int i = 0; i < pstore.size(); i++)
    {
        avgWT += 1.0 * pstore[i].waitingTime;
        avgTT += 1.0 * pstore[i].turnAroundTime;
        avgP += pstore[i].penalty;
    }
    avgWT /= pstore.size();
    avgTT /= pstore.size();
    avgP /= pstore.size();
    cout << "Total RunTime           : " << clock
         << "\nProcesses               : " << completed
         << "\nAverage Waiting Time    : " << avgWT
         << "\nAverage Turnaround Time : " << avgTT
         << "\nAverage Penalty         : " << avgP
         //  << "\ncpuIdleTime             : " << cpuDevice.idleTime
         << "\nThroughput              : " << (completed * 1.0) / clock << endl;

    // /* ---------- exporting process data ----------- */
    // cerr << "pid,TAtime,Wtime,penalty" << endl;

    // for (const auto &process : pstore)
    // {
    //     cerr << process.pid << ","
    //          << process.turnAroundTime << ","
    //          << process.waitingTime << ","
    //          << process.penalty << endl;
    // }

    return 0;
}
