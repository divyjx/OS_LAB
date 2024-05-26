#include <stdio.h>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <queue>
#include <map>
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

    map<int, int> memory_map;
    vector<PageStatus> page_states;

public:
    PageManager(int total_frames, int memory_size, int swap_size, Policy policy)
        : total_frames(total_frames), memory_size(memory_size), swap_size(swap_size), policy(policy)
    {
    }

    void simulate(vector<int> &page_order)
    {

        page_states.resize(total_frames + 1, PHY);
        page_faults = 0;
        curr_mm = 0;
        curr_ss = 0;
        memory_map.clear();

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
                    replace_page(i, page);
                    curr_ss++;
                }
                else
                {
                    assign_page(i, page);
                    curr_mm++;
                }
                page_faults++;
            }
            else if (page_states[page] == SWAP)
            {
                if (curr_mm == memory_size)
                {
                    replace_page(i, page);
                }
                else
                {
                    assign_page(i, page);
                    curr_mm++;
                }
                page_faults++;
            }
            else
            {
                update_page(i, page);
            }
        }

        print_stat();
    }

    void print_stat()
    {
        string s = ((policy == FIFO) ? "FIFO" : ((policy == LRU) ? "LRU" : "RANDOM"));
        cout << s;
        printf(": page faults %d\n", page_faults);
    }

    void update_page(int &i, int page)
    {
        if (policy == LRU)
        {
            auto it = memory_map.begin();
            while (it->second!=page) i++;
            memory_map.erase(it);
            memory_map[i] = page;
        }
    }

    void assign_page(int &i, int page)
    {
        memory_map[i] = page;
        page_states[page] = MAIN;
    }

    void replace_page(int i, int page)
    {
        auto old_page = memory_map.begin();

        if (policy == RANDOM)
        {
            srand(time(0));
            int candidate_page = rand() % memory_map.size();
            while (candidate_page--)
                old_page++;
        }

        memory_map[i] = page;
        page_states[page] = MAIN;
        page_states[old_page->second] = SWAP;
        memory_map.erase(old_page);
    }
};

void SimulateFIFO(vector<int> &page_order, int total_frames, int mem_frames, int swap_frames)
{

    int page_faults = 0;
    int curr_mm = 0;
    int curr_ss = 0;

    vector<PageStatus> page_states(total_frames + 1, PHY);

    map<int, int> memory_map; // stores incoming time , frame number

    for (int i = 0; i < page_order.size(); i++)
    {

        int page = page_order[i];

        if (page_states[page] == PHY)
        {
            if (curr_mm == mem_frames && curr_ss == swap_frames)
            { // full mm and ss
                cerr << ("FIFO Error: main memory and swap space filled\n");
                return;
            }
            else if (curr_mm == mem_frames)
            {
                // perform swap here
                pair<int, int> first_page = *memory_map.begin();
                memory_map.erase(memory_map.begin());
                curr_ss++;
                memory_map[i] = page;
                page_states[page] = MAIN;
                page_states[first_page.second] = SWAP;
            }
            else
            {
                memory_map[i] = page;
                page_states[page] = MAIN;
                curr_mm++;
            }
            page_faults++;
        }
        else if (page_states[page] == SWAP)
        {
            if (curr_mm == mem_frames)
            {
                // perform swap here
                pair<int, int> first_page = *memory_map.begin();
                memory_map.erase(memory_map.begin());
                // curr_ss++;
                memory_map[i] = page;
                page_states[page] = MAIN;
                page_states[first_page.second] = SWAP;
            }
            else
            { // can't happen
                memory_map[i] = page;
                page_states[page] = MAIN;
                curr_mm++;
            }
            page_faults++;
        }
        else
        {
            // Do nothing here
            // pair<int, int> first_page = *memory_map.begin();
            // memory_map.erase(memory_map.begin());
            // memory_map[i] = page;
        }
    }

    printf("FIFO: page faults %d\n", page_faults);
}

