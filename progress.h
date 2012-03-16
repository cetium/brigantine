// Andrew Naplavkov

#ifndef PROGRESS_H
#define PROGRESS_H

#include <string>
#include <vector>

struct progress {
  virtual void init(const std::vector<std::string>& columns = std::vector<std::string>()) = 0;
  virtual bool step(size_t counter = 0, const std::vector<std::string>& row = std::vector<std::string>()) = 0;
}; // progress

#endif // PROGRESS_H
