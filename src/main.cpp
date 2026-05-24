#include <iostream>
#include "Communication.hpp"

int main() {
  Communication::Socket s;
  s.bind("127.0.0.1", 8080);
  return 0;
}
