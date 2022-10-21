#include "display.h"

#include <GLFW/glfw3.h>
#include <display/lodepng.h>

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Display::Impl {
 public:
  Impl();
  ~Impl();

  void DrawTile(float x, float y, TileType type, float alpha);
  void DrawWinMessage();

  bool IsKeyPressed(ActionKey key) const;
  bool Closed() const;
  void ProcessEvents();
  void Render();

 private:
  void InitWindow();
  void InitOpenGL();
  void InitTextures();

  unsigned LoadTexture(const std::string& filename);

 private:
  struct Tile {
    float x;
    float y;
    TileType type;
    float alpha;

    Tile(float x, float y, TileType type, float alpha)
        : x(x), y(y), type(type), alpha(alpha) {}
  };

  GLFWwindow* Window_;

  unsigned NextTextureIndex_;
  std::unordered_map<TileType, unsigned> TileTextures_;
  unsigned WinTexture_;

  std::vector<Tile> Tiles_;
  bool WinMessage_;
};

Display::Impl::Impl() : NextTextureIndex_(0), Tiles_(), WinMessage(false) {
  InitWindow();
  InitOpenGL();
  InitTextures();
}

Display::Impl::~Impl() {
  glfwDestroyWindow(Window_);
  glfwTerminate();
}

void Display::Impl::InitWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("GLFW: Initialization failed!");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SAMPLES, 4);
  Window_ = glfwCreateWindow(700, 700, "2048", NULL, NULL);
  if (!Window_) {
    glfwTerminate();
    throw std::runtime_error("GLFW: Window creation failed!");
  }

  glfwMakeContextCurrent(Window_);
  glfwSwapInterval(1);
}

void Display::Impl::InitOpenGL() {
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glEnable(GL_TEXTURE_2D);
#ifndef WIN32
  glEnable(GL_MULTISAMPLE);
#endif
}

void Display::Impl::InitTextures() {
  TileTextures_[TileType::kTile_1] = LoadTexture("data/1.png");
  TileTextures_[TileType::kTile_2] = LoadTexture("data/2.png");
  TileTextures_[TileType::kTile_4] = LoadTexture("data/4.png");
  TileTextures_[TileType::kTile_8] = LoadTeTileTextures_xture("data/8.png");
  TileTextures_[TileType::kTile_16] = LoadTexture("data/16.png");
  TileTextures_[TileType::kTile_32] = LoadTexture("data/32.png");
  TileTextures_[TileType::kTile_64] = LoadTexture("data/64.png");
  TileTextures_[TileType::kTile_128] = LoadTexture("data/128.png");
  TileTextures_[TileType::kTile_256] = LoadTexture("data/256.png");
  TileTextures_[TileType::kTile_512] = LoadTexture("data/512.png");
  TileTextures_[TileType::kTile_1024] = LoadTexture("data/1024.png");
  TileTextures_[TileType::kTile_2048] = LoadTexture("data/2048.png");

  WinTexture_ = LoadTexture("data/win.png");
}

unsigned Display::Impl::LoadTexture(const std::string& filename) {
  std::vector<unsigned char> image;
  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, filename);
  if (error) {
    throw std::runtime_error(std::string("LODEPNG: ") +
                             lodepng_error_text(error));
  }

  unsigned textureId = NextTextureIndex_++;
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image.data());
  return textureId;
}

void Display::Impl::DrawTile(float x, float y, TileType type, float alpha) {
  Tiles_.emplace_back(x, y, type, alpha);
}

void Display::Impl::DrawWinMessage() { WinMessage_ = true; }

bool Display::Impl::IsKeyPressed(ActionKey key) const {
  switch (key) {
    case ActionKey::kKeyUp:
      return glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS ||
             glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS;
    case ActionKey::kKeyDown:
      return glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS ||
             glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS;
    case ActionKey::kKeyLeft:
      return glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS ||
             glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS;
    case ActionKey::kKeyRight:
      return glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS ||
             glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    default:
      return false;
  }
}

bool Display::Impl::Closed() const { return glfwWindowShouldClose(Window_); }

void Display::Impl::ProcessEvents() { glfwPollEvents(); }

void Display::Impl::Render() {
  int width, height;
  glfwGetFramebufferSize(Window_, &width, &height);

  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.5f, 1.0f, 0.83f, 1.0f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-0.5f, 3.5f, 3.5f, -0.5f, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  for (const auto& tile : Tiles_) {
    glBindTexture(GL_TEXTURE_2D, TileTextures_[tile.type]);
    glColor4f(1.0f, 1.0f, 1.0f, tile.alpha);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(tile.y - 0.5f, tile.x + 0.5f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(tile.y - 0.5f, tile.x - 0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(tile.y + 0.5f, tile.x - 0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(tile.y + 0.5f, tile.x + 0.5f);
    glEnd();
  }

  if (WinMessage_) {
    glBindTexture(GL_TEXTURE_2D, WinTexture);
    glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-0.5f, 3.5f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(3.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(3.5f, 3.5f);
    glEnd();
  }

  glfwSwapBuffers(Window_);

  Tiles_.clear();
  WinMessage_ = false;
}

Display::Display() : Impl_(new Impl()) {}

Display::~Display() {}

void Display::DrawTile(float x, float y, TileType type, float alpha) {
  Impl_->DrawTile(x, y, type, alpha);
}

void Display::DrawWinMessage() { Impl_->DrawWinMessage(); }

double Display::GetTime() const { return glfwGetTime(); }

bool Display::IsKeyPressed(ActionKey key) const {
  return Impl_->IsKeyPressed(key);
}

bool Display::Closed() const { return Impl_->Closed(); }

void Display::ProcessEvents() { Impl_->ProcessEvents(); }

void Display::Render() { Impl_->Render(); }
