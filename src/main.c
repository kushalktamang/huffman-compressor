#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_PATH "sample.txt"
#define BYTE_VALUES 256
#define CHUNK_SIZE 4096

/*
 * Reads fp to EOF in chunks and tallies how often each byte value occurs.
 * Returns 0 on success, -1 if a read error occurred.
 * Returns the total number of bytes read through *total.
 */
static int count_frequencies(FILE *fp, uint64_t freq[BYTE_VALUES],
                             uint64_t *total) {
  unsigned char buf[CHUNK_SIZE];
  size_t n;

  *total = 0;
  while ((n = fread(buf, 1, sizeof buf, fp)) > 0) {
    for (size_t i = 0; i < n; i++) {
      freq[buf[i]]++;
    }
    *total += n;
  }

  // fread returns 0 for both EOF and error; ferror tells them apart.
  return ferror(fp) ? -1 : 0;
}

static void print_frequencies(const uint64_t freq[BYTE_VALUES]) {
  printf("byte char count \n");
  for (int b = 0; b < BYTE_VALUES; b++) {
    if (freq[b] == 0) {
      continue;
    }
    if (isprint(b)) {
      printf("0x%02X  '%c'   %" PRIu64 "\n", b, b, freq[b]);
    } else {
      printf("0x%02X  --    %" PRIu64 "\n", b, freq[b]);
    }
  }
}

int main(void) {
  uint64_t freq[BYTE_VALUES] = {0};
  uint64_t total = 0;

  FILE *fp = fopen(INPUT_PATH, "rb");
  if (fp == NULL) {
    fprintf(stderr, "error: cannot open '%s': %s\n", INPUT_PATH,
            strerror(errno));
    return EXIT_FAILURE;
  }

  if (count_frequencies(fp, freq, &total)) {
    fprintf(stderr, "error: failed while reading '%s'\n", INPUT_PATH);
    fclose(fp);
    return EXIT_FAILURE;
  }

  if (fclose(fp) != 0) {
    fprintf(stderr, "error: cannot close '%s': %s\n", INPUT_PATH,
            strerror(errno));
    return EXIT_FAILURE;
  }

  if (total == 0) {
    printf("'%s' is empty, nothing to count.\n", INPUT_PATH);
    return EXIT_SUCCESS;
  }

  print_frequencies(freq);
  printf("total: %" PRIu64 " bytes\n", total);
  return EXIT_SUCCESS;
}
