#include "minitest.h"
#include "../src/ui.h"

typedef struct{
    const char *name;
    Tree in;
    int delta;
    int upto;
    Tree expected;
}AddTestCase;

void add(ptr_growable_array*  sorted_growables,int delta, int up_to);

void test_ui_add_case(void(*init_test)(AddTestCase* tc)){
    AddTestCase tc = {0};
    init_test(&tc);
    add(&tc.in.sorted_growables,tc.delta,tc.upto);
    for(int i=0;i<tc.in.sorted_growables.len;i++){
        if(tc.in.sorted_growables.data[i]->id!=tc.in.sorted_growables.data[i]->id){
           TEST_ERRORF(tc.name,"failed sorted_growables id dont match exp %d got %d\n",
               tc.expected.sorted_growables.data[i]->id ,
               tc.in.sorted_growables.data[i]->id
           );
        }
    }
}

void test_add_sorted_just_init(AddTestCase* tc){
    tc->name = __func__;

}

void test_ui_add(){
    void(*cases[])(AddTestCase* tc) = {
        test_add_sorted_just_init,
    };

    int test_count = sizeof(cases) / sizeof(cases[0]);
    for (int i = 0; i < test_count; i++) {
        mt_total++;
        test_ui_add_case(cases[i]);
    }
}
