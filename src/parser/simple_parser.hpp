#ifndef SIMPLE_PARSER_HPP
#define SIMPLE_PARSER_HPP

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace simple_parser {

using Delimiters = std::string;
using Token = std::string;
using Tokens = std::vector<Token>;
using Handler = std::function<bool(const Tokens&)>;

class Parser {
 public:
  Parser(std::istream& input, const Delimiters& extra_delimiters = "")
      : input_(input), delimiters_(" \t\r\n" + extra_delimiters) {}

  // Because there is a member variable of reference type, having a copy
  // assignment operator makes no sense.
  Parser& operator=(const Parser& other) = delete;

  void Parse(Handler handler) {
    std::string line;
    while (std::getline(input_, line)) {
      Tokens tokens = Tokenize(line);
      bool is_ending_condition_met = !handler(tokens);
      if (is_ending_condition_met) {
        break;
      }
    }
  }

 private:
  bool IsDelimiter(char c) { return delimiters_.find(c) != std::string::npos; }

  Tokens Tokenize(const std::string& str) {
    Tokens tokens;

    Token token;
    for (const char c : str) {
      if (IsDelimiter(c)) {
        if (!token.empty()) {
          tokens.push_back(token);
          token.clear();
        }
        continue;
      }
      token.push_back(c);
    }
    if (!token.empty()) {
      tokens.push_back(token);
    }

    return tokens;
  }

  std::istream& input_;
  Delimiters delimiters_;
};
}

#endif
