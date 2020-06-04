//
// Created by Александр on 16.12.2019.
//

#ifndef PY_BACKEND_PY_BACKEND_H
#define PY_BACKEND_PY_BACKEND_H


#include "dump.h"
#include "Tree.h"
#include "my_map_char.h"

const char open_bracket  = '{';
const char close_bracket = '}';
const char nil_node      = '"';

namespace type {
    enum {
        num = 0,
        chr = 1,
    };
}

union uni_data {
    int num_;
    char name_;

    uni_data(int num) : num_(num) {}
    uni_data(char name) : name_(name) {}
};


class Py_Backend : Tree<uni_data> {
public:

    FILE* in  = nullptr;
    FILE* out = nullptr;

    bool is_main = 0;

    my_map_char GLOBAL;

    node<uni_data>* during = nullptr;

    Py_Backend(const char* input_file_name, const char* output_file_name) {
        in  = fopen(input_file_name,  "r");
        out = fopen(output_file_name, "w");

        if (in == nullptr) {
            throw "input file was not opened";
        }

        if (out == nullptr) {
            throw "output file was not opened";
        }
    }

    ~Py_Backend() {
        fclose(in);
        fclose(out);
    }

    node<uni_data>* create(int type, uni_data val, node<uni_data>* left = nullptr, node<uni_data>* right = nullptr) {
        return new node<uni_data>(type, val, left, right);
    }

    char* build_tree_rec(char* buff, node<uni_data>* *el) {
        assert(el != nullptr);
        assert(buff != nullptr);

        assert(*buff == open_bracket);
        buff++;
        int n = 0;

        // MAIN INFO

        if (('0' <= *buff && *buff <= '9') || (*buff == '-' && '0' <= *(buff + 1) && *(buff + 1) <= '9')) {

            int x = 0;

            sscanf(buff, "%d%n", &x, &n);
            buff += n;

            w(x);

            *el = create(type::num, x);

        } else {

            w(*buff);

            *el = create(type::chr, *buff);
            buff++;
        }

        if (*buff == close_bracket)
            return buff + 1;


        if (*buff == nil_node) {
            buff++;
        } else {
            buff = build_tree_rec(buff, &((*el)->left_));
        }

        if (*buff == nil_node) {
            buff++;
        } else {
            buff = build_tree_rec(buff, &((*el)->right_));
        }

        assert(*buff == close_bracket);
        return buff + 1;
    }

    int get_tree_from_file() {

        char* buff = nullptr;
        size_t buff_size = get_buffer(in, &buff);

        w(buff);

        build_tree_rec(buff, &head_);

        free(buff);

        return 0;
    }

    void put_tabs(int k) {
        for (int i = 0; i < k; i++)
            fprintf(out, "  ");
    }

    void calculate(node<uni_data>* el) {

        if (el->type_ == type::num) { // num

            fprintf(out, "%d", el->data_.num_);

        } else if (el->left_ != nullptr) { // operation

            if (el->data_.name_ == 'T') {

                fprintf(out, "sqrt(");

                calculate(el->left_);

                fprintf(out, ")");

                return;
            }

            fprintf(out, "(");

            calculate(el->left_);

            switch (el->data_.name_) {

                case '+':
                    fprintf(out, " + ");
                    break;

                case '-':
                    fprintf(out, " - ");
                    break;

                case '*':
                    fprintf(out, " * ");
                    break;

                case '/':
                    fprintf(out, " // ");
                    break;

                default:
                    throw "unknown operation";
            }

            calculate(el->right_);

            fprintf(out, ")");

        } else if (el->right_ == nullptr) { // variable

            fprintf(out, "%c", el->data_.name_);

        } else { // function

            node<uni_data>* tmp = el->right_;

            fprintf(out, "%c(", el->data_.name_);

            bool is_first = 1;

            while (tmp->left_) {

                if (!is_first)
                    fprintf(out, ", ");

                calculate(tmp->left_);

                if (tmp->right_)
                    tmp = tmp->right_;
                else
                    break;

                is_first = 0;
            }

            fprintf(out, ")");

        }

    }

