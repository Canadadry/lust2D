#define MINITEST_IMPL
#include "minitest.h"

void test_ui_add();
void test_ui_compute();
void test_ui_grow_along_axis();

int main(int argc,char ** argv){
    TEST_GROUP(test_ui_add);
    TEST_GROUP(test_ui_grow_along_axis);
    TEST_GROUP(test_ui_compute);
    test_done();
    return 0;
}
