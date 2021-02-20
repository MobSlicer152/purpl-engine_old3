#include <stdbool.h>

#include <purpl/purpl.h>

int main(int argc, char *argv[])
{
	ubyte log_index;
	struct purpl_logger *logger;

	/* Unused parameters */
	NOPE(argc);
	NOPE(argv);

	/* Initialize a logger */
	logger = purpl_init_logger(&log_index, INFO, DEBUG, "purpl.log");
	if (!logger) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return -1;
	}

	/* Write a message */
	purpl_write_log(logger, FILENAME, __LINE__, -1, -1, "test");

	/* Close the logger */
	purpl_end_logger(logger, true);

	return 0;
}
