//
// Created by Александр on 06.12.2019.
//

#ifndef BACKEND_MY_MAP_CHAR_H
#define BACKEND_MY_MAP_CHAR_H

struct elem_map {
    char name_ = 0;
    int num_ = 0;
    elem_map(){}
    elem_map(char name, int num) {
        name_ = name;
        num_ = num;
    }
};

struct my_map_char {
    static const int max_size_ = 1024;

    int size_ = 0;
    elem_map data_[max_size_];

    int find(char c) {
        for (int i = 0; i < size_; i++)
            if (data_[i].name_ == c)
                return i;
        return -1;
    }

    int find(int key) {
        for (int i = 0; i < size_; i++)
            if (data_[i].num_ == key)
                return i;
        return -1;
    }

    int add(elem_map El) {
        if (size_ == max_size_) {
            return -1;
        }
        data_[size_] = El;
        return size_++;
    }

    int& operator[](char c) {
        int i = find(c);
        if (i == -1)
            i = add(elem_map(c, -1));
        return data_[i].num_;
    }

    char operator[](int key) {
        int i = find(key);
        if (i == -1)
            i = add(elem_map('\0', key));
        return data_[i].name_;
    }
};

#endif //BACKEND_MY_MAP_CHAR_H
