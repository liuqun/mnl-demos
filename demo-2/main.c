#include <stdio.h>
#include "my_functions.h"

const int DEBUG_ENABLED = 0;

int main(int argc, char *argv[])
{
	int id;

	id = get_genl_family_id_by_name("DEMO_GEN_CTRL");
	if (id < 0) {
		fprintf(stderr, "Error: DEMO_GEN_CTRL family does NOT exist\n");
		fprintf(stderr, "(please load demo2.ko kernel module first...)\n");
		return(255);
	}
	printf("Demo2 genl family: name=DEMO_GEN_CTRL, id=%d\n", id);
	if (DEBUG_ENABLED) {
		print_genl_family_info_by_family_name_strz("DEMO_GEN_CTRL");
	}
	return(0);
}
