#include <cstdio>
#include <cassert>
#include <iostream>
#include "my_map.h"
using std::cout;
using std::endl;


#include "good_functions.h"
#include "super_define.h"
#include "instructions.h"
#include "dump.h"
#include "my_file_lib.h"
#include "DSL.h"

const int MAX_COMMAND_NAME_SIZE = 20;

/*!
 * main function of disassembler
 *
 * @param[in] buffer code for disassembling
 * @param[in] buffer_size size of buffer
 * @param[out] r_data result of disassembling
 * @return size of data
 */

size_t disassembling(const char* name_of_output_file, char* buffer, int buffer_size);

/*!
 * Check code for all labels and put pointers to labels_pointers array
 *
 * @param[in] buffer
 * @param[in] buffer_end
 * @param[end] labels_pointers
 * @return size of labels_pointers
 */

int get_labels(char* buffer, char* buffer_end, int labels_pointers[]);

int main(int argc, char *argv[]) {
#include "instructions_iniz.h"

    char name_of_instr_file [FILENAME_MAX + 1] = {};
    char name_of_input_file [FILENAME_MAX + 1] = {};
    char name_of_output_file[FILENAME_MAX + 1] = {};
    strcpy(name_of_input_file,  X_("cpu_code.bin"));
    strcpy(name_of_output_file, X_("disasm_output.txt"));

    if (argc == 2) {
        strcpy(name_of_output_file, argv[1]);
    }

    char* buffer = nullptr;
    size_t buffer_size = get_buffer(name_of_input_file, &buffer);
    if (buffer == nullptr) {
        dump(DUMP_INFO, "disasm: Error opening input file");
        return 1;
    }

    disassembling(name_of_output_file, buffer, buffer_size);

    return 0;
}

size_t disassembling(const char* name_of_output_file, char* buffer, int buffer_size) {
    if (buffer == nullptr) {
        dump(DUMP_INFO, "disasm: buffer pointer is null");
        return 0;
    }

    FILE* out = fopen(name_of_output_file, "w");

    char* buff = buffer;
    char* buffer_end = buffer + buffer_size;

    buff += sizeof(Signature);

    int labels_pointers[100] = {};

    std::clock_t timer = std::clock();

    int labels_size = get_labels(buff, buffer_end, labels_pointers);

#define DEF_CMD(name, num, next, cpu_code, disasm_code) \
    case num: PRINT_STR(name); disasm_code; buff += next; PRINT_N; break;

    while (buff < buffer_end) {

        for (int i = 0; i < labels_size; i++)
            if (buff == buffer + labels_pointers[i]) {
                PRINT_CS(label_);
                PRINT_VAR(i + 1);
                PRINT_CS(:\n);
            }

        switch (*buff) {

            #include "commands.h"

            default:
                printf("disasm: Unknown command!\n");
                return 2;
        }
    }

#undef DEF_CMD

    printf("Running time: %lf sec.\n", (double)(std::clock() - timer) / CLOCKS_PER_SEC);

    secure_free(&buffer);
    fclose(out);

    printf("disasm is OK\n\n");

    return 0;
}

int get_labels(char* buffer, char* buffer_end, int labels_pointers[]) {
    int labels_pos = 0;
    buffer_end--;
    for (; buffer < buffer_end; buffer++)
        if (49 <= *buffer && *buffer <= 56) {
            int not_used = 1;
            for (int i = 0; i <= labels_pos; i++) {
                if (labels_pointers[i] == *((int*)(buffer + 1)))
                    not_used = 0;
            }
            if (not_used)
                labels_pointers[labels_pos++] = *((int*)(buffer + 1));
        }
    return labels_pos;
}
