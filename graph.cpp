#include "graph.h"
#include <algorithm>
#include <climits>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

using namespace std;

// constructor, empty graph
// directionalEdges defaults to true
Graph::Graph(bool directionalEdges) { doubleEdge = !directionalEdges; }

// destructor
Graph::~Graph() {
  for (auto &i : innerGraph) {
    delete i;
  }
  innerGraph.clear();
}

// @return total number of vertices
int Graph::verticesSize() const { return innerGraph.size(); }

// @return total number of edges
int Graph::edgesSize() const {
  int ret{0};
  for (auto i : innerGraph) {
    ret += i->edges.size();
  }
  return (doubleEdge ? ret / 2 : ret);
}

int Graph::getIndex(const string &inVertex, const string &find) const {
  if (inVertex.empty()) {
    for (int i = 0; i < innerGraph.size(); i++) {
      if (innerGraph[i]->name == find) {
        return i;
      }
    }
    return -1;
  }
  int inVertexIndex = getIndex("", inVertex);
  for (int i = 0; i < innerGraph[inVertexIndex]->edges.size(); i++) {
    if (innerGraph[inVertexIndex]->edges[i].first->name == find) {
      return i;
    }
  }
  return -1;
}

// @return number of edges from given vertex, -1 if vertex not found
int Graph::vertexDegree(const string &label) const {
  int ret = getIndex("", label);
  if (ret != -1) {
    return innerGraph[ret]->edges.size();
  }
  return ret;
}

// @return true if vertex added, false if it already is in the graph
bool Graph::add(const string &label) {
  if (!contains(label)) {
    Vertex *ins = new Vertex(label);
    for (int i = 0; i < innerGraph.size(); i++) {
      if (innerGraph[i]->name > label) {
        innerGraph.insert(innerGraph.begin() + i, ins);
        return true;
      }
    }
    innerGraph.insert(innerGraph.end(), ins);
    return true;
  }
  return false;
}

/** return true if vertex already in graph */
bool Graph::contains(const string &label) const {
  return getIndex("", label) != -1;
}

// @return string representing edges and weights, "" if vertex not found
// A-3->B, A-5->C should return B(3),C(5)
string Graph::getEdgesAsString(const string &label) const {
  int index = getIndex("", label);
  if (index != -1 && !innerGraph[index]->edges.empty()) {
    string ret;
    for (auto &edge : innerGraph[index]->edges) {
      ret.append(edge.first->name + "(" + to_string(edge.second) + "),");
    }
    ret.erase(ret.end() - 1);
    return ret;
  }
  return "";
}

// @return true if successfully connected
bool Graph::connect(const string &from, const string &to, int weight) {
  if (!this->contains(from)) {
    this->add(from);
  }
  if (!this->contains(to) && from != to) {
    this->add(to);
  }
  int fromIndex = getIndex("", from);
  int toIndex = getIndex("", to);
  bool fromInserted = false;
  if (getIndex(from, to) == -1 && from != to) {
    for (int i = 0; i < innerGraph[fromIndex]->edges.size() && !fromInserted;
         i++) {
      if (innerGraph[fromIndex]->edges[i].first->name > to) {
        innerGraph[fromIndex]->edges.insert(
            innerGraph[fromIndex]->edges.begin() + i,
            pair<Vertex *, int>(innerGraph[toIndex], weight));
        fromInserted = true;
      }
    }
    if (!fromInserted) {
      innerGraph[fromIndex]->edges.insert(
          innerGraph[fromIndex]->edges.end(),
          pair<Vertex *, int>(innerGraph[toIndex], weight));
    }
    if (doubleEdge) {
      bool toInserted = false;
      for (int i = 0; i < innerGraph[toIndex]->edges.size() && !toInserted;
           i++) {
        if (innerGraph[toIndex]->edges[i].first->name > from) {
          innerGraph[toIndex]->edges.insert(
              innerGraph[toIndex]->edges.begin() + i,
              pair<Vertex *, int>(innerGraph[fromIndex], weight));
          toInserted = true;
        }
      }
      if (!toInserted) {
        innerGraph[toIndex]->edges.insert(
            innerGraph[toIndex]->edges.end(),
            pair<Vertex *, int>(innerGraph[fromIndex], weight));
      }
    }
    return true;
  }
  return false;
}

