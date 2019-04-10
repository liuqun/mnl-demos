#include "my_functions.h"

int main(int argc, char *argv[])
{
	int ret;

	ret = print_genl_family_info_by_family_name_strz("DEMO_GEN_CTRL");
	return(ret);
}
