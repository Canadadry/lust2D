#include "minitest.h"
#include "../src/ui.h"
#include <string.h>

typedef struct{
    const char *name;
    NodeIndex head;
    Tree tree;
    ARRAY(PainterCommand) expected;
}TestCase;

#define MIN(x,y) ((x)<(y)?(x):(y))

bool painter_command_match(const char* test_name,int i,PainterCommand exp, PainterCommand got){
    if (memcmp(&exp, &got, sizeof(PainterCommand))!=0){
        TEST_ERRORF(test_name,
            "failed command %s dont match\ngot\nx:%d y:%d w:%d h:%d\n want\nx:%d y:%d w:%d h:%d\n",
            test_name,i,
            got.x,got.y,got.w,got.h,
            exp.x,exp.y,exp.w,exp.h
        );
        return false;
    }
    return true;
}

void test_ui_compute_case(void(*init_test)(TestCase* tc)){
    TestCase tc = {0};
    init_test(&tc);
    compute(tc.tree,tc.head);
    if(tc.expected.len != tc.tree.commands.len){
        TEST_ERRORF(tc.name,"failed command len exp %d got %d\n",tc.expected.len , tc.tree.commands.len);
    }
    int min_len = MIN(tc.expected.len , tc.tree.commands.len);
    for(int i=0;i<min_len;i++ ){
    	if(painter_command_match(tc.name,i,tc.expected.data[i],tc.tree.commands.data[i])==false) {
            break;
    	}
    }
}

void test_no_children_fixed_position(TestCase* tc){
    tc->name =__func__;
    tc->expected =(ARRAY(PainterCommand)){
        .data =(PainterCommand[]){
            {.x=10,.y=10,.w=100,.h=100,.painter={0}}
        },
        .capacity=1,
        .len=1,
    };
    tc->tree=(Tree){
        .commands=(ARRAY(PainterCommand)){
            .data=(PainterCommand[]){{0}},
            .capacity=1,
            .len=0,
        },
        .nodes=(ARRAY(Node)){
            .capacity=1,
            .len=1,
            .data=(Node[]){{
                .painter={0},
                .pos ={10,10},
                .size={
                    (Size){.kind=SizeKindFixed,.size=100,},
                    (Size){.kind=SizeKindFixed,.size=100,},
                },
            }},
        },
    };
}

void test_ui_compute(){
    void(*cases[])(TestCase* tc) = {
        test_no_children_fixed_position,
    };

    int test_count = sizeof(cases) / sizeof(cases[0]);
    for (int i = 0; i < test_count; i++) {
        mt_total++;
        test_ui_compute_case(cases[i]);
    }
}
