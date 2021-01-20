//
// Created by redwan on 1/16/21.
//
#include <algorithm>
#include <map>
#include "../include/Table.h"

#define DEBUG_INFO(x) cout << "\033[1;36m" << x << "\033[0m"
#define RECURSION_LIMIT (10000)
using namespace std;
namespace recursive_solver
{
    typedef pair<size_t, size_t> table_block_pair;
    inline void available_actions(weak_ptr<Table> table, bool operation, vector<table_block_pair>&result)
    {
        auto subject = table.lock();
        for (int i = 0; i < subject->blocks.size(); ++i) {
            if (!subject->status[i] ^ operation)
                result.push_back(make_pair(subject->get_id(), i));
        }
    }
    template <typename T>
    inline bool winning_state(TableList<T>& task)
    {
        bool result = true;
        for(auto t : task)
        {
            for (auto block: t->blocks)
            {
                bool isValidColor = (block.color != t->default_color) || (block.color == BLACK);
                result = result && isValidColor;
            }
        }
        return result;
    }
    template <typename T>
    inline void move(const table_block_pair& a1, const table_block_pair& a2, TableList<T>& task)

    {
        int i = task.find_index(a1.first);
        int j = task.find_index(a2.first);
        std::swap(task[i]->blocks[a1.second].color, task[j]->blocks[a2.second].color);
        swap(task[i]->status[a1.second], task[j]->status[a2.second]);
    }

    template <typename T>
    T choice(vector<T>& data)
    {
        vector<T> samples_out;
        std::sample(data.begin(), data.end(), std::back_inserter(samples_out),
                    1, std::mt19937{std::random_device{}()});
        return samples_out.at(0);
    }


    template <typename T>
    inline int solver(TableList<T>& task, int num_robots, map<size_t, size_t>& caches, int recursion_count = 0) {
        if (winning_state(task)) {
//            DEBUG_INFO ("[Info]: Found winning states  "<< task.get_hash() << endl);
            return 0;
        }
        else if (recursion_count > RECURSION_LIMIT)
        {
//            cerr << "[Info]: Not Found winning states" << endl;
            return -1;
        }
        // get available actions in terms of picks and places
        vector<table_block_pair> blocks, places;

        auto task_id_from = task.get_hash();
        bool pick = true;
        bool drop = false;
        for (int i = 0; i < task.size(); ++i) {
            available_actions(task[i]->get_ptr(), pick, blocks);
            available_actions(task[i]->get_ptr(), drop, places);
        }

        for (int j = 0; j < num_robots; ++j) {
            auto sample_pick = choice(blocks);
            auto sample_drop = choice(places);
            move(sample_pick, sample_drop, task);
        }
        auto task_id_to = task.get_hash();
        caches.insert(make_pair(task_id_to, task_id_from));
        return solver(task, num_robots, caches, ++recursion_count) + 1;

    }

    inline int solution_length(size_t start, size_t goal, map<size_t, size_t>&solution, bool verbose )
    {
        int count = 0;
        int max_iter = 100;
        while(goal != start )
        {
            if (verbose)
                cout << "["<< goal << "] <- " ;
            goal = solution[goal];
            if(--max_iter <= 0)
                break;

            ++count;
        }
        if (verbose)
            cout << "["<< goal << "] \n " ;
        return ++count;
    }

}