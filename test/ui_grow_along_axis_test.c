#include "minitest.h"
#include "../src/ui.h"

typedef struct{
    const char *name;
    Tree in;
    int remaining;
    ARRAY(Growable) expected;
}GrowAlongAxisTestCase;


#define MAX_GROWABLE 10

void grow_along_axis(Tree* tree,int remaining);

void test_ui_grow_along_axis_case(void(*init_test)(GrowAlongAxisTestCase* tc)){
    GrowAlongAxisTestCase tc = {0};
    STATIC_ZERO_INIT(Growable, tc.in.growables,in_grow,MAX_GROWABLE);
    STATIC_ZERO_INIT(ptr_growable, tc.in.sorted_growables,in_ptr_grow,MAX_GROWABLE);
    STATIC_ZERO_INIT(Growable, tc.expected,exp_grow,MAX_GROWABLE);
    init_test(&tc);

    grow_along_axis(&tc.in,tc.remaining);

    if(tc.in.growables.len!=tc.expected.len){
       TEST_ERRORF(tc.name,"failed growables len dont match exp %d got %d\n",
           tc.expected.len ,
           tc.in.growables.len
       );
    }
    for(int i=0;i<tc.expected.len;i++){
        if(tc.in.growables.data[i].id!=tc.expected.data[i].id){
           TEST_ERRORF(tc.name,"failed growables[%d] id dont match exp %d got %d\n",
               i,
               tc.expected.data[i].id ,
               tc.in.growables.data[i].id
           );
        }
        if(tc.in.growables.data[i].val!=tc.expected.data[i].val){
           TEST_ERRORF(tc.name,"failed growables[%d] val dont match exp %d got %d\n",
               i,
               tc.expected.data[i].val ,
               tc.in.growables.data[i].val
           );
        }
        if(tc.in.growables.data[i].min!=tc.expected.data[i].min){
           TEST_ERRORF(tc.name,"failed growables[%d] min dont match exp %d got %d\n",
               i,
               tc.expected.data[i].min ,
               tc.in.growables.data[i].min
           );
        }
        if(tc.in.growables.data[i].max!=tc.expected.data[i].max){
           TEST_ERRORF(tc.name,"failed growables[%d] max dont match exp %d got %d\n",
               i,
               tc.expected.data[i].max ,
               tc.in.growables.data[i].max
           );
        }
        if(tc.in.growables.data[i].to_remove!=tc.expected.data[i].to_remove){
           TEST_ERRORF(tc.name,"failed growables[%d] to_remove dont match exp %d got %d\n",
               i,
               tc.expected.data[i].to_remove,
               tc.in.growables.data[i].to_remove
           );
        }
    }
}

void test_ui_grow_along_axis_unsorted_just_init(GrowAlongAxisTestCase* tc){
    tc->name = __func__;
    tc->in.growables.len=3;
    tc->in.growables.data[0]=(Growable){.id=0,.val= 0,.min=0,.max=0,.to_remove=0};
    tc->in.growables.data[1]=(Growable){.id=1,.val=20,.min=0,.max=0,.to_remove=0};
    tc->in.growables.data[2]=(Growable){.id=2,.val=10,.min=0,.max=0,.to_remove=0};
    tc->expected.len=3;
    tc->expected.data[0]=(Growable){.id=0,.val= 0,.min=0,.max=0,.to_remove=0};
    tc->expected.data[1]=(Growable){.id=1,.val=20,.min=0,.max=0,.to_remove=0};
    tc->expected.data[2]=(Growable){.id=2,.val=10,.min=0,.max=0,.to_remove=0};
}


void test_ui_grow_along_axis(){
    void(*cases[])(GrowAlongAxisTestCase* tc) = {
        test_ui_grow_along_axis_unsorted_just_init,
    };

    int test_count = sizeof(cases) / sizeof(cases[0]);
    for (int i = 0; i < test_count; i++) {
        mt_total++;
        test_ui_grow_along_axis_case(cases[i]);
    }
}
