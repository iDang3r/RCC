//
// Created by Александр on 02.12.2019.
//

#ifndef RCC_FRONTEND_FRONTEND_H
#define RCC_FRONTEND_FRONTEND_H

#include "dump.h"
#include "Tree.h"
#include "my_map.h"

const char open_bracket  = '{';
const char close_bracket = '}';
const char nil_node      = '"';

namespace type {
    enum {
        num  = 0,
        var  = 1,
        func = 2,
        serv = 3,
    };
}

#define ADD(l, r) create(type::func, '+', l, r)
#define SUB(l, r) create(type::func, '-', l, r)
#define MUL(l, r) create(type::func, '*', l, r)
#define DIV(l, r) create(type::func, '/', l, r)
#define SQRT(r)   create(type::func, 'T', r, nullptr)
#define VAR(x)    create(type::var, x)
#define NUM(x)    create(type::num, x);
#define INI(l, r) create(type::func, '~', l, r)
#define PNT(l, r) create(type::func, '.', l, r)
#define RET(x)    create(type::func, '@', nullptr, x)
#define IF(l, r)  create(type::func, 'I', l, r)
#define WH(l, r)  create(type::func, 'W', l, r)
#define B(l, r)   create(type::var, 'B', l, r)
#define IN(x)     create(type::func, '>', nullptr, x);
#define OUT(x)    create(type::func, '<', nullptr, x);

union uni_data {
    int num_;
    char name_;

    uni_data(int num) : num_(num) {}
    uni_data(char name) : name_(name) {}
};

class Frontend : Tree<uni_data> {
public:

    FILE* in  = nullptr;
    FILE* out = nullptr;

    wchar_t* data_[1024]{};
    const wchar_t* s = nullptr;
    const wchar_t* s1 = nullptr;
    int data_begin_ = 0;
    int data_size_ = 0;
    int all_symbs_counter = 0;
    int not_space_counter_ = 0;

    int l_i = 0;
    int l_j = 0;
    int i = 0;
    int j = 0;

    bool is_func['z' + 1]{};

    node<uni_data>* during = nullptr;

    // Methods

    Frontend(const char* input_file_name, const char* output_file_name) {
        in  = fopen(input_file_name,  "r");
        out = fopen(output_file_name, "w");

        if (in == nullptr) {
            throw "input file was not opened";
        }

        if (out == nullptr) {
            throw "output file was not opened";
        }

        wchar_t buff[256]{};

        while (fscanf(in, "%l[^L\n]", buff) != EOF) {
            if (buff[0] != L'\n') {
                int buff_size = wcslen(buff);
                for (int i = buff_size - 1; i > 0 && buff[i] == L' '; i--)
                    buff[i] = L'\0';
                data_[data_size_] = wcsdup(buff);
                all_symbs_counter += buff_size;
                data_size_++;
            }
            if (buff[0] == L'>')
                data_begin_ = data_size_;
            getwc(in);
        }

        for (int i = data_begin_; i < data_size_; i++)
            wprintf(L"%ls\n", data_[i]);

        cout << endl;

        push_head(L'B');
        during = head_;
    }

