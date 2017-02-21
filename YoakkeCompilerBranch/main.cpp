#include <iostream>
#include <string>

#include "src\parse\ylexer.h"
#include "src\parse\yexpr_parser.h"
#include "src\utility\static_block.h"
#include "src\utility\double_dispatcher.h"
#include "src\ast\expr.h"
#include "src\unit_testing\expr_unit_test.h"

int main(void) {
	yk::unit_testing::test_expr_parser();

	system("PAUSE");
	return 0;
}
