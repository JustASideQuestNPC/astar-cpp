// Created by Joseph Williams (JustASideQuestNPC) on 7/11/2023.
//
// Defines the PixelRenderer class, which upscales a texture to create a pixelated image
// without any blurring, misalignment, or other artifacts.

#ifndef ASTAR_CPP_PIXEL_RENDERER_H
#define ASTAR_CPP_PIXEL_RENDERER_H
#include <SFML/Graphics.hpp>
#include <cmath> // do i really need to include this just for floor()?

struct PixelRenderer {
  sf::RenderTexture bufferTexture{};
  sf::Sprite bufferSprite;
  sf::View bufferView;
  PixelRenderer(const sf::Vector2f windowSize, const int upscaleFactor) { // both the x and y in windowSize *must* be divisible by the upscale factor
    sf::Vector2f canvasSize{
      static_cast<float>(floor(windowSize.x / upscaleFactor)),
      static_cast<float>(floor(windowSize.y / upscaleFactor))
    };
    bufferTexture.create(canvasSize.x, canvasSize.y); // this will be upscaled to fit the window
    bufferTexture.setSmooth(false); // prevents blurring

    bufferSprite.setTexture(bufferTexture.getTexture());

    bufferView.setSize(canvasSize);
    bufferView.setCenter(canvasSize.x / 2, canvasSize.y / 2);
  }

  void clear(const sf::Color &color=sf::Color(0x000000FF)) {
    bufferTexture.clear();
  }

  void display(sf::RenderWindow &window) {
    bufferTexture.display(); // finalize the buffer image

    sf::View tempView{window.getView()}; // get the current window view so we can put it back later

    // draw the buffer sprite to the window
    window.setView(bufferView);
    window.draw(bufferSprite);

    window.setView(tempView); // leave everything like we found it
  }

  // wrappers for the buffer texture to make things a bit more clean
  void draw(const sf::Drawable &drawable, const sf::RenderStates &states=sf::RenderStates::Default) {
    bufferTexture.draw(drawable, states);
  }
  void draw(const sf::Vertex *vertices, std::size_t vertexCount, sf::PrimitiveType type,
            const sf::RenderStates &states=sf::RenderStates::Default) {
    bufferTexture.draw(vertices, vertexCount, type, states);
  }
  void draw(const sf::VertexBuffer &vertexBuffer, const sf::RenderStates &states=sf::RenderStates::Default) {
    bufferTexture.draw(vertexBuffer, states);
  }
  void draw(const sf::VertexBuffer &vertexBuffer, std::size_t firstVertex, std::size_t vertexCount,
            const sf::RenderStates &states=sf::RenderStates::Default) {
    bufferTexture.draw(vertexBuffer, firstVertex, vertexCount, states);
  }
};

#endif //ASTAR_CPP_PIXEL_RENDERER_H
