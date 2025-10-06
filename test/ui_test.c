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
#define MAX_NODE_LEN 2

bool painter_command_match(const char* test_name,int i,PainterCommand exp, PainterCommand got){
    if (memcmp(&exp, &got, sizeof(PainterCommand))==0){
        return true;
    }
    TEST_ERRORF(test_name,
        "failed command %d dont match\ngot\nx:%d y:%d w:%d h:%d\n want\nx:%d y:%d w:%d h:%d\n",
        i,
        got.x,got.y,got.w,got.h,
        exp.x,exp.y,exp.w,exp.h
    );
    return false;
}

void test_ui_compute_case(void(*init_test)(TestCase* tc)){
    TestCase tc = {0};
    PainterCommand got_commands[MAX_NODE_LEN]={0};
    PainterCommand exp_commands[MAX_NODE_LEN]={0};
    Node nodes[MAX_NODE_LEN]={0};
    tc.tree.nodes.capacity=MAX_NODE_LEN;
    tc.tree.nodes.data=nodes;
    tc.tree.commands.capacity=MAX_NODE_LEN;
    tc.tree.commands.data=got_commands;
    tc.expected.capacity=MAX_NODE_LEN;
    tc.expected.data=exp_commands;
    init_test(&tc);
    compute(&tc.tree,tc.head);
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
    tc->expected.len=1;
    tc->expected.data[0] = (PainterCommand){.x=10,.y=10,.w=100,.h=100,.painter={0}};
    tc->tree.nodes.len=1;
    tc->tree.nodes.data[0]=(Node){
        .painter={0},
        .pos ={10,10},
        .size={
            (Size){.kind=SizeKindFixed,.size=100},
            (Size){.kind=SizeKindFixed,.size=100},
        },
        .first_children=-1,
        .last_children=-1,
        .next=-1,
    };
}

void test_one_children_fixed_position(TestCase* tc){
    tc->name = __func__;
    tc->expected.len=2;
    tc->expected.data[0] = (PainterCommand){.x=10,.y=10,.w=100,.h=100,.painter={0}};
    tc->expected.data[1] = (PainterCommand){.x=20,.y=20,.w=50,.h=50,.painter={0}};
    tc->tree.nodes.len=2;
    tc->tree.nodes.data[0]=(Node){
        .pos ={10,10},
        .layout=LayoutStack,
        .size={
            (Size){.kind=SizeKindFixed,.size=100},
            (Size){.kind=SizeKindFixed,.size=100},
        },
        .first_children = 1,
        .last_children = 1,
        .next=-1,
    };
    tc->tree.nodes.data[1]=(Node){
        .pos ={10,10},
        .size={
            (Size){.kind=SizeKindFixed,.size=50},
            (Size){.kind=SizeKindFixed,.size=50},
        },
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_ui_compute(){
    void(*cases[])(TestCase* tc) = {
        test_no_children_fixed_position,
        test_one_children_fixed_position,
    };

    int test_count = sizeof(cases) / sizeof(cases[0]);
    for (int i = 0; i < test_count; i++) {
        mt_total++;
        test_ui_compute_case(cases[i]);
    }
}
