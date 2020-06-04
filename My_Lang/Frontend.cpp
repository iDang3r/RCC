#include <iostream>
#include <cstdio>
#include <clocale>
#include <cstring>

using std::cin;
using std::cout;
using std::endl;

#include "my_file_lib.h"
#include "dump.h"
#include "Frontend.h"
#include "color_printf.h"

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    char*  input_file_name = strdup(X_("myLang.txt"));
    char* output_file_name = strdup(X_("tree_from_My_lang.txt"));

    try {
        Frontend Q(input_file_name, output_file_name);

        Q.vars_funcs_parsing();

//        Q.dump_picture();

        Q.put_tree_in_file();
    } catch (const char* str_error) {
        printf_red(str_error);
    }


    free(input_file_name);
    free(output_file_name);
    return 0;
}