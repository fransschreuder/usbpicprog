//#include <p30f2010.h>

int main(void);
unsigned int Add(unsigned int a, unsigned int b);
unsigned int x, y, z;

int main(void)
{
  x = 2;
  y = 5;
  z = Add(x,y);
  return 0;
}

unsigned int Add(unsigned int a, unsigned int b)
{
  return(a+b);
}