void SimulateLRU(vector<int> &page_order, int total_frames, int mem_frames, int swap_frames)
{
    int page_faults = 0;
    int curr_mm = 0;
    int curr_ss = 0;

    vector<PageStatus> page_states(total_frames + 1, PHY);

    map<int, int> memory_map; // stores incoming time , frame number

    for (int i = 0; i < page_order.size(); i++)
    {

        int page = page_order[i];

        if (page_states[page] == PHY)
        {
            if (curr_mm == mem_frames && curr_ss == swap_frames)
            { // full mm and ss
                cerr << ("LRU Error: main memory and swap space filled\n");
                return;
            }
            else if (curr_mm == mem_frames)
            {
                // perform swap here
                pair<int, int> first_page = *memory_map.begin();
                memory_map.erase(memory_map.begin());
                curr_ss++;
                memory_map[i] = page;
                page_states[page] = MAIN;
                page_states[first_page.second] = SWAP;
            }
            else
            {
                memory_map[i] = page;
                page_states[page] = MAIN;
                curr_mm++;
            }
            page_faults++;
        }
        else if (page_states[page] == SWAP)
        {
            if (curr_mm == mem_frames)
            {
                // perform swap here
                pair<int, int> first_page = *memory_map.begin();
                memory_map.erase(memory_map.begin());
                // curr_ss++;
                memory_map[i] = page;
                page_states[page] = MAIN;
                page_states[first_page.second] = SWAP;
            }
            else
            { // can't happen
                memory_map[i] = page;
                page_states[page] = MAIN;
                curr_mm++;
            }
            page_faults++;
        }
        else
        {
            pair<int, int> first_page = *memory_map.begin();
            memory_map.erase(memory_map.begin());
            memory_map[i] = page;
        }
    }

    printf("LRU: page faults %d\n", page_faults);
}

void SimulateRandom(vector<int> &page_order, int total_frames, int mem_frames, int swap_frames)
{
    int page_faults = 0;
    int curr_mm = 0;
    int curr_ss = 0;

    vector<PageStatus> page_states(total_frames + 1, PHY);

    map<int, int> memory_map; // stores incoming time , frame number

    for (int i = 0; i < page_order.size(); i++)
    {

        int page = page_order[i];

        if (page_states[page] == PHY)
        {
            if (curr_mm == mem_frames && curr_ss == swap_frames)
            { // full mm and ss
                cerr << ("LRU Error: main memory and swap space filled\n");
                return;
            }
            else if (curr_mm == mem_frames)
            {

                // perform swap here on a random page
                srand(time(0)+10);
                int candidate_page = rand() % memory_map.size();
                auto it = memory_map.begin();
                while (candidate_page--)
                    it++;

                pair<int, int> rem_page = *it;
                memory_map.erase(it);
                curr_ss++;
                memory_map[i] = page;
                page_states[page] = MAIN;
                page_states[rem_page.second] = SWAP;
            }
            else
            {
                memory_map[i] = page;
                page_states[page] = MAIN;
                curr_mm++;
            }
            page_faults++;
        }
        else if (page_states[page] == SWAP)
        {
            if (curr_mm == mem_frames)
            {

                // perform swap here on a random page

                srand(time(0));
                int candidate_page = rand() % memory_map.size();
                auto it = memory_map.begin();
                while (candidate_page--)
                    it++;

                pair<int, int> rem_page = *it;
                memory_map.erase(it);
                // curr_ss++;
                memory_map[i] = page;
                page_states[page] = MAIN;
                page_states[rem_page.second] = SWAP;
            }
            else
            { // can't happen
                memory_map[i] = page;
                page_states[page] = MAIN;
                curr_mm++;
            }
            page_faults++;
        }
        else
        {
            // pair<int, int> first_page = *memory_map.begin();
            // memory_map.erase(memory_map.begin());
            // memory_map[i] = page;
        }
    }

    printf("Random: page faults %d\n", page_faults);
}

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

    while (!file.eof())
    {
        int x;
        file >> x;
        assertm((x <= total_frames && x > 0), "Error: Data in test file have invalid frame numbers. Try increasing total physical frames.");
        page_order.push_back(x);
    }
    // cout << page_order.size() <<" "<< page_order[page_order.size()-1];
    // srand(time(0));
    // cout << rand()%100 << endl;
    SimulateFIFO(page_order, total_frames, mem_frames, swap_frames);
    SimulateLRU(page_order, total_frames, mem_frames, swap_frames);
    SimulateRandom(page_order, total_frames, mem_frames, swap_frames);

    PageManager FIFO_PM = PageManager(total_frames, mem_frames, swap_frames, FIFO);
    PageManager LRU_PM = PageManager(total_frames, mem_frames, swap_frames, LRU);
    PageManager RANDOM_PM = PageManager(total_frames, mem_frames, swap_frames, RANDOM);
    FIFO_PM.simulate(page_order);
    LRU_PM.simulate(page_order);
    RANDOM_PM.simulate(page_order);
}