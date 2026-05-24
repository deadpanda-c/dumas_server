#include <iostream>
#include "Communication.hpp"

int main() {
  Communication::Socket s;

  s.init("127.0.0.1", 8080);

  s.run();
  return 0;
}
