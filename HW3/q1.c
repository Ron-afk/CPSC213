#include <stdlib.h>
#include <stdio.h>

// YOU: Allocate these global variables, using these names
int  l, b;
int* c;
int  r[10];

int main (int argv, char** argc) {
  // Ignore this block of code
  if (argv != 11) {
    fprintf (stderr, "usage: r[0] ... r[9]\n");
    exit (EXIT_FAILURE);
  }
  for (int u=0; u<10; u++)
    r[u] = atol (argc[1 + u]);

  // YOU: Implement this code
  l  = r[3];
  l  = r[l];
  c  = &b;
  *c = 8;
  c  = &r[r[5]];
  *c = *c + r[4];

  // Ignore this block of code
  printf ("l=%d b=%d c=&r[%d] r={", l, b, c - r);
  for (int u=0; u<10; u++)
    printf("%d%s", r[u], u<9? ", ": "}\n");
  }
