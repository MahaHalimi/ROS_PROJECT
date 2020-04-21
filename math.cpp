#include <math.h>

using namespace std;

float norm(float x[2])
{
  return sqrt(pow(x[0],2) + pow(x[1],2));
}

float angle(float x[2])
{
  return atan2(x[1],x[0]);
}

float sawtooth(float x)
{
  return 2.*atan(tan(x/2.));
  return fmod(x + M_PI, 2.*M_PI) - M_PI; // this is equivalent
}

float customrand()
{
  return 2.*(-0.5 + rand() / double(RAND_MAX));
}