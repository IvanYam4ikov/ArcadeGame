#include "ScoreStore.h"

#include <algorithm>
#include <fstream>

namespace {
const std::size_t maximumScores = 10;

std::string fileFor(const std::string& gameId)
{
    if (gameId == "tetris") return "highscores_tetris.txt";
    if (gameId == "frogger") return "highscores_frogger.txt";
    return "highscores_brick_breaker.txt";
}
}

namespace ScoreStore {
std::vector<int> load(const std::string& gameId)
{
    std::ifstream input(fileFor(gameId).c_str());
    // Preserve scores written by the earlier single-leaderboard version.
    if (!input.is_open() && gameId == "brick_breaker") {
        input.clear();
        input.open("breakout_highscores.txt");
    }
    std::vector<int> scores;
    int score = 0;
    while (input >> score) {
        if (score >= 0) scores.push_back(score);
    }
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    if (scores.size() > maximumScores) scores.resize(maximumScores);
    return scores;
}

void submit(const std::string& gameId, int score)
{
    if (score <= 0) return;
    std::vector<int> scores = load(gameId);
    scores.push_back(score);
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    if (scores.size() > maximumScores) scores.resize(maximumScores);

    std::ofstream output(fileFor(gameId).c_str(), std::ios::trunc);
    for (std::vector<int>::const_iterator value = scores.begin(); value != scores.end(); ++value) {
        output << *value << '\n';
    }
}
}
