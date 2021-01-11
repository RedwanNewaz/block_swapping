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
#include <string>
#include <bitset>
#include <sstream>
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
    COLOR color;

    friend ostream &operator<<(ostream &os, const Object &object) {
        os << "(table_id: " << object.table_id << " block_id: " << object.block_id << " color: " << object.color << ")";
        return os;
    }
};


class Table: public enable_shared_from_this<Table>
{
    using TablePtr = shared_ptr<Table>;
public:
    Table(int num_places, int num_blocks, COLOR color):
    num_places_(num_places), default_color(color)
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
    TablePtr get_ptr()
    {
        return shared_from_this();
    }

    std::size_t get_hash()
    {
        ostringstream os;
        copy(status.begin(), status.end(),
             ostream_iterator<bool>(os, ""));
        return std::hash<std::string>{}(os.str()) ;
    }

    int get_id(){ return id_;};

protected:
    int num_places_, id_;
public:
    vector<bool> status;
    vector<Object> blocks;
    COLOR default_color;
};

template <typename T>
class TableList:public vector<T>
{
public:

    size_t get_hash()
    {
        size_t value;
        for (int i = 0; i < this->size(); ++i)
        {
            if( i == 0)
                value = this->at(i)->get_hash();
            else
                value = value >> this->at(i)->get_hash();
        }
        return value;
    }

    int find_index(int table_id)
    {
        for (int i = 0; i < this->size(); ++i)
            if( this->at(i)->get_id() == table_id)
                return i;
    }

};


#endif //TAMP_CPP_TABLE_H
