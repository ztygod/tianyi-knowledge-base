#include<stdio.h>
#include "rng.h"

int main(void)
{
  printf("you own $%.2f\n", rng_f());
  return 0;
}
