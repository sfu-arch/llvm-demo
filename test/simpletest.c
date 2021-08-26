
#include <stdio.h>

void milad () {
  int a = 2, b = 1;
  int c = a  + b;
  int d = c / 2;
  printf ("%d\n", d);
  printf ("%d\n", a++);
  printf ("%d\n", !b);
  printf ("milad\n");
}
void a() { printf("Found a\n"); }
void b() { printf("Found b\n"); }
int main() {
  b();
  a();
  b();
  // milad();
  printf("Done!\n");
  return 0;
}

