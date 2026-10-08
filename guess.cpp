#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <random>
#include <string>

struct Level {
    std::string name;
    int chances;
};

const Level LEVELS[] = {{"Easy", 10}, {"Medium", 5}, {"Hard", 3}};
const int MIN_NUMBER = 1;
const int MAX_NUMBER = 100;
const char* SCORE_FILE = "highscores.txt";

using Scores = std::map<std::string, int>;

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string plural(int n, const std::string& word) {
    return std::to_string(n) + " " + word + (n == 1 ? "" : "s");
}

// Returns nothing when stdin is closed (Ctrl+D)
std::optional<std::string> ask(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) return std::nullopt;
    return trim(line);
}

std::optional<int> toInt(const std::string& s) {
    try {
        size_t used = 0;
        int n = std::stoi(s, &used);
        if (used != s.size()) return std::nullopt;
        return n;
    } catch (...) {
        return std::nullopt;
    }
}

Scores loadScores() {
    Scores scores;
    std::ifstream in(SCORE_FILE);
    std::string name;
    int attempts;
    while (in >> name >> attempts) {
        if (attempts > 0) scores[name] = attempts;
    }
    return scores;
}

void saveScores(const Scores& scores) {
    std::ofstream out(SCORE_FILE);
    for (const auto& [name, attempts] : scores) {
        out << name << " " << attempts << "\n";
    }
    if (!out) std::cerr << "Couldn't save your high score to " << SCORE_FILE << ".\n";
}

std::string hint(int number, int given, std::mt19937& rng) {
    switch (given) {
    case 0:
        return number % 2 == 0 ? "The number is even." : "The number is odd.";
    case 1: {
        int offset = std::uniform_int_distribution<int>(0, 19)(rng);
        int low = std::max(MIN_NUMBER, std::min(number - offset, MAX_NUMBER - 19));
        return "The number is between " + std::to_string(low) + " and " +
               std::to_string(low + 19) + ".";
    }
    default:
        return number % 5 == 0 ? "The number is a multiple of 5."
                               : "The number is not a multiple of 5.";
    }
}

void welcome() {
    std::cout << "Welcome to the Number Guessing Game!\n"
              << "I'm thinking of a number between " << MIN_NUMBER << " and " << MAX_NUMBER << ".\n"
              << "Guess it before you run out of chances. After each wrong guess I'll tell you\n"
              << "whether the number is greater or less. Type 'hint' if you get stuck.\n\n";
}

std::optional<int> chooseLevel() {
    std::cout << "Please select the difficulty level:\n";
    for (int i = 0; i < 3; i++) {
        std::cout << i + 1 << ". " << LEVELS[i].name << " (" << LEVELS[i].chances << " chances)\n";
    }
    while (true) {
        auto input = ask("\nEnter your choice: ");
        if (!input) return std::nullopt;

        auto choice = toInt(*input);
        if (choice && *choice >= 1 && *choice <= 3) return *choice - 1;
        for (int i = 0; i < 3; i++) {
            if (lower(*input) == lower(LEVELS[i].name)) return i;
        }
        std::cout << "Please enter 1, 2 or 3.\n";
    }
}

// Returns false if the player's input ended mid-game
bool playRound(const Level& level, Scores& best, std::mt19937& rng) {
    int secret = std::uniform_int_distribution<int>(MIN_NUMBER, MAX_NUMBER)(rng);

    std::cout << "\nGreat! You have selected the " << level.name << " difficulty level.\n"
              << "You have " << level.chances << " chances to guess the correct number.\n";
    if (best.count(level.name)) {
        std::cout << "Your best on " << level.name << " is " << plural(best[level.name], "attempt") << ".\n";
    }
    std::cout << "Let's start the game!\n\n";

    auto start = std::chrono::steady_clock::now();
    int attempts = 0;
    int hintsGiven = 0;

    while (attempts < level.chances) {
        auto input = ask("Enter your guess: ");
        if (!input) return false;

        if (lower(*input) == "hint") {
            if (hintsGiven >= 3) {
                std::cout << "No more hints left this round.\n";
            } else {
                std::cout << hint(secret, hintsGiven++, rng) << "\n";
            }
            continue;
        }

        auto guess = toInt(*input);
        if (!guess || *guess < MIN_NUMBER || *guess > MAX_NUMBER) {
            std::cout << "Please enter a whole number between " << MIN_NUMBER << " and " << MAX_NUMBER << ".\n";
            continue;
        }

        attempts++;
        if (*guess == secret) {
            std::chrono::duration<double> taken = std::chrono::steady_clock::now() - start;
            std::cout << "Congratulations! You guessed the correct number in "
                      << plural(attempts, "attempt") << ".\n"
                      << "It took you " << std::fixed << std::setprecision(1) << taken.count() << " seconds.\n";

            if (hintsGiven > 0) {
                std::cout << "No high score recorded, since you used hints.\n";
            } else if (!best.count(level.name) || attempts < best[level.name]) {
                best[level.name] = attempts;
                saveScores(best);
                std::cout << "New high score for " << level.name << "!\n";
            }
            return true;
        }

        std::cout << "Incorrect! The number is " << (secret < *guess ? "less" : "greater")
                  << " than " << *guess << ".\n";
    }

    std::cout << "Sorry, you're out of chances. The number was " << secret << ".\n";
    return true;
}

bool playAgain() {
    while (true) {
        auto input = ask("\nPlay again? (y/n): ");
        if (!input) return false;
        std::string answer = lower(*input);
        if (answer == "y" || answer == "yes") {
            std::cout << "\n";
            return true;
        }
        if (answer == "n" || answer == "no") return false;
        std::cout << "Please answer y or n.\n";
    }
}

int main() {
    std::mt19937 rng{std::random_device{}()};
    Scores best = loadScores();

    welcome();
    do {
        auto level = chooseLevel();
        if (!level) break;
        if (!playRound(LEVELS[*level], best, rng)) break;
    } while (playAgain());

    std::cout << "\nThanks for playing!\n";
    return 0;
}
