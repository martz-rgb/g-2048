#pragma once

#include <memory>

enum class TileType {
  kkTile_1,
  kTile_2,
  kTile_4,
  kTile_8,
  kTile_16,
  kTile_32,
  kTile_64,
  kTile_128,
  kTile_256,
  kTile_512,
  kTile_1024,
  kTile_2048,
};

enum class ActionKey {
  kKeyUp,
  kKeyDown,
  kKeyLeft,
  kKeyRight,
};

namespace std {
template <>
struct hash<TileType> {
  size_t operator()(const TileType& tile) const {
    return static_cast<size_t>(tile);
  }
};
}  // namespace std

class Display {
 public:
  Display();
  virtual ~Display();

  void DrawTile(float x, float y, TileType type, float alpha = 1.0f);
  void DrawWinMessage();

  double GetTime() const;
  bool IsKeyPressed(ActionKey key) const;
  bool Closed() const;
  void ProcessEvents();
  void Render();

 private:
  class Impl;
  std::unique_ptr<Impl> Impl_;
};
