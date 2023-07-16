#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "pixel_renderer.h"
#include "a_star.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace aStar;

const std::string cfgFilePath{"../datafiles/config.json"};
std::vector<std::string> gridStrings;
std::vector<std::vector<int> > colorData;
std::vector<std::vector<CellType> > gridColors;
std::unordered_map<CellType, sf::Color> cellColors;

sf::Vector2f startpoint, endpoint, gridSize;
int main()
{

  // load the config file
  std::cout << "Loading configs...\n";
  std::ifstream cfgFile{cfgFilePath};
  json config = json::parse(cfgFile);
  std::cout << "Config file parsed successfully\n";

  // load the window dimensions
  sf::Vector2f canvasSize{config["grid size"]["x"].get<float>(), config["grid size"]["y"].get<float>()};
  int upscaleFactor{config["pixel upscaling"].get<int>()};
  sf::Vector2f windowSize{canvasSize.x * upscaleFactor, canvasSize.y * upscaleFactor};

  // convert the array of strings in the config file into a 2D vector of CellTypes to use for pathfinding
  gridStrings = config["grid"].get<std::vector<std::string> >();
  gridSize = {static_cast<float>(gridStrings[0].length()), static_cast<float>(gridStrings.size())};
  if (gridSize.y != canvasSize.y || gridSize.x != canvasSize.x) {
    std::cout << "Warning: Grid size does not match canvas size (grid is " << gridSize.x << 'x'
              << gridSize.y << ", canvas is " << canvasSize.x << 'x' << canvasSize.y << ")\n";
  }

  for (size_t y{0}; y < gridSize.y; ++y) {
    if (gridStrings[y].length() != gridSize.x) {
      std::cout << "Fatal Error: Found grid rows of different lengths\n";
      return 1;
    }
    std::vector<CellType> gridLine{};
    for (size_t x{0}; x < gridSize.x; ++x) {
      switch(gridStrings[y][x]) {
        case '#':
          gridLine.push_back(BLOCKED);
          break;
        default:
          gridLine.push_back(EMPTY);
          break;
      }
    }
    gridColors.push_back(gridLine);
  }
  std::cout << "Grid built successfully" << '\n';

  // load the start and end points
  startpoint = {config["start point"][0].get<float>(), config["start point"][1].get<float>()};
  endpoint = {config["end point"][0].get<float>(), config["end point"][1].get<float>()};

  // check for errors/warnings with start and end points
  if (startpoint.x < 0 || startpoint.x >= gridSize.x || startpoint.y < 0 || startpoint.y >= gridSize.y) {
    std::cout << "Fatal Error: Start point out of bounds\n";
    return 3;
  }
  if (endpoint.x < 0 || endpoint.x >= gridSize.x || endpoint.y < 0 || endpoint.y >= gridSize.y) {
    std::cout << " Fatal Error: End point out of bounds\n";
    return 3;
  }
  if (startpoint.x == endpoint.x && startpoint.y == endpoint.y) {
    std::cout << "Warning: Start point and end point have the same coordinates\n";
  }
  if (gridColors[startpoint.y][startpoint.x] == BLOCKED) {
    std::cout << "Warning: Start point replaced a blocked cell\n";
  }
  if (gridColors[endpoint.y][endpoint.x] == BLOCKED) {
    std::cout << "Warning: End point replaced a blocked cell\n";
  }

  // add start and end points to the grid
  gridColors[startpoint.y][startpoint.x] = START;
  gridColors[endpoint.y][endpoint.x] = END;

  // load the array of color values into a map of sf::Color objects
  colorData = config["cell colors"].get<std::vector<std::vector<int> > >();
  for (size_t i{0}; i < colorData.size(); ++i) {
    cellColors[static_cast<CellType>(i)] = sf::Color(colorData[i][0], colorData[i][1], colorData[i][2]);
  }

  // load the step time and create a clock
  const int stepInterval = config["step time"].get<int>();
  int stepTimer{stepInterval};
  int deltaTime;
  sf::Clock clock{};

  std::cout << "All configs loaded successfully\n";

  // set up the pixel renderer
  PixelRenderer canvas{windowSize, upscaleFactor};
  sf::RectangleShape pixel({1, 1});
  pixel.setOutlineColor({0, 0, 0});

  // create a window
  auto window = sf::RenderWindow{ { static_cast<unsigned int>(windowSize.x),
                                    static_cast<unsigned int>(windowSize.y)},
                                  "A* Pathfinding Demo" };
  window.setFramerateLimit(60);

  // create pathfinder
  Pathfinder pathfinder{};
  pathfinder.init(gridColors, startpoint, endpoint);

  while (window.isOpen())
  {
    // handle events
    for (auto event = sf::Event{}; window.pollEvent(event);)
    {
      if (event.type == sf::Event::Closed)
      {
        window.close();
      }
    }

    // step the algorithm forward if the timer has ended
    deltaTime = clock.restart().asMilliseconds();
    stepTimer -= deltaTime;
    if (stepTimer <= 0) {
      pathfinder.step(gridColors);
      stepTimer = stepInterval;
    }

    window.clear();
    canvas.clear();

    // render the grid
    for (size_t y{0}; y < gridSize.y; ++y) {
      for (size_t x{0}; x < gridSize.x; ++x) {
        pixel.setFillColor(cellColors[gridColors[y][x]]);
        pixel.setPosition(x, y);
        canvas.draw(pixel);
      }
    }

    canvas.display(window);
    window.display();
  }
}