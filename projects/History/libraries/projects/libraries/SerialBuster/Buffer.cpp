#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <string.h> // memcpy
#include "Buffer.h"

Buffer::Buffer() {}

void Buffer::init(uint16_t _size) { 
  buf = (uint8_t *) calloc(_size, sizeof(uint8_t));
  size = _size;
  length = 0;
  cursor_in = 0;
  cursor_out = 0;
}

// returns index of newly enqueued byte
uint16_t Buffer::enqueueUInt8(uint8_t b) {
  uint16_t ret = 0;
  length++;
  cursor_in %= size;
  buf[cursor_in] = b;
  ret = cursor_in;
  cursor_in++;
  return ret;
}

uint16_t Buffer::enqueueUInt8(uint8_t * data, uint16_t len) {
  uint16_t ret = 0;
  bool first = false;
  for (uint16_t i=0; i<len; i++){
    if(first) {
      ret = enqueueUInt8(data[i]);
      first = false;
    }else{
      enqueueUInt8(data[i]);
    }
  }
  return ret;
}

uint16_t Buffer::enqueueUInt16(uint16_t bb) {
  uint16_t ret = 0;
  length += 2;
  cursor_in %= size;
  writeUInt16(bb, cursor_in);
  ret = cursor_in;
  cursor_in += 2;
  return ret;
}

uint8_t Buffer::peek() {
  return buf[cursor_out % size];
}

void Buffer::clear() {
  cursor_in = 0;
  cursor_out = 0;
  length = 0;
}

uint8_t Buffer::dequeue() {
  length--;
  cursor_out %= size;
  return buf[cursor_out++];
}

// stack pop (filo)
uint8_t Buffer::pop() {
  length--;
  cursor_in--;
  cursor_in %= size;
  return buf[cursor_in];
}


// ###### INDEX BASED FUNCTIONS

void Buffer::writeUInt8(uint8_t val, uint16_t offset){
  buf[(offset % size)] = val;
}

uint8_t Buffer::readUInt8(uint16_t offset) {
  return buf[(offset % size)];
}

void Buffer::writeUInt16(uint16_t val, uint16_t offset) {
  buf[((offset) % size)] = val & 0xFF;
  buf[((offset+1) % size)] = (val >> 8) & 0xFF;
}

uint16_t Buffer::readUInt16(uint16_t offset) {
  return (uint16_t) (buf[(offset+1 % size)] << 8 | buf[(offset % size)]);
}

void Buffer::writeUInt32(uint32_t val, uint16_t offset) {
  buf[offset+0 % size] = (val & 0xFF);
  buf[offset+1 % size] = ((val >> 8) & 0xFF);
  buf[offset+2 % size] = ((val >> 16) & 0xFF);
  buf[offset+3 % size] = ((val >> 24) & 0xFF);
}

uint32_t Buffer::readUInt32(uint16_t offset) {
  return (uint32_t) ((uint32_t) buf[offset+0 % size] |
                     (uint32_t) buf[offset+1 % size] << 8 |
                     (uint32_t) buf[offset+2 % size] << 16 |
                     (uint32_t) buf[offset+3 % size] << 24);
}

void Buffer::writeFloat(float val, uint16_t offset) {
  union buffer_u u;
  u.f = val;
  for (uint8_t i = 0; i < 4; i++)
    buf[offset+i % size] = u.b[i];
}

float Buffer::readFloat(uint16_t offset) {
  union buffer_u u;
  for (uint8_t i = 0; i < 4; i++)
    u.b[i] = buf[offset+i % size];
  return u.f;
}

uint16_t Buffer::readCursorPos() {
  return cursor_out;
}

uint16_t Buffer::getDataLength() {
  return length;
}

uint16_t Buffer::getSize() {
  return size;
}

uint8_t & Buffer::operator[] (uint16_t index) {
  return buf[(index % size)];
}

void Buffer::release() {
  free(buf);
}

Buffer::~Buffer() {
  release();
}
