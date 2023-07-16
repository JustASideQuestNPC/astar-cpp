#pragma once
#include <vector>
#include <limits>

namespace aStar {
  // for compactness
  constexpr int infinity = std::numeric_limits<int>::max();
  int sqr(int x) { return x * x; }

  // mainly for display, also used in the Pathfinder ctor
  enum CellType {
    EMPTY,
    BLOCKED,
    START,
    END,
    OPEN,
    CLOSED
  };

  struct Node {
    int x, y;    // x and y coordinate
    int px{-1};  // parent x coordinate
    int py{-1};  // parent y coordinate
    int g;       // distance from the starting node
    int h;       // estimated distance from the end node ("heuristic")
    int f;       // estimated total cost of the node (g + h)

    bool operator==(const Node &other) const {
      return this->x == other.x && this->y == other.y;
    }
    bool operator!=(const Node &other) const {
      return this->x != other.x || this->y != other.y;
    }
  };

  // for generating children
  const std::vector<Node> adjacentCoords{
    { 0, -1},
    { 1, -1},
    { 1,  0},
    { 1,  1},
    { 0,  1},
    {-1,  1},
    {-1,  0},
    {-1, -1},
  };

  // handles all pathfinding
  struct Pathfinder {
    Node start, end;
    std::vector<Node> openNodes, closedNodes;
    int gridWidth, gridHeight;
    bool pathFound{false};

    void init(std::vector<std::vector<CellType> > &grid, sf::Vector2f startpoint, sf::Vector2f endpoint) {
      gridWidth = grid[0].size();
      gridHeight = grid.size();

      closedNodes = {};
      openNodes = {{static_cast<int>(startpoint.x), static_cast<int>(startpoint.y)}}; // add the start node to the beginning of the open list

      start = {static_cast<int>(startpoint.x), static_cast<int>(startpoint.y)};
      end = {static_cast<int>(endpoint.x), static_cast<int>(endpoint.y)};
    }

    void reset() {

    }

    void step(std::vector<std::vector<CellType> > &gridColors) {
      if (openNodes.size() == 0 || pathFound) return;

      // find the node with the lowest cost
      Node parent{openNodes[0]};
      size_t parentIndex{0};
      for (size_t i{1}; i < openNodes.size(); ++i) {
        if (openNodes[i].f < parent.f) {
          parent = openNodes[i];
          parentIndex = i;
        }
      }

      // update grid colors
      if (parent != start && parent != end) gridColors[parent.y][parent.x] = EMPTY;

      // move the parent node to the closed list
      openNodes.erase(std::next(openNodes.begin(), parentIndex));
      closedNodes.push_back(parent);

      // check if the closed node is the end node
      if (parent == end) {
        pathFound = true;
        return;
      }

      // update grid colors
      for (size_t y{0}; y < gridColors.size(); ++y) {
        for (size_t x{0}; x < gridColors[0].size(); ++x) {
          if (gridColors[y][x] == OPEN) gridColors[y][x] == EMPTY;
        }
      }

      // words cannot describe how inefficient this is
      Node p{parent};
      while (p.px != -1 && p.py != -1) {
        gridColors[p.y][p.x] = OPEN;
        for (size_t i{0}; i < closedNodes.size(); ++i) {
          if (closedNodes[i].x == p.px && closedNodes[i].y == p.py) {
            p = closedNodes[i];
            break;
          }
        }
      }

      // generate children
      std::vector<Node> children{};
      Node c;
      for (size_t i{0}; i < adjacentCoords.size(); ++i) {
        c = {parent.x + adjacentCoords[i].x, parent.y + adjacentCoords[i].y, parent.x, parent.y}; // create the node

        // ensure node is within the grid
        if (c.x < 0 || c.x >= gridWidth || c.y < 0 || c.y >= gridHeight) continue;

        // ensure node is walkable
        if (gridColors[c.y][c.x] == BLOCKED) continue;

        // add the node to the child list
        children.push_back(c);
      }

      // loop through children
      bool childClosed, smallerEntryFound;
      for (Node child : children) {
        childClosed = false;
        // check if child is on the closed list
        for (Node n : closedNodes) {
          if (n == child) {
            childClosed = true;
            break;
          }
        }
        if (childClosed) continue;

        // find f, g, and h values
        child.g = parent.g + 1;
        child.h = (sqr(child.x - end.x) + sqr(child.y - end.y));
        child.f = child.g + child.h;

        // check if a child with a smaller g value is in the open list
        smallerEntryFound = false;
        for (Node n : openNodes) {
          if (child == n && child.g > n.g) {
            smallerEntryFound = true;
            break;
          }
        }
        if (smallerEntryFound) continue;

        // add the child to the open list
        openNodes.push_back(child);
      }
    }
  };
}