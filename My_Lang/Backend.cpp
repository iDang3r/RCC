#include <iostream>
#include <cstdio>
#include <cstring>
#include <cassert>

using std::cin;
using std::cout;
using std::endl;

#include "my_file_lib.h"
#include "dump.h"
#include "color_printf.h"
#include "Backend.h"
#include "my_map_char.h"


int main() {
    char*  input_file_name = strdup(X_("tree_from_My_lang.txt"));
    char* output_file_name = strdup(X_("asm_code_from_my_Lang.txt"));

    try {

        Backend Q(input_file_name, output_file_name);

        Q.get_tree_from_file();

        Q.vars_funcs_parsing();

//        Q.dump_picture();

    } catch (const char* str_error) {
        printf_red(str_error);
    }

    free(input_file_name);
    free(output_file_name);
    return 0;
}