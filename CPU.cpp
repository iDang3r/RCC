#include <cstdio>
#include <cassert>
#include <iostream>
#include <cmath>
#include "ctime"

using std::cout;
using std::endl;
using std::max;
using std::min;

#define DEBUG

#include "my_stack.h"
#include "dump.h"
#include "my_file_lib.h"
#include "super_define.h"
#include "instructions.h"

const char VERSION[4] = "2.1";

//registers
int  registers[15] = {};
char image_[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH] = {};
int  RAM_  [MAX_RAM_SIZE] = {};
char VRAM_ [MAX_VRAM_SIZE] = {};

/*!
 * puts in VideoRAM circle [2*r + 1, 2*r + 1]
 *
 * @param[in] r radius if circle
 */

void make_circle(int r);

/*!
 * Prints image [n, m]
 *
 * @param[in] n height of image
 * @param[in] m width of image
 */

void print_image(int n, int m);

/*!
 * main function of CPU
 *
 * @param[in] code input code to run
 * @param[int] code_size size of code
 * @return code of error
 */

int processing(char* code, size_t code_size);


int main(int argc, char *argv[]) {

    char name_of_input_file[FILENAME_MAX + 1] = {};  //"/Users/alex/Desktop/~X/cpu_code.bin";
    strcpy(name_of_input_file,  X_("cpu_code.bin"));

    char* code = nullptr;
    size_t code_size = get_buffer(name_of_input_file, &code);
    if (code == nullptr) {
        dump(DUMP_INFO, "CPU: Error opening input file");
        return 3;
    }

    return processing(code, code_size);
}

int processing(char* code, size_t code_size) {
    if (code == nullptr) {
        dump(DUMP_INFO, "cpu: code == nullptr");
        return 1;
    }

    char* code_to_free = code;

    ((Signature*)code)->dump();

    if (strcmp(((Signature*)code)->version_, VERSION) != 0) {
        dump(DUMP_INFO, "Versions of asmed code and cpu are not equal!\n");
        return 5;
    }

    code += sizeof(Signature);

    Stack(St);
    Stack(Func_calls);

    printf("\033[1;32mProgram begin\033[0m\n");

    std::clock_t timer = std::clock();

#define DEF_CMD(name, num, next, cpu_code, disasm_code) \
    case num: cpu_code; code += next; break;

    while (code_to_free + code_size > code) {
//        St.dump(DUMP_INFO);
//        Func_calls.dump(DUMP_INFO);
        switch (*code) {

            // AUTO_GENERATED SWITCH

            #include "commands.h"

            default:
                printf("CPU: Unknown command!\n");
                return 2;
        }
    }
#undef DEF_CMD

    printf("\033[1;32mProgram end\033[0m\n");

    secure_free(&code_to_free);

    printf("\nRunning time: %lf sec.\n", (double)(std::clock() - timer) / CLOCKS_PER_SEC);
    printf("CPU is OK\n\n");

    return 0;
}

void make_circle(int r) {
    assert(0 < r && 2 * r <= MAX_IMAGE_HEIGHT);
    for (int i = 0; i <= r; i++) {
        for (int j = 0; j <= r; j++) {
            if (pow(r - i, 2) + pow(r - j, 2) <= pow(r, 2))
                image_[i][j] = CYAN;
            else
                image_[i][j] = RED;
        }
    }

    for (int i = r; i <= 2 * r; i++) {
        for (int j = 0; j <= r; j++) {
            image_[i][j] = image_[2 * r - i][j];
        }
    }
    for (int i = 0; i <= 2 * r; i++) {
        for (int j = r; j <= 2 * r; j++) {
            image_[i][j] = image_[i][2 * r - j];
        }
    }
}

void print_image(int n, int m) {
    assert(0 < n && n <= MAX_IMAGE_HEIGHT);
    assert(0 < m && m <= MAX_IMAGE_WIDTH);

    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("\033[%dm  \033[0m", image_[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
