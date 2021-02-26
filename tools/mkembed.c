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
	size_t k;
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
		output_name = PURPL_CALLOC(strlen("embed.c") + 1, char);
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
	if (!input) {
		fprintf(stderr, "Error: failed to read file: %s\n",
			strerror(errno));
		return errno;
	}
	printf("Using input file \"%s\" (%zu bytes)\n", input_name, input_len);

	/* Open the output file */
	fp = fopen(output_name, "wb");
	if (!fp) {
		fprintf(stderr, "Error: failed to truncate file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Write the start of the array */
	printf("Writing data to \"%s\"...\n", output_name);
	i = fprintf(fp, "const unsigned char %s_start[] = {\n\t", sym_base);
	if (!i) {
		fprintf(stderr, "Error: couldn't write to file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Now write the individual bytes */
	j = 0;
	k = i;
	for (i = 0; i < input_len; i++) {
		errno = 0;
		j += fprintf(fp, "0x%0X", input[i] & 0xFF);
		if (i < input_len - 1)
			j += fprintf(fp, ", ");
		else
			fprintf(fp, "\n");

		/* Check for an error */
		if (errno) {
			fprintf(stderr, "Error: couldn't write to file: %s\n",
				strerror(errno));
			return errno;
		}

		/* Add to the total output written */
		k += j;

		/* Ensure we don't exceed 80 columns/line :) */
		if (j >= 70) {
			j = 0;
			errno = 0;
			fprintf(fp, "\n\t");

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
		"};\nconst unsigned char *%s_end =\n\t(unsigned char *)(%s_start +"
		" sizeof(%s_start));\nconst unsigned long %s_size = sizeof(%s_start);\n",
		sym_base, sym_base, sym_base, sym_base, sym_base);
	if (!i) {
		fprintf(stderr, "Error: couldn't write to file: %s\n",
			strerror(errno));
		return errno;
	}

	/* And now we have our total bytes outputted */
	k += i;

	/* Close the file and free input */
	fclose(fp);
	free(input);
	
	/* And we're done */
	printf("Done! Output file is \"%s\", containing %zu bytes.\n", output_name, k);
	if (!have_custom_output)
		free(output_name);
	return 0;
}

void usage(const char *prog)
{
	printf("Usage: %s <binary file> <symbols basename> [<output>]\n",
	       PURPL_GET_FILENAME(prog));
	exit(EINVAL);
}
