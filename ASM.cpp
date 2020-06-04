#include <cstdio>
#include <cassert>
#include <iostream>
#include <string>
#include "my_map.h"
using std::cout;
using std::endl;

#include "good_functions.h"
#include "super_define.h"
#include "instructions.h"
#include "dump.h"
#include "my_file_lib.h"

const int MAX_COMMAND_NAME_SIZE = 40;
const int MAX_REG_NAME_SIZE = 5;
const int MAX_NUMBER_SIZE = 10;

const char VERSION[4] = "2.1";

bool second_assembling = false;
bool second_assembling_going = false;

/*!
 *
 * @param[in] argc from main
 * @param[in] argv from main
 * @param[out] name_of_instr_file instruction file name
 * @param[out] name_of_input_file input file name
 * @param[out] name_of_output_file output file name
 * @return  0 to show help
 * @return  1 invalid number of arguments
 * @return -1 file names were received successfully
 */

int get_arg(int argc, char *argv[], char* name_of_instr_file, char* name_of_input_file, char* name_of_output_file);

/*!
 * assembling code to CPU
 *
 * @param[in] buffer buffer with source code
 * @param[in] buffer_size size of buffer
 * @param[out] r_data pointer on data
 * @return size of data
 */

size_t assembling(char* buffer, size_t buffer_size, char* *r_data);

/*!
 *
 * "intructions_file" "input_file" "output_file"
 * if flag is -- read name of file from console
 *
 * @param[in] argc
 * @param[in] argv
 * @return 0 success
 * @return 1 invalid number of arguments
 * @return 2 error opening instruction file
 * @return 3 error opening input file
 */

int main(int argc, char *argv[]) {
#include "instructions_iniz.h"

    char name_of_instr_file [FILENAME_MAX + 1] = {};
    char name_of_input_file [FILENAME_MAX + 1] = {};
    char name_of_output_file[FILENAME_MAX + 1] = {};
    strcpy(name_of_input_file,  X_("input.txt"));
    strcpy(name_of_output_file, X_("cpu_code.bin"));

    if (argc == 2) {
        strcpy(name_of_input_file, argv[1]);
    }

    char* buffer = nullptr;
    size_t buffer_size = get_buffer(name_of_input_file, &buffer);
    if (buffer == nullptr) {
        dump(DUMP_INFO, "asm: Error opening input file");
        return 3;
    }

    printf("\nGo ASM\n");

    std::clock_t timer = std::clock();

    char* data = nullptr;
    size_t data_size = assembling(buffer, buffer_size, &data);

    if (second_assembling) {
        secure_free(&data);
        second_assembling_going = true;
        assembling(buffer, buffer_size, &data);
    }

    secure_free(&buffer);

    put_data_in_file(name_of_output_file, data, data_size);

    printf("\nRunning time: %lf sec.\n", (double)(std::clock() - timer) / CLOCKS_PER_SEC);

    printf("asm is OK\n");

    return 0;
}

