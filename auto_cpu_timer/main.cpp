#include <boost/timer/timer.hpp>

void f()
{
  boost::timer::auto_cpu_timer timer;
  sleep(2);
}


int main()
{
  f();
  return 0;
}