bool Graph::disconnect(const string &from, const string &to) {
  if (this->contains(from) && this->contains(to) && from != to) {
    int fromIndex = getIndex("", from);
    int toIndex = getIndex("", to);
    for (int i = 0; i < innerGraph[fromIndex]->edges.size(); i++) {
      if (innerGraph[fromIndex]->edges[i].first->name == to) {
        innerGraph[fromIndex]->edges.erase(
            innerGraph[fromIndex]->edges.begin() + i);
        if (doubleEdge) {
          for (int j = 0; j < innerGraph[toIndex]->edges.size(); j++) {
            if (innerGraph[toIndex]->edges[j].first->name == from) {
              innerGraph[toIndex]->edges.erase(
                  innerGraph[toIndex]->edges.begin() + j);
            }
          }
        }
        return true;
      }
    }
  }
  return false;
}

void Graph::dfsHelper(const Vertex *curr, vector<string> &visited) {
  if (find(visited.begin(), visited.end(), curr->name) == visited.end()) {
    visited.emplace_back(curr->name);
    for (const auto &edge : curr->edges) {
      dfsHelper(edge.first, visited);
    }
  }
}

// depth-first traversal starting from given startLabel
void Graph::dfs(const string &startLabel, void visit(const string &label)) {
  if (this->contains(startLabel)) {
    vector<string> retVector;
    string ret;
    dfsHelper(innerGraph[getIndex("", startLabel)], retVector);
    for (auto &item : retVector) {
      ret.append(item);
    }
    return visit(ret);
  }
}

// breadth-first traversal starting from startLabel
void Graph::bfs(const string &startLabel, void visit(const string &label)) {
  if (this->contains(startLabel)) {
    vector<string> visited;
    string ret;
    queue<Vertex *> queue;
    Vertex *curr = innerGraph[getIndex("", startLabel)];
    queue.push(curr);
    visited.emplace_back(queue.front()->name);
    while (!queue.empty()) {
      curr = queue.front();
      queue.pop();
      for (auto &edge : curr->edges) {
        if (find(visited.begin(), visited.end(), edge.first->name) ==
            visited.end()) {
          visited.emplace_back(edge.first->name);
          queue.push(edge.first);
        }
      }
    }
    for (auto &item : visited) {
      ret.append(item);
    }
    return visit(ret);
  }
}

void Graph::dijkstraPrimHelper(Vertex *curr,
                               map<Vertex *, pair<vector<string>, int>> &holder,
                               int distance, vector<string> path,
                               map<Vertex *, int> &distanceHolder,
                               int pastDistance) const {
  if (holder.count(curr) == 0 || holder.at(curr).second > distance) {
    path.emplace_back(curr->name);
    if (holder.count(curr) == 0) {
      holder.emplace(curr, make_pair(path, distance));
    } else {
      holder.at(curr) = make_pair(path, distance);
    }
    if (distanceHolder.count(curr) == 0) {
      distanceHolder.emplace(curr, pastDistance);
    } else {
      distanceHolder.at(curr) = pastDistance;
    }
    for (auto &edge : curr->edges) {
      dijkstraPrimHelper(edge.first, holder, distance + edge.second, path,
                         distanceHolder, edge.second);
    }
  }
}

// store the weights in a map
// store the previous label in a map
pair<map<string, int>, map<string, string>>
Graph::dijkstra(const string &startLabel) const {
  map<string, int> weights;
  map<string, string> previous;
  if (this->contains(startLabel)) {
    vector<string> starter;
    map<Vertex *, pair<vector<string>, int>> holder;
    map<Vertex *, int> temp;
    dijkstraPrimHelper(innerGraph[getIndex("", startLabel)], holder, 0, starter,
                       temp, 0);
    holder.erase(innerGraph.at(getIndex("", startLabel)));
    while (!holder.empty()) {
      int small = INT_MAX;
      int smallIndex = 0;
      for (int i = 0; i < innerGraph.size(); i++) {
        if (holder.count(innerGraph[i]) != 0 &&
            holder.at(innerGraph[i]).second < small) {
          small = holder.at(innerGraph[i]).second;
          smallIndex = i;
        }
      }
      weights.emplace(innerGraph[smallIndex]->name, small);
      previous.emplace(
          innerGraph[smallIndex]->name,
          holder.at(innerGraph[smallIndex])
              .first.at(holder.at(innerGraph[smallIndex]).first.size() - 2));
      holder.erase(innerGraph[smallIndex]);
    }
    return make_pair(weights, previous);
  }
  return make_pair(weights, previous);
}

