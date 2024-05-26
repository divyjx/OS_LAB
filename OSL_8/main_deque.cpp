#include <stdio.h>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>
#include <sstream>
#define assertm(exp, msg) assert(((void)msg, exp))

using namespace std;

enum PageStatus
{
    SWAP,
    PHY,
    MAIN
};

enum Policy
{
    FIFO,
    LRU,
    RANDOM
};

class PageManager
{
private:
    int total_frames;
    int memory_size;
    int swap_size;
    Policy policy;

    int page_faults;
    int curr_mm;
    int curr_ss;

    deque<int> memory_queue;

    vector<PageStatus> page_states;

public:
    PageManager(int total_frames, int memory_size, int swap_size, Policy policy)
        : total_frames(total_frames), memory_size(memory_size), swap_size(swap_size), policy(policy)
    {
    }

    ~PageManager(){memory_queue.clear(); page_states.clear();}

    void simulate(vector<int> &page_order, bool debug)
    {

        page_states.resize(total_frames + 1, PHY);
        page_faults = 0;
        curr_mm = 0;
        curr_ss = 0;
        memory_queue.clear();

        for (int i = 0; i < page_order.size(); i++)
        {

            int page = page_order[i];

            if (page_states[page] == PHY)
            {
                if (curr_mm == memory_size && curr_ss == swap_size)
                {
                    cerr << ("Error: main memory and swap space filled\n");
                    return;
                }
                else if (curr_mm == memory_size)
                {
                    replace_page(page);
                    curr_ss++;
                }
                else
                {
                    assign_page(page);
                    curr_mm++;
                }
                page_faults++;
            }
            else if (page_states[page] == SWAP)
            {
                if (curr_mm == memory_size)
                {
                    replace_page(page);
                }
                else
                {
                    assign_page(page);
                    curr_mm++;
                }
                page_faults++;
            }
            else
            {
                // cerr << "here" << endl;
                update_page(page);
            }

            if (debug)
            {
                cerr << page << endl;
                for (const auto &elem : memory_queue)
                    cerr << elem << " ";
                cerr << endl;
                for (int i = 0; i < total_frames; i++)
                    if (page_states[i + 1] == SWAP)
                        cerr << i + 1 << " ";
                
                cerr << endl 
                    //  << page_faults << endl
                     << endl;
                
            }
        }

        print_stat();
    }

    void print_stat()
    {
        string s = ((policy == FIFO) ? "FIFO" : ((policy == LRU) ? "LRU" : "RANDOM"));
        cerr << s;
        printf(": page faults %d\n", page_faults);
    }

    void update_page(int page)
    {
        if (policy == LRU)
        {
            cerr << "updated" << " ";
            memory_queue.erase(find(memory_queue.begin(), memory_queue.end(), page));
            memory_queue.push_back(page);
        }
    }

    void assign_page(int page)
    {
        memory_queue.push_back(page);
        cerr << "assigned" << " ";
        page_states[page] = MAIN;
    }

    void replace_page(int page)
    {
        cerr << "replaced" << "page "<<page<<" ";
        auto old_page = memory_queue.begin();
        int old_page_val = *old_page;
        if (page == 30) cerr << old_page_val << " ";

        if (policy == RANDOM)
        {
            srand(time(0));
            int candidate_page = rand() % memory_queue.size();
            old_page += candidate_page;
        }

        if (page == 30) cerr << "here" << " ";
        memory_queue.push_back(page);

        if (page == 30) cerr << "here" << " ";
        page_states[page] = MAIN;

        if (page == 30) cerr << "here" << " ";
        page_states[old_page_val] = SWAP;

        if (page == 30) cerr << "here" << " ";
        memory_queue.erase(old_page, memory_queue.end());

        if (page == 30) cerr << "here" << " ";
    }
};

int main(int argc, char *argv[])
{

    assertm((argc == 5), "Error: Invalid number of arguments");

    // assumption that only numbers are passed
    int total_frames = stoi(argv[1]);
    int mem_frames = stoi(argv[2]);
    int swap_frames = stoi(argv[3]);

    assertm((total_frames > 0 && mem_frames > 0 && swap_frames >= 0), "Error: Invalid input");

    vector<int> page_order;
    ifstream file(argv[4]);
    string line;
    getline(file, line);
    stringstream ss(line);
    int x;

    while (ss >> x)
    {
        assertm((x <= total_frames && x > 0), "Error: Data in test file have invalid frame numbers. Try increasing total physical frames.");
        page_order.push_back(x);
    }
    // cout << page_order.size() << endl;

    PageManager FIFO_PM(total_frames, mem_frames, swap_frames, FIFO);
    PageManager LRU_PM(total_frames, mem_frames, swap_frames, LRU);
    PageManager RANDOM_PM(total_frames, mem_frames, swap_frames, RANDOM);
    // FIFO_PM     .simulate(page_order, true);
    LRU_PM      .simulate(page_order, false);
    RANDOM_PM   .simulate(page_order, false);
}