#ifndef ALIASES_H
#define ALIASES_H

#include <array>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#define SELF (*this)
#define NULLPTR (nullptr)

#define createNew std::make_unique

using string = std::string;

template<typename T> using set = std::set<T>;
template<typename T> using list = std::list<T>;
template<typename T> using queue = std::queue<T>;
template<typename T> using stack = std::stack<T>;
template<typename T> using vector = std::vector<T>;

template<typename T, int S> using array = std::array<T, S>;

template<typename T1, typename T2> using map = std::map<T1, T2>;
template<typename T1, typename T2> using unordered_map = std::unordered_map<T1, T2>;

template<typename T> using pointer = std::unique_ptr<T>;
template<typename T> using reference = std::reference_wrapper<T>;

#endif