    void asembl_block(node<uni_data>* el, int tabs) {

        if (el->left_->type_ == type::chr && (el->left_->data_.name_ == '<' || el->left_->data_.name_ == '>')) { // in/out

            if (el->left_->data_.name_ == '>') { // in

                put_tabs(tabs);
                fprintf(out, "%c = int(input())\n", el->left_->right_->data_.name_);

            } else { // out

                put_tabs(tabs);
                fprintf(out, "print(");
                calculate(el->left_->right_);
                fprintf(out, ")\n");

            }

        } else if (el->left_->data_.name_ == '~') {

            put_tabs(tabs);
            fprintf(out, "%c = ", el->left_->left_->data_.name_);
            calculate(el->left_->right_);
            fprintf(out, "\n");

        } else if (el->left_->data_.name_ == 'I') {

            put_tabs(tabs);
            fprintf(out, "if ");
            calculate(el->left_->left_->left_);

            switch (el->left_->left_->data_.name_) {
                case '>':

                    fprintf(out, " > ");

                    break;

                case '<':

                    fprintf(out, " < ");

                    break;

                case '~':

                    fprintf(out, " == ");

                    break;

                case '!':

                    fprintf(out, " != ");

                    break;

            }

            calculate(el->left_->left_->right_);

            fprintf(out, ":\n");

            asembl_block(el->left_->right_->left_, tabs + 1);

            put_tabs(tabs);
            fprintf(out, "else:\n");

            asembl_block(el->left_->right_->right_, tabs + 1);

            fprintf(out, "\n");

        } else if (el->left_->data_.name_ == 'W') {

//            fprintf(out, "jmp_%d:", while_jmp);
//
//            calculate(el->left_->left_->left_, count_vars, myMapChar);
//            calculate(el->left_->left_->right_, count_vars, myMapChar);
//
//            switch (el->left_->left_->data_.name_) {
//                case '>':
//
//                    fprintf(out, "jbe jmp_%d\n", stop_jmp);
//
//                    break;
//
//                case '<':
//
//                    fprintf(out, "jae jmp_%d\n", stop_jmp);
//
//                    break;
//
//                case '~':
//
//                    fprintf(out, "jne jmp_%d\n", stop_jmp);
//
//                    break;
//
//                case '!':
//
//                    fprintf(out, "je jmp_%d\n", stop_jmp);
//
//                    break;
//
//            }
//
//            asembl_block(el->left_->right_, tabs);
//
//            fprintf(out, "jmp jmp_%d\n", while_jmp);
//            fprintf(out, "jmp_%d:\n", stop_jmp);

        } else if (el->left_->data_.name_ == '@') {

            if (is_main)
                return;

            put_tabs(tabs);
            fprintf(out, "return ");

            calculate(el->left_->right_);

            fprintf(out, "\n");
        }

        if (el->right_)
            asembl_block(el->right_, tabs);
    }

    void vars_funcs_parsing() {

        fprintf(out, "from math import *\n");

        during = head_;

        while (during->left_ && during->left_->type_ == type::chr && during->left_->data_.name_ == 'V') {

            cout << "New var: " << during->left_->right_->data_.name_ << endl;

            GLOBAL[during->left_->right_->data_.name_] = 1;

            fprintf(out, "%c = 0\n", during->left_->right_->data_.name_);

            during = during->right_;
        }

        while (during->left_ && during->left_->type_ == type::chr && during->left_->data_.name_ == 'F') {

            int count_vars = 0;

            node<uni_data>* vars_node_tmp = during->left_->right_->left_;

            bool is_first = 1;

            fprintf(out, "def %c(", during->left_->right_->data_.name_);

            while (vars_node_tmp->left_) {

                if (!is_first)
                    fprintf(out, ", ");
                fprintf(out, "%c", vars_node_tmp->left_->data_.name_);

                vars_node_tmp = vars_node_tmp->right_;

                is_first = 0;
            }

            fprintf(out, "):\n");

            for (int i = 0; i < GLOBAL.size_; i++) {
                put_tabs(1);
                fprintf(out, "global %c\n", GLOBAL.data_[i].name_);
            }

            asembl_block(during->left_->right_->right_, 1);

            during = during->right_;
        }

        is_main = 1;

        assert(during->data_.name_ == '$');

        asembl_block(during->right_, 0);

    }

};


#endif //PY_BACKEND_PY_BACKEND_H