int get_arg(int argc, char *argv[], char* name_of_instr_file, char* name_of_input_file, char* name_of_output_file) {
    if (argc >= 2 && (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("Assembler programm (v1.0)\n"
               "1 flag is name of instruction file\n"
               "2 flag is name of input file\n"
               "3 flag is name of output file\n"
               "use -- flag if you want to write name of file in console\n");
        return 0;
    }
    if (argc < 2 || argc > 4 || (argc == 2 && strcmp(argv[1], "--") != 0)
                                || (argc == 3 && strcmp(argv[2], "--") != 0)) {
        printf("invalid number of arguments\n");
        return 1;
    }
    if (!strcmp(argv[1], "--")) {
        printf("Enter name of instruction file: ");
        scanf("%s", name_of_instr_file);
    } else {
        strcpy(name_of_instr_file, argv[1]);
    }
    if ((argc < 3 && !strcmp(argv[1], "--")) || !strcmp(argv[2], "--")) {
        printf("Enter name of input file: ");
        scanf("%s", name_of_input_file);
    } else {
        strcpy(name_of_input_file, argv[2]);
    }
    if ((argc < 3 && !strcmp(argv[1], "--"))  || (argc < 4 && !strcmp(argv[2], "--")) || !strcmp(argv[3], "--")) {
        printf("Enter name of output file: ");
        scanf("%s", name_of_output_file);
    } else {
        strcpy(name_of_output_file, argv[3]);
    }
    return -1;
}

size_t assembling(char* buffer, size_t buffer_size, char* *r_data) {
    if (buffer == nullptr) {
        dump(DUMP_INFO, "buffer pointer is null");
        *r_data = nullptr;
        return 0;
    }

    char* data = (char*)calloc(100 * buffer_size, sizeof(char));
    if (data == nullptr) {
        dump(DUMP_INFO, "callock did not allocate memory");
        *r_data = nullptr;
        return 0;
    }
    *r_data = data;

    char* buffer_beg = buffer;
    char* buffer_end = buffer + buffer_size;
    char  name_of_command [MAX_COMMAND_NAME_SIZE + 1] = {};
    char  name_of_label   [MAX_COMMAND_NAME_SIZE + 1] = {};
    char  name_of_register[MAX_REG_NAME_SIZE + 1] = {};
    int x = 0;
    int n = 0;

    *((Signature*)data) = Signature(VERSION, buffer_size);

    data += sizeof(Signature);

    for (;buffer_beg < buffer_end; ++buffer_beg) {
        if (*buffer_beg == '\n' || *buffer_beg == '\t' || *buffer_beg == ' ')
            continue;

        sscanf(buffer_beg, "%[a-z_0-9<>=!]%n", name_of_command, &n);
        buffer_beg += n;
//        w(name_of_command);

        if (*buffer_beg == ':') {
            labels[name_of_command] = data - *r_data;
            continue;
        }

        if (instructs[name_of_command] == 0) {
            dump(DUMP_INFO, "Unknown command!");
            printf("%s\n", name_of_command);
            assert(ERROR);
        }

        *data = (char)instructs[name_of_command];

        if (*data == 1) {
            sscanf(buffer_beg, "%d%n", &x, &n);
            buffer_beg += n;
            *((int *) (data + sizeof(char))) = x * 100;
            data += sizeof(int);
        }

        if (*data == 15 || *data == 16 || *data == 17 || *data == 18) {

            x = -1;
            *name_of_register = '\0';

            sscanf(buffer_beg, " [%[a-z] + %d]%n", name_of_register, &x, &n);
            if (x == -1 || *name_of_register == '\0') {
                sscanf(buffer_beg, " [%d + %[a-z]]%n", &x, name_of_register, &n);
                if (x == -1 || *name_of_register == '\0') {
                    sscanf(buffer_beg, " [%d]%n", &x, &n);
                    if (x == -1) {
                        sscanf(buffer_beg, " [ %[a-z]]%n", name_of_register, &n);
                        if (*name_of_register == '\0') {
                            dump(DUMP_INFO, "Failed _RAM inctructions!");
                            assert(ERROR);
                        }
                    }
                }
            }

            if (*name_of_register != '\0' && find_registers[name_of_register] == 0) {
                dump(DUMP_INFO, "Failed name of register!");
                assert(ERROR);
            }

            buffer_beg += n;
            data += sizeof(char);
            *data = (char)find_registers[name_of_register];

            *((int*)(data + sizeof(char))) = x;
            data += sizeof(int);
        }

//        if (*data == 17 || *data == 18) {
//            sscanf(buffer_beg, " [%d]%n", &x, &n);
//            buffer_beg += n;
//            *((int*)(data + sizeof(char))) = x;
//            data += sizeof(int);
//        }

        if (*data == 10 || *data == 11 || *data == 21) {
            sscanf(buffer_beg + 1, " %[a-z0-9]%n", name_of_register, &n);
            buffer_beg += n + 1;
            data += sizeof(char);
            if (find_registers[name_of_register] == 0) {
                dump(DUMP_INFO, "Failed name of register!");
                assert(ERROR);
            }
            *data = (char)find_registers[name_of_register];
        }

        if (*data == 20) {
            sscanf(buffer_beg, "%d%n", &x, &n);
            buffer_beg += n;
            *((int*)(data + sizeof(char))) = x;
            data += sizeof(int);
        }

        if (49 <= *data && *data <= 56) {
            sscanf(buffer_beg + 1, "%[a-z_0-9<>=!]%n", name_of_label, &n);
            buffer_beg += n + 1;
            if (labels[name_of_label] == 0) {
                if (second_assembling_going) {
                    dump(DUMP_INFO, "Faild labels!");
                    printf("Name of failed label: %s\n", name_of_label);
                    assert(ERROR);
                }
                second_assembling = true;
            } else {
                *((int*)(data + sizeof(char))) = labels[name_of_label];
            }
            data += sizeof(int);
        }

        data += sizeof(char);
    }

    return data - *r_data;
}