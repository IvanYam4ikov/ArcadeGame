#ifndef SCORE_STORE_H
#define SCORE_STORE_H

#include <string>
#include <vector>

namespace ScoreStore {
std::vector<int> load(const std::string& gameId);
void submit(const std::string& gameId, int score);
}

#endif
