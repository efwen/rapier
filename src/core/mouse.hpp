#pragma once

namespace rp::input {
  struct Mouse {
    typedef enum {
      Left = 0,
      Right,
      Middle,
      ENUM_SIZE,
    } Button;

    typedef struct {
      int x;
      int y;
    } Position;
  };
}