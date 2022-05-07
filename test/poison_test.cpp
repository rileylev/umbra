#include <umbra/shadow.hpp>

int poison(int x) {
  UMBRA_POISON(x){
    return x;
  }
}
