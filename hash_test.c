#include <string.h>
#include <stdio.h>
#include <time.h>

#define MOD_FOR_TYPES(a, t) (a & ((sizeof(t) << 3) - 1))
#define ELEMENTS(a) (sizeof a / sizeof a[0])

unsigned long
tiny_hash(char *str)
{
  unsigned long hash = 0;
  int c;

  while ((c = *str++))
    hash ^= (hash - c) << MOD_FOR_TYPES((hash + c), long);

  return hash;
}

unsigned long
tiny_hash_v2(char *str)
{
  unsigned long hash = 0;
  int c;

  while ((c = *str++))
    hash ^= (hash + c) << MOD_FOR_TYPES((hash - c), long);

  return hash;
}

unsigned long
djb2_hash(char *str)
{
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

unsigned long
sdbm_hash(char *str)
{
  unsigned long hash = 0;
  int c;

  while ((c = *str++))
      hash = c + (hash << 6) + (hash << 16) - hash;

  return hash;
}

unsigned long
lose_lose_hash(char *str)
{
  unsigned int hash = 0;
  int c;

  while ((c = *str++))
    hash += c;

  return hash;
}

unsigned long
fnv_1a_hash(char *str)
{
  unsigned long hash = 0xcbf29ce484222325;
  int c;

  while ((c = *str++))
    hash = (hash ^ c) * 0x100000001b3;

  return hash;
}

unsigned long
java_hash(char *str)
{
  unsigned int hash = 7;
  int c;

  while ((c = *str++))
    hash = hash * 31 + c;

  return hash;
}


int
main(int argc, char* argv[])
{
  FILE *f, *d;
  char str[256], tmp[16];

  struct { char *name; unsigned long (*function) (char*); } fn_pairs[] = {
    {"tiny", tiny_hash},
    {"tiny2", tiny_hash_v2},
    {"djb2", djb2_hash},
    {"sdbm", sdbm_hash},
    {"lose-lose", lose_lose_hash},
    {"java", java_hash},
    {"fnv-1a_example", fnv_1a_hash}
  };

  if (argc != 2)
    return puts("File name needed.");

  if(!(f = fopen(argv[1], "r")))
    return puts("Can't read file.");

  puts("Warming up and writing files.");
  for (int i = 0; i < ELEMENTS(fn_pairs); ++i) {
    strcpy(tmp, fn_pairs[i].name);
    strcat(tmp, "_hash.txt");

    if (!(d = fopen(tmp, "w")))
      return puts("Can't write file.");

    if(str[254])
      return puts("Buffer is too small to read input correctly.");

    while (fgets(str, sizeof str, f))
      fprintf(d, "%lu\n", fn_pairs[i].function(str));

    fclose(d);
    rewind(f);
  }

  puts("Testing...");
  for (int i = 0; i < ELEMENTS(fn_pairs); ++i) {
    clock_t start = clock();

    while (fgets(str, sizeof str, f))
      fn_pairs[i].function(str);

    printf("%s: %ldms\n", fn_pairs[i].name, (clock() - start) / (CLOCKS_PER_SEC / 1000));
    rewind(f);
  }

  puts("");
  fclose(f);
}
