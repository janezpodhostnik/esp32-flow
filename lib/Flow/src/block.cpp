#include "block.h"

Block::Block() {
  this->height = 0;
  this->id = "";
}

Block::Block(unsigned long height, String id) {
  this->height = height;
  this->id = id;
}

Block::~Block() {}

bool Block::is_zero() const {
  return this->height == 0;
}