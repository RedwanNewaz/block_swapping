//
// Created by redwan on 1/20/21.
//

#ifndef TAMP_CPP_TASK_H
#define TAMP_CPP_TASK_H
#include <iostream>
#include <deque>
#include <vector>
#include <cassert>
#include <iterator>
using namespace std;
class Task
{
public:
    Task(int num_robot)
    {
        task_.resize(num_robot);
        table_.resize(num_robot);
    }
    void add_task(int robot_id, int from , int to)
    {
        assert(robot_id < task_.size());
        task_[robot_id].push_back(from);
        task_[robot_id].push_back(to);
    }
    void add_table(int robot_id, int from , int to)
    {
        assert(robot_id < table_.size());
        table_[robot_id].push_back(from);
        table_[robot_id].push_back(to);
    }
    deque<int> operator[](int robot_id)
    {
        assert(robot_id < task_.size());
        return task_[robot_id];
    }
    int get_table(int robot_id, int block_id)
    {
        return table_[robot_id][block_id];
    }

    friend ostream &operator<<(ostream &os, const Task &task) {
        os << "[states]:  | ";
        copy(task.states_.begin(),task.states_.end(), ostream_iterator<std::size_t>(os, " ") );
        os<<endl;
        for (int i = 0; i < task.task_.size(); ++i) {
            os << "[task]: blocks " << i+1 << " | ";
            copy(task.task_[i].begin(),task.task_[i].end(), ostream_iterator<int>(os, " ") );
            os<<endl;
            os << "[task]: tables " << i+1 << " | ";
            copy(task.table_[i].begin(),task.table_[i].end(), ostream_iterator<int>(os, " ") );
            os<<endl;
        }

        return os;
    }
    void add_state(const size_t& state)
    {
        states_.push_back(state);
    }
    size_t size() const
    {
        return states_.size();
    }

private:
    vector<deque<int>> task_, table_;
    deque<size_t> states_;

};


#endif //TAMP_CPP_TASK_H
