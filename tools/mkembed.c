#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <purpl/types.h>
#include <purpl/util.h>

void usage(const char *prog);

int main(int argc, char *argv[])
{
	size_t i;
	size_t j;
	char *sym_base;
	char *input;
	size_t input_len;
	FILE *fp;
	char *input_name;
	bool have_custom_output;
	char *output_name;

	/* Check arguments */
	if (argc < 3)
		usage(argv[0]);

	/* Make an alias to our input file and the base name for the symbols */
	input_name = argv[1];
	sym_base = argv[2];

	/* Check if there's an alternate output file */
	have_custom_output = (argc > 3);

	/* Make a separate buffer */
	if (have_custom_output) {
		output_name = argv[3];
	} else {
		/* Allocate a buffer */
		output_name = PURPL_CALLOC(strlen("embed.c"), char);
		if (!output_name) {
			fprintf(stderr,
				"Error: failed to allocate buffer: %s\n",
				strerror(errno));
			return errno;
		}

		/* Copy in the text */
		strcpy(output_name, "embed.c");
	}

	/* Use my handy function to read the input file */
	input = purpl_read_file(&input_len, NULL, false, "%s", input_name);
	if (input) {
		fprintf(stderr, "Error: failed to read file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Open the output file now */
	fp = fopen(output_name, "rb");
	if (!fp) {
		fprintf(stderr, "Error: failed to open file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Check if it's got anything in it */
	fseek(fp, 0, SEEK_END);
	if (ftell(fp)) {
		printf("File not empty. Shall I overwrite it? [n]: ");
		i = getc(stdin);
		if (i == 'y' || i == 'Y') {
			printf("\nOverwriting file.\n");
		} else {
			printf("\nNot overwriting file.\n");
			return ECANCELED;
		}
	}

	/* Reopen the file and truncate it */
	fp = freopen(output_name, "wb", fp);
	if (!fp) {
		fprintf(stderr, "Error: failed to truncate file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Write the start of the array */
	i = fprintf(fp, "const unsigned char %s_start[] = {\n", sym_base);
	if (!i) {
		fprintf(stderr, "Error: couldn't write to file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Now write the individual bytes */
	j = 0;
	for (i = 0; i < input_len; i++) {
		errno = 0;
		j += fprintf(fp, "0x%X,", input[i]);

		/* Check for an error */
		if (errno) {
			fprintf(stderr, "Error: couldn't write to file: %s\n",
				strerror(errno));
			return errno;
		}

		/* Ensure we don't exceed 80 columns/line :) */
		if (j >= 70) {
			j = 0;
			errno = 0;
			fprintf(fp, "\n");

			/* Check for an error again */
			if (errno) {
				fprintf(stderr,
					"Error: couldn't write to file: %s\n",
					strerror(errno));
				return errno;
			}
		}
	}

	/* Terminate the array and write the other symbols */
	i = fprintf(
		fp,
		"\b\b };\nconst unsigned char %s_end[] = %s_start + sizeof(%s_start); const size_t %s_size = %s_end - %s_start;",
		sym_base, sym_base, sym_base, sym_base, sym_base, sym_base);
	if (!i) {
		fprintf(stderr, "Error: couldn't write to file: %s\n",
			strerror(errno));
		return errno;
	}
}

void usage(const char *prog)
{
	printf("Usage: %s <binary file> <symbols basename> [<output>]\n",
	       PURPL_GET_FILENAME(prog));
	exit(-EINVAL);
}
