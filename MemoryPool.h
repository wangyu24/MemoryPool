#pragma once

#include <algorithm>
#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace memorypool {

template <typename T> struct type_wrapper { using type = T; };

template <typename... Ts, typename TF> void for_types(TF &&f) {
  (f(type_wrapper<Ts>{}), ...);
}

template <typename T, typename Tuple> struct TupleIndex;

template <typename T, typename... Types>
struct TupleIndex<T, std::tuple<T, Types...>> {
  static constexpr const std::size_t value = 0;
};

template <typename T, typename U, typename... Types>
struct TupleIndex<T, std::tuple<U, Types...>> {
  static constexpr const std::size_t value =
      1 + TupleIndex<T, std::tuple<Types...>>::value;
};

template <typename T> class Pool {
public:
  Pool(unsigned int n) : mutex_m(), free_m(0), used_m(0) {
    for (unsigned int i = 0; i < n; ++i) {
      free_m.push_front(std::make_unique<T>());
    }
  }

  Pool(const Pool &other) = delete;
  Pool(Pool &&r) {
    free_m = std::move(r.free_m);
    used_m = std::move(r.used_m);
  }
  Pool &operator=(const Pool &other) = delete;
  Pool &operator=(Pool &&r) {
    free_m = std::move(r.free_m);
    used_m = std::move(r.used_m);
  }

  std::shared_ptr<T> alloc() {
    std::unique_lock<std::mutex> lock(mutex_m);
    if (free_m.empty()) {
      free_m.push_front(std::make_unique<T>());
    }
    auto it = free_m.begin();
    std::shared_ptr<T> sptr(it->get(), [=](T *ptr) { this->free(ptr); });
    used_m.push_front(std::move(*it));
    free_m.erase(it);
    return sptr;
  }

  size_t getFreeCount() {
    std::unique_lock<std::mutex> lock(mutex_m);
    return free_m.size();
  }

private:
  void free(T *obj) {
    std::unique_lock<std::mutex> lock(mutex_m);
    auto it =
        std::find_if(used_m.begin(), used_m.end(),
                     [&](std::unique_ptr<T> &p) { return p.get() == obj; });
    if (it != used_m.end()) {
      free_m.push_back(std::move(*it));
      used_m.erase(it);
    } else {
      throw std::runtime_error("unexpected: unknown object freed.");
    }
  }

  std::mutex mutex_m;
  std::list<std::unique_ptr<T>> free_m;
  std::list<std::unique_ptr<T>> used_m;
};

template <typename... Types> class MemoryPoolManager {
public:
  static MemoryPoolManager &Instance() {
    static MemoryPoolManager manager;
    return manager;
  }

  template <typename T> std::shared_ptr<T> alloc() {
    return std::get<Pool<T>>(
               pool_vec[TupleIndex<T, std::tuple<Types...>>::value])
        .alloc();
  }

private:
  MemoryPoolManager() {
    for_types<Types...>([&](auto t) {
      using t_type = typename decltype(t)::type;
      pool_vec.emplace_back(Pool<t_type>(PREALLOCATED_ITEM_NUM));
    });
  }
  ~MemoryPoolManager() = default;
  MemoryPoolManager(const MemoryPoolManager &) = delete;
  MemoryPoolManager &operator=(const MemoryPoolManager &) = delete;

  std::vector<std::variant<Pool<Types>...>> pool_vec;
  constexpr const static unsigned int PREALLOCATED_ITEM_NUM = 1000;
};

} // namespace memorypool
