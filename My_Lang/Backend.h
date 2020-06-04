//
// Created by Александр on 06.12.2019.
//

#ifndef BACKEND_BACKEND_H
#define BACKEND_BACKEND_H

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

class Backend : Tree<uni_data> {
public:

    FILE* in  = nullptr;
    FILE* out = nullptr;

    bool is_func['z' + 1]{};

    node<uni_data>* during = nullptr;

    my_map_char vars;
    my_map_char funcs;

    int jmp_counter = 1;

    int free_memory = 10;

    // Methods

    Backend(const char* input_file_name, const char* output_file_name) {
        in  = fopen(input_file_name,  "r");
        out = fopen(output_file_name, "w");

        if (in == nullptr) {
            throw "input file was not opened";
        }

        if (out == nullptr) {
            throw "output file was not opened";
        }
    }

    ~Backend() {
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

    void dump_picture_rec(FILE* out_, node<uni_data>* el, int last = -1) {

        fprintf(out_, "class %p {\n", el);

        if (last == 1) {
            fprintf(out_, "right\n");
        } else if (last == 0) {
            fprintf(out_, "left\n");
        } else {
            fprintf(out_, "Default\n");
        }

        fprintf(out_, "--\n");

        {  // MAIN CONTENT PRINTING

            if (el->type_ == type::chr) {
                fprintf(out_, "%c\n", el->data_.name_);
            } else { // type::num
                fprintf(out_, "%d\n", el->data_.num_);
            }

        }

        if (el->right_) {
            fprintf(out_, "--\n");
            fprintf(out_, "right: %p\n", el->right_);
        }

        if (el->left_) {
            fprintf(out_, "--\n");
            fprintf(out_, "left: %p\n", el->left_);
        }

        fprintf(out_, "}\n");

        if (el->right_) {
            fprintf(out_, "%p -down-> %p\n", el, el->right_);

            dump_picture_rec(out_, el->right_, 1);
        }

        if (el->left_) {
            fprintf(out_, "%p -down-> %p\n", el, el->left_);

            dump_picture_rec(out_, el->left_, 0);
        }
    }

    void dump_picture() {

        FILE* out_ = fopen("/Users/alex/Desktop/pictures/Backend.pu", "w");

        if (out_ == nullptr) {
            dump(DUMP_INFO, "file was not opened");
            return;
        }

        fprintf(out_, "@startuml\n !define DARKORANGE\n !include style.puml\n class head_\n");

        fprintf(out_, "head_ -down-> %p\n", head_);

        dump_picture_rec(out_, head_);

        fprintf(out_, "@enduml\n");
        fclose(out_);

        system("java -jar --illegal-access=warn /Users/alex/plantuml.jar -tsvg /Users/alex/Desktop/pictures/Backend.pu");
        system("open /Users/alex/Desktop/pictures/Backend.svg");

    }

    void calculate(node<uni_data>* el, int count_vars, my_map_char &myMapChar) {

        if (el->type_ == type::num) { // num

            fprintf(out, "push %d\n", el->data_.num_);

        } else if (el->left_ != nullptr) { // operation

            if (el->data_.name_ == 'T') {

                calculate(el->left_, count_vars, myMapChar);

                fprintf(out, "sqrt\n");

                return;
            }

            calculate(el->left_,  count_vars, myMapChar);
            calculate(el->right_, count_vars, myMapChar);

            switch (el->data_.name_) {

                case '+':
                    fprintf(out, "add\n");
                    break;

                case '-':
                    fprintf(out, "sub\n");
                    break;

                case '*':
                    fprintf(out, "mul\n");
                    break;

                case '/':
                    fprintf(out, "div\n");
                    break;

                default:
                    throw "unknown operation";
            }

        } else if (el->right_ == nullptr) { // variable

            if (myMapChar[el->data_.name_] != -1) {
                fprintf(out, "pushm [ax + %d]\n", myMapChar[el->data_.name_]);
            } else {
                fprintf(out, "pushm [%d]\n", vars[el->data_.name_]);
            }

        } else { // function

            node<uni_data>* tmp = el->right_;

            while (tmp->left_) {

                calculate(tmp->left_, count_vars, myMapChar);

                if (tmp->right_)
                    tmp = tmp->right_;
                else
                    break;
            }

            fprintf(out, "pushr ax\n");
            fprintf(out, "push %d\n", count_vars);
            fprintf(out, "add\n");
            fprintf(out, "popr ax\n");

            fprintf(out, "call %c\n", el->data_.name_);

            fprintf(out, "pushr ax\n");
            fprintf(out, "push %d\n", count_vars);
            fprintf(out, "sub\n");
            fprintf(out, "popr ax\n");
        }

    }

    void asembl_block(node<uni_data>* el, int count_vars, my_map_char &myMapChar) {

        if (el->left_->type_ == type::chr && (el->left_->data_.name_ == '<' || el->left_->data_.name_ == '>')) { // in/out

            if (el->left_->data_.name_ == '>') { // in

                fprintf(out, "in\n");
                if (myMapChar[el->left_->right_->data_.name_] != -1) { // local var

                    fprintf(out, "popm [ax + %d]\n", myMapChar[el->left_->right_->data_.name_]);

                } else {

                    fprintf(out, "popm [%d]\n", vars[el->left_->right_->data_.name_]);

                }

            } else { // out

                calculate(el->left_->right_, count_vars, myMapChar);
                fprintf(out, "out\n");

            }

        } else if (el->left_->data_.name_ == '~') {

            calculate(el->left_->right_, count_vars, myMapChar);

            if (myMapChar[el->left_->left_->data_.name_] != -1) { // local var

                fprintf(out, "popm [ax + %d]\n", myMapChar[el->left_->left_->data_.name_]);

            } else {

                fprintf(out, "popm [%d]\n", vars[el->left_->left_->data_.name_]);

            }

        } else if (el->left_->data_.name_ == 'I') {

            calculate(el->left_->left_->left_, count_vars, myMapChar);
            calculate(el->left_->left_->right_, count_vars, myMapChar);

            int else_jmp = jmp_counter++;
            int out_jmp  = jmp_counter++;

            switch (el->left_->left_->data_.name_) {
                case '>':

                    fprintf(out, "jbe jmp_%d\n", else_jmp);

                    break;

                case '<':

                    fprintf(out, "jae jmp_%d\n", else_jmp);

                    break;

                case '~':

                    fprintf(out, "jne jmp_%d\n", else_jmp);

                    break;

                case '!':

                    fprintf(out, "je jmp_%d\n", else_jmp);

                    break;

            }

            asembl_block(el->left_->right_->left_, count_vars, myMapChar);

            fprintf(out, "jmp jmp_%d\n", out_jmp);
            fprintf(out, "jmp_%d:\n", else_jmp);

            asembl_block(el->left_->right_->right_, count_vars, myMapChar);

            fprintf(out, "jmp_%d:\n", out_jmp);
        } else if (el->left_->data_.name_ == 'W') {

            int while_jmp = jmp_counter++;
            int stop_jmp  = jmp_counter++;

            fprintf(out, "jmp_%d:", while_jmp);

            calculate(el->left_->left_->left_, count_vars, myMapChar);
            calculate(el->left_->left_->right_, count_vars, myMapChar);

            switch (el->left_->left_->data_.name_) {
                case '>':

                    fprintf(out, "jbe jmp_%d\n", stop_jmp);

                    break;

                case '<':

                    fprintf(out, "jae jmp_%d\n", stop_jmp);

                    break;

                case '~':

                    fprintf(out, "jne jmp_%d\n", stop_jmp);

                    break;

                case '!':

                    fprintf(out, "je jmp_%d\n", stop_jmp);

                    break;

            }

            asembl_block(el->left_->right_, count_vars, myMapChar);

            fprintf(out, "jmp jmp_%d\n", while_jmp);
            fprintf(out, "jmp_%d:\n", stop_jmp);

        } else if (el->left_->data_.name_ == '@') {

            calculate(el->left_->right_, count_vars, myMapChar);

        }

        if (el->right_)
            asembl_block(el->right_, count_vars, myMapChar);
    }

    void vars_funcs_parsing() {

        during = head_;

        while (during->left_ && during->left_->type_ == type::chr && during->left_->data_.name_ == 'V') {

            vars[during->left_->right_->data_.name_] = free_memory++;
            cout << "New var: " << during->left_->right_->data_.name_ << endl;

            during = during->right_;
        }

        fprintf(out, "push %d\n", free_memory + 2);
        fprintf(out, "popr ax\n");
        fprintf(out, "jmp main\n");
        fprintf(out, "\n");

        while (during->left_ && during->left_->type_ == type::chr && during->left_->data_.name_ == 'F') {

            int count_vars = 0;

            node<uni_data>* vars_node_tmp = during->left_->right_->left_;
            my_map_char func_vars_tmp;

            fprintf(out, "%c:\n", during->left_->right_->data_.name_);

            while (vars_node_tmp->left_) {

                func_vars_tmp[vars_node_tmp->left_->data_.name_] = count_vars++;

                vars_node_tmp = vars_node_tmp->right_;
            }

            for (int i = count_vars - 1; i >= 0; i--) {
                fprintf(out, "popm [ax + %d]\n", i);
            }

            asembl_block(during->left_->right_->right_, count_vars, func_vars_tmp);

            fprintf(out, "ret\n");

            during = during->right_;
        }

        assert(during->data_.name_ == '$');

        fprintf(out, "\n\n");
        fprintf(out, "main:\n");

        my_map_char fake;

        asembl_block(during->right_, 0, fake);

    }

};

#endif //BACKEND_BACKEND_H
