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


int main() {
    std::cout << "Hello, World!" << std::endl;
    auto table1 = make_shared<Table>(5,3,BLUE);
    auto table2 = make_shared<Table>(5,3,RED);
    vector<shared_ptr<Table>> task;
    task.emplace_back(table1);
    task.emplace_back(table2);
    vector <Object> blocks, places;
    bool pick = true;
    bool drop = false;
    for (int i = 0; i < task.size(); ++i) {
        available_actions(task[i]->get_ptr(), pick, blocks);
        available_actions(task[i]->get_ptr(), drop, places);

    }
    copy(blocks.begin(), blocks.end(), ostream_iterator<Object>(cout, "\n"));

    cout << "drop " <<endl;
    copy(places.begin(), places.end(), ostream_iterator<Object>(cout, "\n"));

    return 0;
}