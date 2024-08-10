#include <alpha/app/app.hpp>

alpha::app app;

void interrupt_handler(int signum)
{
  PLAY_UNUSED(signum);
  app.stop();
}

int main(int argc, char** argv)
{
  std::string name{"alpha"};

  if (argc > 1)
  {
    name = argv[1];
  }

  play::logger::name = name;

  signal(SIGINT, interrupt_handler);

  if (app.start(fmt::format("{}.json", name)))
  {
    app.wait();
  }

  return 0;
}