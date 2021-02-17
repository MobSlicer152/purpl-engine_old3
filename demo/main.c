#include <purpl/purpl.h>

int main(int argc, char *argv)
{
	ubyte log_index;
	struct purpl_logger *logger;

	/* Initialize a logger */
	logger = purpl_init_logger(&log_index, INFO, DEBUG, "purpl.log");
	if (!logger) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}
