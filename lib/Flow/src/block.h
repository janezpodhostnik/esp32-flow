#pragma once
#include <WString.h>

class Block {
  public:
    Block();
    Block( unsigned long height,String id);
    ~Block();

    String id;
    unsigned long height;

    bool is_zero() const;
};