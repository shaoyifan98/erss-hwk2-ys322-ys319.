#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include "httpProxy.h"

int main() {
  httpProxy proxy = httpProxy();
  proxy.init();
}