    ~Frontend() {
        for (int i_ = 0; i_ < data_size_; i_++)
            free(data_[i_]);
        fclose(in);
        fclose(out);
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

            if (el->type_ == type::var || el->type_ == type::func || el->type_ == type::serv) {
                fprintf(out_, "%c\n", el->data_.name_);
            } else {
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

        FILE* out_ = fopen("/Users/alex/Desktop/pictures/Frontend.pu", "w");

        if (out_ == nullptr) {
            dump(DUMP_INFO, "file was not opened");
            return;
        }

        fprintf(out_, "@startuml\n !define DARKORANGE\n !include style.puml\n class head_\n");

        fprintf(out_, "head_ -down-> %p\n", head_);

        dump_picture_rec(out_, head_);

        fprintf(out_, "@enduml\n");
        fclose(out_);

        system("java -jar --illegal-access=warn /Users/alex/plantuml.jar -tsvg /Users/alex/Desktop/pictures/Frontend.pu");
        system("open /Users/alex/Desktop/pictures/Frontend.svg");

    }

    void put_tree_rec(node<uni_data>* el) {
        assert(out);
        assert(el);

        fprintf(out, "%c", open_bracket);

        if (el->type_ == type::var || el->type_ == type::func) {
            fprintf(out, "%c", el->data_.name_);
        } else if (el->type_ == type::num) {
            fprintf(out, "%d", el->data_.num_);
        } else {
            dump(DUMP_INFO, "unknown type");
        }

        if (el->left_ != nullptr || el->right_ != nullptr) {
            if (el->left_ != nullptr) {
                put_tree_rec(el->left_);
            } else {
                fprintf(out, "%c", nil_node);
            }

            if (el->right_ != nullptr) {
                put_tree_rec(el->right_);
            } else {
                fprintf(out, "%c", nil_node);
            }
        }

        fprintf(out, "%c", close_bracket);
    }

    void go_on() {

        switch (data_[i][j]) {
            case L'═':

                if (l_j < j) {
                    l_j++;
                    j++;
                } else {
                    l_j--;
                    j--;
                }

                break;

            case L'║':

                if (l_i < i) {
                    l_i++;
                    i++;
                } else {
                    l_i--;
                    i--;
                }

                break;

            case L'╬':

                if (l_j < j) {
                    l_j++;
                    j++;
                } else if (l_j > j) {
                    l_j--;
                    j--;
                } else if (l_i < i) {
                    l_i++;
                    i++;
                } else {
                    l_i--;
                    i--;
                }

                break;

            case L'╔':

                if (l_i > i) {
                    l_i--;
                    j++;
                } else {
                    l_j--;
                    i++;
                }

                break;

            case L'╗':

                if (l_j < j) {
                    l_j++;
                    i++;
                } else {
                    l_i--;
                    j--;
                }

                break;

            case L'╚':

                if (l_i < i) {
                    l_i++;
                    j++;
                } else {
                    l_j--;
                    i--;
                }

                break;

            case L'╝':

                if (l_j < j) {
                    l_j++;
                    i--;
                } else {
                    l_i++;
                    j--;
                }

                break;

            default:
                wprintf(L"go_on stoped at: %lc\n", data_[i][j]);
                return;
        }
        go_on();

    }

    int put_tree_in_file() {

        if (head_ != nullptr)
            put_tree_rec(head_);
        else
            fprintf(out, "%c", nil_node);

        printf("Tree was rewriten\n");
        return 0;
    }

    bool is_open_tube(int i_, int j_) {

        switch (data_[i_][j_]) {
            case L'═':break;
            case L'║':break;
            case L'╔':break;
            case L'╗':break;
            case L'╚':break;
            case L'╝':break;
            case L'╬':break;
            default:
                return false;
        }
        return true;
    }

    node<uni_data>* create(int type, uni_data val, node<uni_data>* left = nullptr, node<uni_data>* right = nullptr) {
        return new node<uni_data>(type, val, left, right);
    }

    node<uni_data>* params_to_tree() {
        while (*s == L' ' || *s == L',')
            s++;

        node<uni_data>* res = make_node(',');

        if (isalpha(char(*s))) {
            push_left(res, char(*s));
            s++;
            res->right_ = params_to_tree();
        }

        return res;
    }

    int conv_const_rec(node<uni_data>* el) {
        assert(el);

        if (el->type_ == type::func) {

            node<uni_data>* tmp = nullptr;

            switch (el->data_.name_) {

                case '+':

                    if (el->left_->type_ == type::num && el->left_->data_.num_ == 0) {

                        delete el->left_;

                        tmp = el->right_;

                        *el = *(el->right_);

                        delete tmp;
                        return 1;
                    }

                    if (el->right_->type_ == type::num && el->right_->data_.num_ == 0) {

                        delete el->right_;

                        tmp = el->left_;

                        *el = *(el->left_);

                        delete tmp;
                        return 1;
                    }

                    break;

                case '-':

                    if (el->left_->type_ == type::num && el->left_->data_.num_ == 0) {

                        delete el->left_;

                        tmp = el->right_;

                        *el = *(el->right_);

                        delete tmp;
                        return 1;
                    }

                    break;

                case '*':

                    if ((el->left_->type_ == type::num && el->left_->data_.num_ == 0) ||
                            (el->right_->type_ == type::num && el->right_->data_.num_ == 0)) {

                        free_tree(el->left_);
                        free_tree(el->right_);

                        el->type_ = type::num;
                        el->data_.num_ = 0;

                        return 1;
                    }

                    break;

                case '/':

                    if (el->left_->type_ == type::num && el->left_->data_.num_ == 0) {

                        free_tree(el->left_);
                        free_tree(el->right_);

                        el->type_ = type::num;
                        el->data_.num_ = 0;

                        return 1;
                    }

                    if (el->right_->type_ == type::num && el->right_->data_.num_ == 1) {

                        delete el->right_;

                        tmp = el->left_;

                        *el = *(el->left_);

                        delete tmp;
                        return 1;
                    }

                    break;

            }

        }

        int cx = 0;

        if (el->left_) {
            while (conv_const_rec(el->left_))
                cx++;
        }

        if (el->right_) {
            while (conv_const_rec(el->right_))
                cx++;
        }

        return cx;
    }

    void conv_const() {
        while (conv_const_rec(head_));
    }

    node<uni_data>* GetN() {
        node<uni_data>* val = nullptr;

        int tmp_value = 0;
        int n = 0;

        swscanf(s, L"%d%n", &tmp_value, &n);
        s += n;

        val = NUM(tmp_value);

        return val;
    }

    node<uni_data>* GetF() {
        node<uni_data>* val = nullptr;

        if (L'a' <= *s && *s <= L'z' && !is_func[(char)(*s)]) {

            cout << "VAR was found: " << (char)(*s) << endl;

            val = VAR((char)(*s));
            s++;

            return val;
        }

        char func = (char)(*s);
        s++;

        cout << "FUNC was found: " << func << endl;

        val = create(type::func, func, nullptr, GetParams_by_points());

        return val;
    }

    node<uni_data>* GetP() {
        node<uni_data>* val = nullptr;

        if (*s == L'(') {
            s++;

            val = GetR();

            assert(*s == L')');
            s++;
            return val;
        }

        if ((L'0' <= *s && *s <= L'9') || *s == L'-') {

            val = GetN();

            return val;
        }

        val = GetF();

        return val;
    }

    node<uni_data>* GetS() {

        node<uni_data>* val = nullptr;

        if (*s == L'√') {
            s++;

            val = SQRT(GetP());

        } else {
            val = GetP();
        }

        return val;
    }

    node<uni_data>* GetT() {
        node<uni_data>* val = nullptr;

        val = GetS();

        while (*s == L'*' || *s == L'/') {
            char op = (char)(*s);
            s++;

            if (op == '*')
                val = MUL(val, GetS());
            else
                val = DIV(val, GetS());

        }
        return val;
    }

    node<uni_data>* GetR() {
        node<uni_data>* val = nullptr;

        val = GetT();

        while (*s == L'+' || *s == L'-') {
            char op = (char)(*s);
            s++;

            if (op == '+')
                val = ADD(val, GetT());
            else
                val = SUB(val, GetT());

        }
        return val;
    }

    node<uni_data>* GetE1() { // May be not used
        node<uni_data>* val = nullptr;

        val = GetR();

        if (*s == L'~') {
            s++;

            val = INI(val, GetR());

        }
        return val;
    }

    node<uni_data>* GetCond() {
        node<uni_data>* val = nullptr;

        val = GetR();

        assert(*s == L'>' || *s == L'<' || *s == L'!' || *s == L'~');

        val = create(type::func, (char)(*(s++)), val, GetR());

        return val;
    }

    node<uni_data>* GetB() {
        node<uni_data>* val = nullptr;

        go_on();

        if (data_[i][j] == L'<' || data_[i][j] == L'>') {

            s = &data_[i][j + 1];

            if (data_[i][j] == L'<') {
                val = OUT(GetR());
            } else {
                val =  IN(GetR());
            }

            l_i = i;
            l_j = j = s - &data_[i][0];

            if (data_[i][j + 1] != L' ' && data_[i][j + 1] != L'\0') {
                j++;
            } else if (i != data_size_ - 1 && wcslen(data_[i + 1]) > j && data_[i + 1][j] != L' ' && data_[i + 1][j] != L'\0') {
                i++;
            } else if (i != 0 && wcslen(data_[i - 1]) > j && data_[i - 1][j] != L' ' && data_[i - 1][j] != L'\0') {
                i--;
            } else {
                dump(DUMP_INFO);
                throw "next trail after if/while was not found";
            }

        } else if (data_[i][j + 1] == L'~') {

            s = &data_[i][j + 2];
            val = INI(VAR((char)data_[i][j]), GetR());

            l_i = i;
            l_j = j = s - &data_[i][0];

            if (data_[i][j + 1] != L' ' && data_[i][j + 1] != L'\0') {
                j++;
            } else if (data_[i + 1][j] != L' ' && data_[i + 1][j] != L'\0') {
                i++;
            } else if (data_[i - 1][j] != L' ' && data_[i - 1][j] != L'\0') {
                i--;
            } else {
                dump(DUMP_INFO);
                throw "next trail after init was not found";
            }

        } else if (data_[i][j] == L'&') {

            bool is_if = true;

            if ((i != l_i || j - 1 != l_j) && is_open_tube(i, j - 1)) {
                is_if = false;
            } else if ((i - 1 != l_i || j != l_j) && is_open_tube(i - 1, j)) {
                is_if = false;
            } else if ((i + 1 != l_i || j != l_j) && is_open_tube(i + 1, j)) {
                is_if = false;
            }

            w(is_if);

            s = &data_[i][j + 1];

            node<uni_data>* cond = GetCond();

            l_i = i;
            l_j = j = s - &data_[i][0];

            if (data_[i][j + 1] != L' ' && data_[i][j + 1] != L'\0') {
                j++;
            } else if (i != data_size_ - 1 && wcslen(data_[i + 1]) > j && data_[i + 1][j] != L' ' && data_[i + 1][j] != L'\0') {
                i++;
            } else if (i != 0 && wcslen(data_[i - 1]) > j && data_[i - 1][j] != L' ' && data_[i - 1][j] != L'\0') {
                i--;
            } else {
                dump(DUMP_INFO);
                throw "next trail after if/while was not found";
            }

            if (is_if) { // IF
                go_on();

                node<uni_data>*  true_branch = nullptr;
                node<uni_data>* false_branch = nullptr;

                int if_i = i;
                int if_j = j;

                switch (data_[i][j]) {
                    case L'╟':

                        if (l_i < i) {

                            l_i++;
                            i++;

                        } else {

                            l_i--;
                            i--;

                        }

                        true_branch = GetB();

                        l_i = if_i;
                        l_j = if_j;
                        i = if_i;
                        j = if_j + 1;

                        false_branch = GetB();

                        break;

                    case L'╢':

                        if (l_i < i) {

                            l_i++;
                            i++;

                        } else {

                            l_i--;
                            i--;

                        }

                        true_branch = GetB();

                        l_i = if_i;
                        l_j = if_j;
                        i = if_i;
                        j = if_j - 1;

                        false_branch = GetB();

                        break;

                    case L'╧':

                        if (l_j < j) {

                            l_j++;
                            j++;

                        } else {

                            l_j--;
                            j--;

                        }

                        true_branch = GetB();

                        l_i = if_i;
                        l_j = if_j;
                        i = if_i - 1;
                        j = if_j;

                        false_branch = GetB();

                        break;

                    case L'╤':

                        if (l_j < j) {

                            l_j++;
                            j++;

                        } else {

                            l_j--;
                            j--;

                        }

                        true_branch = GetB();

                        l_i = if_i;
                        l_j = if_j;
                        i = if_i + 1;
                        j = if_j;

                        false_branch = GetB();

                        break;

                    default:

                        cout << i << " " << j << " <________\n";

                        dump(DUMP_INFO);
                        throw "unknown tube after if";
                }

                val = IF(cond, create(type::func, 'S', true_branch, false_branch));

                switch (data_[i][j]) {
                    case L'╨':

                        l_i = i;
                        l_j = j;

                        i--;

                        break;

                    case L'╡':

                        l_i = i;
                        l_j = j;

                        j--;

                        break;

                    case L'╞':

                        l_i = i;
                        l_j = j;

                        j++;

                        break;

                    case L'╥':

                        l_i = i;
                        l_j = j;

                        i++;

                        break;
                }

            } else { // WHILE

                node<uni_data>* branch = GetB();

                assert(data_[i][j] == L'#');

                l_i = i;
                l_j = j;

                if (data_[i][j + 1] != L' ' && data_[i][j + 1] != L'\0') {

                    j++;

                    switch (data_[i][j]) {

                        case L'═':break;
                        case L'╗':break;
                        case L'╝':break;
                        default: j--;

                    }

                } else if (i != data_size_ - 1 && wcslen(data_[i + 1]) > j && data_[i + 1][j] != L' ' && data_[i + 1][j] != L'\0') {

                    i++;

                    switch (data_[i][j]) {

                        case L'║':break;
                        case L'╝':break;
                        case L'╚':break;
                        default: i--;

                    }

                } else if (i != 0 && wcslen(data_[i - 1]) > j && data_[i - 1][j] != L' ' && data_[i - 1][j] != L'\0') {

                    i--;

                    switch (data_[i][j]) {

                        case L'║':break;
                        case L'╔':break;
                        case L'╗':break;
                        default: i++;

                    }

                }

                val = WH(cond, branch);

            }

        } else if (data_[i][j] == L'@') {
            s = &data_[i][j + 1];

            if (*s != L' ' && *s != L'\0')
                val = B(RET(GetR()), nullptr);

            l_i = i;
            l_j = j = s - &data_[i][0];
            return val;
        }

        if (val != nullptr) {
            val = B(val, GetB());
        }

        return val;
    }

    node<uni_data>* GetParams_by_points() {
        node<uni_data>* val = nullptr;

        val = PNT(GetR(), nullptr);

        if (*s == L'.') {
            s++;
            val->right_ = GetParams_by_points();
        }

        return val;
    }

    node<uni_data>* find_function_to_tree(char func) {

        bool u = true;

        for (int i_ = data_begin_; i_ < data_size_ && u; i_++) {
            for (int j_ = 0; j_ < wcslen(data_[i_]) && u; j_++) {
                if (data_[i_][j_] == (wchar_t)func && (j_ == 0 || data_[i_][j_ - 1] == L' ')) {

                    i = i_;
                    j = j_;

                    u = false;
                }
            }
        }

        while (iswalpha(data_[i][j + 1]) || data_[i][j + 1] == L'.') {
            j++;
        }

        l_i = i;
        l_j = j;

        if (data_[i][j + 1] != L' ' && data_[i][j + 1] != L'\0') {
            j++;
        } else if (i != data_size_ - 1 && wcslen(data_[i + 1]) > j && data_[i + 1][j] != L' ' && data_[i + 1][j] != L'\0') {
            i++;
        } else if (i != 0 && wcslen(data_[i - 1]) > j && data_[i - 1][j] != L' ' && data_[i - 1][j] != L'\0') {
            i--;
        } else {
            cout << func << endl;
            dump(DUMP_INFO);
            throw "next trail after function was not found";
        }

        return GetB();
    }

    void vars_funcs_parsing() {
        int pos = 0;
        int vars = 0, funcs = 0;
        if (wcsncmp(data_[pos], L"vars:", 5) == 0) {
            for (int i = 5; i < wcslen(data_[pos]); i++) {
                if (data_[pos][i] == L';')
                    break;

                if (data_[pos][i] == L' ' || data_[pos][i] == L',')
                    continue;

                push_left(during, L'V');
                push_right(during->left_, data_[pos][i]);
                push_right(during, L'B');
                during = during->right_;
            }
            vars = 1;
            pos++;
        }
        if (wcsncmp(data_[pos], L"funcs:", 6) == 0) {
            for (int i = 6; i < wcslen(data_[pos]); i++) {
                if (data_[pos][i] == L';')
                    break;
                if (data_[pos][i] == L' ' || data_[pos][i] == L',')
                    continue;

                char func = (char)data_[pos][i];
                if (is_func[func]) {
                    throw "repeated name of function!";
                } else {
                    is_func[func] = true;
                }
                i++;

                s = &data_[pos][i];
                if (*s != L'(') {
                    throw "failed function open scope '('";
                }
                s++;

                push_left(during, 'F');
                push_right(during, 'B');

                push_right(during->left_, func);

                cout << "DECLARATION " << func << endl;

                during->left_->right_->left_ = params_to_tree();
                s1 = s;
                during->left_->right_->right_ = find_function_to_tree(func);
                s = s1;

                during = during->right_;

                if (*s != L')') {
                    cout << (char)*s << endl;
                    throw "failed function close scope ')'";
                }
                s++;
                i = s - data_[pos];
            }
            funcs = 1;
            pos++;
        }

        if (data_[pos][0] != L'>') {
            throw "failed vars or funcs init";
        }

        during->data_ = '$';
        during->right_ = find_function_to_tree('$');

    }

};


#endif //RCC_FRONTEND_FRONTEND_H
