#include <iostream>
#include "include/Table.h"

void available_actions(weak_ptr<Table> table, bool operation, vector<Object>&result)
{
    auto subject = table.lock();
    for (int i = 0; i < subject->blocks.size(); ++i) {
        if (!subject->status[i] ^ operation)
            result.push_back(subject->blocks[i]);
    }
}

template <typename T>
bool winning_state(TableList<T>& task)
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



int main() {
    std::cout << "Hello, World!" << std::endl;
    srand (1234); // random seed
    auto table1 = make_shared<Table>(5,3,BLUE);
    auto table2 = make_shared<Table>(5,3,RED);
    TableList<shared_ptr<Table>> task;

    // get hash value for each table
    task.emplace_back(table1);
    task.emplace_back(table2);

    cout << task.get_hash() << "| win " <<
    winning_state(task) << endl;
    // get available actions in terms of picks and places
    vector <Object> blocks, places;
    bool pick = true;
    bool drop = false;
    for (int i = 0; i < task.size(); ++i) {
        available_actions(task[i]->get_ptr(), pick, blocks);
        available_actions(task[i]->get_ptr(), drop, places);
    }



    // debug
    copy(blocks.begin(), blocks.end(), ostream_iterator<Object>(cout, "\n"));
    cout << "drop " <<endl;
    copy(places.begin(), places.end(), ostream_iterator<Object>(cout, "\n"));

    return 0;
}