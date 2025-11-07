#include "minitest.h"
#include "../src/ui.h"

typedef struct{
    const char *name;
    Tree in;
    int delta;
    int upto;
    Tree expected;
}AddTestCase;


#define MAX_GROWABLE 10

void add(ptr_growable_array*  sorted_growables,int delta, int up_to);
void build_sorted_growable(Tree *tree);

void test_ui_add_case(void(*init_test)(AddTestCase* tc)){
    AddTestCase tc = {0};
    STATIC_ZERO_INIT(Growable, tc.in.growables,in_grow,MAX_GROWABLE);
    STATIC_ZERO_INIT(ptr_growable, tc.in.sorted_growables,in_ptr_grow,MAX_GROWABLE);
    STATIC_ZERO_INIT(Growable, tc.expected.growables,exp_grow,MAX_GROWABLE);
    STATIC_ZERO_INIT(ptr_growable, tc.expected.sorted_growables,exp_ptr_grow,MAX_GROWABLE);
    init_test(&tc);
    build_sorted_growable(&tc.in);
    build_sorted_growable(&tc.expected);

    add(&tc.in.sorted_growables,tc.delta,tc.upto);

    if(tc.in.sorted_growables.len!=tc.expected.sorted_growables.len){
       TEST_ERRORF(tc.name,"failed sorted_growables len dont match exp %d got %d\n",
           tc.expected.sorted_growables.len ,
           tc.in.sorted_growables.len
       );
    }
    for(int i=0;i<tc.expected.sorted_growables.len;i++){
        if(tc.in.sorted_growables.data[i]->id!=tc.expected.sorted_growables.data[i]->id){
           TEST_ERRORF(tc.name,"failed sorted_growables[%d] id dont match exp %d got %d\n",
               i,
               tc.expected.sorted_growables.data[i]->id ,
               tc.in.sorted_growables.data[i]->id
           );
        }
        if(tc.in.sorted_growables.data[i]->val!=tc.expected.sorted_growables.data[i]->val){
           TEST_ERRORF(tc.name,"failed sorted_growables[%d] val dont match exp %d got %d\n",
               i,
               tc.expected.sorted_growables.data[i]->val ,
               tc.in.sorted_growables.data[i]->val
           );
        }
        if(tc.in.sorted_growables.data[i]->min!=tc.expected.sorted_growables.data[i]->min){
           TEST_ERRORF(tc.name,"failed sorted_growables[%d] min dont match exp %d got %d\n",
               i,
               tc.expected.sorted_growables.data[i]->min ,
               tc.in.sorted_growables.data[i]->min
           );
        }
        if(tc.in.sorted_growables.data[i]->max!=tc.expected.sorted_growables.data[i]->max){
           TEST_ERRORF(tc.name,"failed sorted_growables[%d] max dont match exp %d got %d\n",
               i,
               tc.expected.sorted_growables.data[i]->max ,
               tc.in.sorted_growables.data[i]->max
           );
        }
        if(tc.in.sorted_growables.data[i]->to_remove!=tc.expected.sorted_growables.data[i]->to_remove){
           TEST_ERRORF(tc.name,"failed sorted_growables[%d] to_remove dont match exp %d got %d\n",
               i,
               tc.expected.sorted_growables.data[i]->to_remove,
               tc.in.sorted_growables.data[i]->to_remove
           );
        }
    }
}

void test_add_unsorted_just_init(AddTestCase* tc){
    tc->name = __func__;
    tc->in.growables.len=3;
    tc->in.growables.data[0]=(Growable){.id=0,.val= 0,.min=0,.max=0,.to_remove=0};
    tc->in.growables.data[1]=(Growable){.id=1,.val=20,.min=0,.max=0,.to_remove=0};
    tc->in.growables.data[2]=(Growable){.id=2,.val=10,.min=0,.max=0,.to_remove=0};
    tc->expected.growables.len=3;
    tc->expected.growables.data[0]=(Growable){.id=1,.val=20,.min=0,.max=0,.to_remove=0};
    tc->expected.growables.data[1]=(Growable){.id=2,.val=10,.min=0,.max=0,.to_remove=0};
    tc->expected.growables.data[2]=(Growable){.id=0,.val= 0,.min=0,.max=0,.to_remove=0};
}

void test_add_value(AddTestCase* tc){
    tc->name = __func__;
    tc->in.growables.len=3;
    tc->in.growables.data[0]=(Growable){.id=0,.val= 0,.min=0,.max=0,.to_remove=0};
    tc->in.growables.data[1]=(Growable){.id=1,.val=20,.min=0,.max=0,.to_remove=0};
    tc->in.growables.data[2]=(Growable){.id=2,.val=10,.min=0,.max=0,.to_remove=0};
    tc->delta=10;
    tc->upto=2;
    tc->expected.growables.len=3;
    tc->expected.growables.data[0]=(Growable){.id=1,.val=30,.min=0,.max=0,.to_remove=0};
    tc->expected.growables.data[1]=(Growable){.id=2,.val=20,.min=0,.max=0,.to_remove=0};
    tc->expected.growables.data[2]=(Growable){.id=0,.val=10,.min=0,.max=0,.to_remove=0};
}

void test_add_value_only_on_first(AddTestCase* tc){
    tc->name = __func__;
    tc->in.growables.len=3;
    tc->in.growables.data[0]=(Growable){.id=0,.val= 0,.min=0,.max=0,.to_remove=0};
    tc->in.growables.data[1]=(Growable){.id=1,.val=20,.min=0,.max=0,.to_remove=0};
    tc->in.growables.data[2]=(Growable){.id=2,.val=10,.min=0,.max=0,.to_remove=0};
    tc->delta=10;
    tc->upto=0;
    tc->expected.growables.len=3;
    tc->expected.growables.data[0]=(Growable){.id=1,.val=20,.min=0,.max=0,.to_remove=0};
    tc->expected.growables.data[1]=(Growable){.id=2,.val=10,.min=0,.max=0,.to_remove=0};
    tc->expected.growables.data[2]=(Growable){.id=0,.val=10,.min=0,.max=0,.to_remove=0};
}

void test_add_value_overflow_one(AddTestCase* tc){
    tc->name = __func__;
    tc->in.growables.len=3;
    tc->in.growables.data[0]=(Growable){.id=0,.val= 0,.min=0,.max= 0,.to_remove=0};
    tc->in.growables.data[1]=(Growable){.id=1,.val=20,.min=0,.max=30,.to_remove=0};
    tc->in.growables.data[2]=(Growable){.id=2,.val=10,.min=0,.max= 0,.to_remove=0};
    tc->delta=20;
    tc->upto=2;
    tc->expected.growables.len=2;
    tc->expected.growables.data[0]=(Growable){.id=0,.val=20,.min=0,.max= 0,.to_remove=0};
    tc->expected.growables.data[1]=(Growable){.id=2,.val=30,.min=0,.max= 0,.to_remove=0};
}

void test_ui_add(){
    void(*cases[])(AddTestCase* tc) = {
        test_add_unsorted_just_init,
        test_add_value,
        test_add_value_only_on_first,
        test_add_value_overflow_one,
    };

    int test_count = sizeof(cases) / sizeof(cases[0]);
    for (int i = 0; i < test_count; i++) {
        mt_total++;
        test_ui_add_case(cases[i]);
    }
}