// minimum spanning tree using Prim's algorithm. dijkstraPrimHelper gets
// the lowest path to every single vertex.
int Graph::mstPrim(const string &startLabel,
                   void visit(const string &from, const string &to,
                              int weight)) const {
  if (this->contains(startLabel)) {
    int ret{0};
    vector<string> starter;
    map<Vertex *, pair<vector<string>, int>> holder;
    map<Vertex *, int> dists;
    dijkstraPrimHelper(innerGraph[getIndex("", startLabel)], holder, 0, starter,
                       dists, 0);
    while (!holder.empty()) {
      int small = INT_MAX;
      int smallIndex = 0;
      for (int i = 0; i < innerGraph.size(); i++) {
        if (holder.count(innerGraph[i]) != 0 &&
            holder.at(innerGraph[i]).second < small) {
          small = holder.at(innerGraph[i]).second;
          smallIndex = i;
        }
      }
      if (holder.at(innerGraph[smallIndex]).first.size() > 1) {
        visit(holder.at(innerGraph[smallIndex])
                  .first.at(holder.at(innerGraph[smallIndex]).first.size() - 2),
              holder.at(innerGraph[smallIndex])
                  .first.at(holder.at(innerGraph[smallIndex]).first.size() - 1),
              dists.at(innerGraph[smallIndex]));
        ret += dists.at(innerGraph[smallIndex]);
      }
      holder.erase(innerGraph[smallIndex]);
    }
    return ret;
  }
  return -1;
}

// BONUS: minimum spanning tree using Kruskal's algorithm
int Graph::mstKruskal(const string &startLabel,
                      void visit(const string &from, const string &to,
                                 int weight)) const {
  vector<Vertex *> visited;
  int ret{0};
  int shouldReturn{0};
  while (visited.size() < innerGraph.size() && shouldReturn < 100) {
    int outerSmallIndex{0};
    int innerSmallIndex{0};
    int small{INT_MAX};
    for (int i = 0; i < innerGraph.size(); i++) {
      for (int j = 0; j < innerGraph[i]->edges.size(); j++) {
        if (innerGraph[i]->edges[j].second < small &&
            count(visited.begin(), visited.end(),
                  innerGraph[i]->edges[j].first) == 0) {
          shouldReturn = 0;
          outerSmallIndex = i;
          innerSmallIndex = j;
          small = innerGraph[i]->edges[j].second;
        }
      }
    }
    if (count(visited.begin(), visited.end(),
              innerGraph[outerSmallIndex]->edges[innerSmallIndex].first) == 0) {
      visit(innerGraph[outerSmallIndex]->name,
            innerGraph[outerSmallIndex]->edges[innerSmallIndex].first->name,
            small);
      visited.push_back(
          innerGraph[outerSmallIndex]->edges[innerSmallIndex].first);
      ret += small;
      if (count(visited.begin(), visited.end(), innerGraph[outerSmallIndex]) ==
          0) {
        visited.push_back(innerGraph[outerSmallIndex]);
      }
    }
    shouldReturn++;
  }
  return ret;
}

// read a text file and create the graph
bool Graph::readFile(const string &filename) {
  ifstream myfile(filename);
  if (!myfile.is_open()) {
    cerr << "Failed to open " << filename << endl;
    return false;
  }
  int edges = 0;
  int weight = 0;
  string fromVertex;
  string toVertex;
  myfile >> edges;
  for (int i = 0; i < edges; ++i) {
    myfile >> fromVertex >> toVertex >> weight;
    connect(fromVertex, toVertex, weight);
  }
  myfile.close();
  return true;
}
