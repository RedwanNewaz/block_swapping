//
// Created by Redwan Newaz on 1/11/21.
//
#include <iostream>
#include <memory>
#include <cassert>
#include <vector>
#include <random>
#include <ostream>
#include <iterator>
using namespace std;
#ifndef TAMP_CPP_TABLE_H
#define TAMP_CPP_TABLE_H

enum COLOR
{
    RED,
    BLUE,
    BLACK
};


struct Object
{
    const int table_id, block_id;
    const COLOR color;

    friend ostream &operator<<(ostream &os, const Object &object) {
        os << "(table_id: " << object.table_id << " block_id: " << object.block_id << " color: " << object.color << ")";
        return os;
    }
};


class Table: public enable_shared_from_this<Table>
{
public:
    Table(int num_places, int num_blocks, COLOR color):
    num_places_(num_places)
    {
        assert(num_blocks<num_places && "number of places must be greater than number of blocks");
        status.resize(num_places_);
        for (int i = 0; i < num_blocks; ++i) {
            status[i] = true;
        }
        random_device randomDevice;
        mt19937 gt(randomDevice());
        shuffle(status.begin(), status.end(), gt);
        id_ = rand();
        int count = 0;
        for (auto item: status)
        {
            if (item)
                blocks.emplace_back(Object{id_, count, color});
            else
                blocks.emplace_back(Object{id_, count, BLACK});
            ++count;
        }
//        cout << "[Table] #places "<< num_places  << endl;
//        copy(blocks.begin(), blocks.end(), ostream_iterator<Object>(cout, "\n"));
//        cout << endl;
    }
    shared_ptr<Table> get_ptr()
    {
        return shared_from_this();
    }

protected:
    int num_places_, id_;
public:
    vector<bool> status;
    vector<Object> blocks;
};


#endif //TAMP_CPP_TABLE_H
