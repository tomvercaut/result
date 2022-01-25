#ifndef RESULT_RESULT_HPP
#define RESULT_RESULT_HPP

#include <compare>
#include <iostream>
#include <optional>
#include <type_traits>

namespace result {

template <typename T, typename E> class result;

template <typename T> class ok;

template <typename E> class err;

struct ok_tag_t {};
struct err_tag_t {};
struct empty_tag_t {};

constexpr auto operator<=>(const ok_tag_t &, const ok_tag_t &) {
  return std::strong_ordering::equal;
}
constexpr auto operator<=>(const ok_tag_t &, const empty_tag_t &) {
  return std::strong_ordering::greater;
}
constexpr auto operator<=>(const ok_tag_t &, const err_tag_t &) {
  return std::strong_ordering::greater;
}
constexpr auto operator<=>(const err_tag_t &, const err_tag_t &) {
  return std::strong_ordering::equal;
}
constexpr auto operator<=>(const err_tag_t &, const ok_tag_t &) {
  return std::strong_ordering::less;
}
constexpr auto operator<=>(const err_tag_t &, const empty_tag_t &) {
  return std::strong_ordering::less;
}
constexpr auto operator<=>(const empty_tag_t &, const empty_tag_t &) {
  return std::strong_ordering::equal;
}
constexpr auto operator<=>(const empty_tag_t &, const ok_tag_t &) {
  return std::strong_ordering::less;
}
constexpr auto operator<=>(const empty_tag_t &, const err_tag_t &) {
  return std::strong_ordering::greater;
}

constexpr ok_tag_t ok_tag = ok_tag_t{};
constexpr err_tag_t err_tag = err_tag_t{};
constexpr empty_tag_t empty_tag = empty_tag_t{};

constexpr bool operator==(empty_tag_t, empty_tag_t) { return true; }
constexpr bool operator!=(empty_tag_t, empty_tag_t) { return false; }

enum class result_type { ok, err };

// constexpr bool operator==(const result_type& lhs,const result_type& rhs ) {
//   if ((lhs == result_type::ok && rhs == result_type::ok) &&
//       (lhs == result_type::err && rhs == result_type::err)) return true;
//  return false;
// }

template <typename T> struct [[maybe_unused]] is_result : std::false_type {};

template <typename T, typename E>
struct [[maybe_unused]] is_result<result<T, E>> : std::true_type {};

template <typename T> class ok {
public:
  using value_type [[maybe_unused]] = T;

  explicit constexpr ok(const T &value) : m_value(value) {}
  explicit constexpr ok(T &&value) : m_value(std::move(value)) {}

  constexpr const T &value() const & { return m_value; }
  [[maybe_unused]] constexpr T &&value() && { return std::move(m_value); }

  template <typename E> constexpr operator result<T, E>() const & {
    return result<T, E>(oke(m_value));
  }

  template <typename E> constexpr operator result<T, E>() && {
    return result<T, E>(ok(std::move(m_value)));
  }

private:
  T m_value;
};

template <> class ok<empty_tag_t> {
public:
  using value_type [[maybe_unused]] = empty_tag_t;

  constexpr ok() = default;
  [[maybe_unused]] constexpr ok(empty_tag_t) {}
  [[maybe_unused]] constexpr const empty_tag_t value() const noexcept {
    return {};
  }
};

ok() -> ok<empty_tag_t>;

template <typename T> class err {
public:
  using value_type [[maybe_unused]] = T;

  explicit constexpr err(const T &value) : m_value(value) {}
  explicit constexpr err(T &&value) : m_value(std::move(value)) {}

  constexpr const T &value() const & { return m_value; }
  [[maybe_unused]] constexpr T &&value() && { return std::move(m_value); }

private:
  T m_value;
};

template <> class err<empty_tag_t> {
public:
  using value_type [[maybe_unused]] = empty_tag_t;

  constexpr err() = default;
  [[maybe_unused]] constexpr err(empty_tag_t) {}
  [[maybe_unused]] constexpr const empty_tag_t value() const noexcept {
    return {};
  }
};

namespace details {
inline void terminate(const std::string_view &msg) {
  std::cerr << msg << std::endl;
  std::terminate();
}

} // namespace details

/// result is a type to represent either a value (ok) or failure (err).
/// \tparam T value type
/// \tparam E error type
template <typename T, typename E> class result {

public:
  using value_type [[maybe_unused]] = T;
  using error_type [[maybe_unused]] = E;
  using data_type = std::aligned_union_t<1, T, E>;

  static_assert(std::is_same<std::remove_reference_t<T>, T>::value,
                "result<T, E> can't store reference types. "
                "Try using `std::reference_wrapper`");
  static_assert(std::is_same<std::remove_reference_t<E>, E>::value,
                "result<T, E> can't store reference types. "
                "Try using `std::reference_wrapper`");
  static_assert(!std::is_same<T, void>::value,
                "result<T, E> can't be created with T=void. "
                "Try replacing T with `empty_tag_t`");
  static_assert(!std::is_same<E, void>::value,
                "result<T, E> can't be created with E=void. "
                "Try replacing E with `empty_tag_t`");

  friend class ok<T>;
  friend class err<E>;

  constexpr result() {
    static_assert(std::is_default_constructible<T>::value,
                  "result<T, E> can only be default constructed if T "
                  "is default constructible.");
  }

  constexpr result(ok<T> value) {
    if constexpr (!std::is_same_v<T, empty_tag_t>) {
      new (std::addressof(m_data)) std::decay_t<T>(std::move(value).value());
    }
    m_type = result_type::ok;
  }

  constexpr result(err<E> value) {
    if constexpr (!std::is_same_v<E, empty_tag_t>) {
      new (std::addressof(m_data)) std::decay_t<E>(std::move(value).value());
    }
    m_type = result_type::err;
  }

  template <typename... Args> constexpr result(ok_tag_t, Args &&...args) {
    if constexpr (!std::is_same_v<T, empty_tag_t>) {
      new (std::addressof(m_data)) std::decay_t<T>(std::forward<Args>(args)...);
    }
    m_type = result_type::ok;
  }

  template <typename... Args> constexpr result(err_tag_t, Args &&...args) {
    if constexpr (!std::is_same_v<E, empty_tag_t>) {
      new (std::addressof(m_data)) std::decay_t<E>(std::forward<Args>(args)...);
    }
    m_type = result_type::err;
  }

  constexpr result(const result<T, E> &other) noexcept(
      std::is_nothrow_copy_assignable_v<T>
          &&std::is_nothrow_copy_assignable_v<E>) {
    m_type = other.m_type;
    if (m_type == result_type::ok) {
      new (std::addressof(m_data)) std::decay_t<T>(other.template get<T>());
    } else {
      new (std::addressof(m_data)) std::decay_t<E>(other.template get<E>());
    }
  }

  constexpr result(result<T, E> &&other) noexcept(
      std::is_nothrow_move_assignable_v<T>
          &&std::is_nothrow_move_assignable_v<E>) {
    m_type = other.m_type;
    if (m_type == result_type::ok) {
      new (std::addressof(m_data))
          std::decay_t<T>(std::move(other).template get<T>());
    } else {
      new (std::addressof(m_data))
          std::decay_t<E>(std::move(other).template get<E>());
    }
  }

  ~result() { destroy(); }

  /// Assign another result to this instance.
  /// If the result type changes, allocate space for the new data type.
  constexpr result<T, E> &operator=(const result<T, E> &rhs) noexcept(
      std::is_nothrow_copy_assignable_v<T>
          &&std::is_nothrow_copy_assignable_v<E>) {
    destroy();
    bool reallocate = m_type != rhs.m_type;
    m_type = rhs.m_type;
    if (reallocate) {
      if (m_type == result_type::ok) {
        new (std::addressof(m_data)) std::decay_t<T>(rhs.template get<T>());
      } else {
        new (std::addressof(m_data)) std::decay_t<E>(rhs.template get<E>());
      }
    } else {
      if (m_type == result_type::ok) {
        T &value = get<T>();
        value = rhs.template get<T>();
      } else {
        E &value = get<E>();
        value = rhs.template get<E>();
      }
    }
    return *this;
  }

  /// Assign an ok value to this instance.
  /// If the result type changes, allocate space for the new data type.
  constexpr result<T, E> &operator=(const ok<T> &rhs) noexcept(
      std::is_nothrow_copy_assignable_v<T>
          &&std::is_nothrow_copy_assignable_v<E>) {
    destroy();
    bool reallocate = m_type != result_type::ok;
    m_type = result_type::ok;
    if (reallocate) {
      new (std::addressof(m_data)) std::decay_t<T>(rhs.value());
    } else {
      T &value = get<T>();
      value = rhs.value();
    }
    return *this;
  }

  /// Assign an ok value to this instance.
  /// If the result type changes, allocate space for the new data type.
  constexpr result<T, E> &
  operator=(ok<T> &&rhs) noexcept(std::is_nothrow_copy_assignable_v<T>
                                      &&std::is_nothrow_copy_assignable_v<E>) {
    destroy();
    bool reallocate = m_type != result_type::ok;
    m_type = result_type::ok;
    if (reallocate) {
      new (std::addressof(m_data)) std::decay_t<T>(std::move(rhs).value());
    } else {
      T &value = get<T>();
      value = std::move(rhs).value();
    }
    return *this;
  }

  /// Assign another result to this instance.
  /// If the result type changes, allocate space for the new data type.
  constexpr result<T, E> &operator=(result<T, E> &&rhs) noexcept(
      std::is_nothrow_move_assignable_v<T>
          &&std::is_nothrow_move_assignable_v<E>) {
    destroy();
    bool reallocate = m_type != rhs.m_type;
    m_type = rhs.m_type;
    if (reallocate) {
      if (m_type == result_type::ok) {
        new (std::addressof(m_data))
            std::decay_t<T>(std::move(rhs).template get<T>());
      } else {
        new (std::addressof(m_data))
            std::decay_t<E>(std::move(rhs).template get<E>());
      }
    } else {
      if (m_type == result_type::ok) {
        T &value = get<T>();
        value = std::move(rhs).template get<T>();
      } else {
        E &value = get<E>();
        value = std::move(rhs).template get<E>();
      }
    }
    return *this;
  }

  /// Assign an err value to this instance.
  /// If the result type changes, allocate space for the new data type.
  constexpr result<T, E> &operator=(const err<E> &rhs) noexcept(
      std::is_nothrow_copy_assignable_v<T>
          &&std::is_nothrow_copy_assignable_v<E>) {
    destroy();
    bool reallocate = m_type != result_type::err;
    m_type = result_type::err;
    if (reallocate) {
      new (std::addressof(m_data)) std::decay_t<E>(rhs.value());
    } else {
      E &value = get<E>();
      value = rhs.value();
    }
    return *this;
  }

  /// Assign an err value to this instance.
  /// If the result type changes, allocate space for the new data type.
  constexpr result<T, E> &
  operator=(err<E> &&rhs) noexcept(std::is_nothrow_copy_assignable_v<T>
                                       &&std::is_nothrow_copy_assignable_v<E>) {
    destroy();
    bool reallocate = m_type != result_type::err;
    m_type = result_type::err;
    if (reallocate) {
      new (std::addressof(m_data)) std::decay_t<E>(std::move(rhs).value());
    } else {
      E &value = get<E>();
      value = std::move(rhs).value();
    }
    return *this;
  }

  constexpr explicit operator bool() const noexcept { return is_ok(); }

  constexpr bool is_ok() const noexcept { return m_type == result_type::ok; }

  constexpr bool is_err() const noexcept { return m_type == result_type::err; }

  constexpr bool operator==(const result<T, E> &rhs) const {
    if (m_type != rhs.m_type) {
      return false;
    }
    if (m_type == result_type::ok) {
      if constexpr (std::is_same_v<T, empty_tag_t>) {
        return true;
      } else {
        return get<T>() == rhs.template get<T>();
      }
    } else {
      if constexpr (std::is_same_v<E, empty_tag_t>) {
        return true;
      } else {
        return get<E>() == rhs.template get<E>();
      }
    }
  }

  constexpr bool operator!=(const result<T, E> &rhs) const {
    return !(*this == rhs);
  }

  constexpr bool operator==(const ok<T> &rhs) const {
    if constexpr (std::is_same_v<T, empty_tag_t>) {
      return true;
    } else {
      return m_type == result_type::ok && get<T>() == rhs.value();
    }
  }

  constexpr bool operator!=(const ok<T> &rhs) const { return !(*this == rhs); }

  constexpr bool operator==(const err<E> &rhs) const {
    if constexpr (std::is_same_v<E, empty_tag_t>) {
      return true;
    } else {
      return m_type == result_type::err && get<E>() == rhs.value();
    }
  }

  constexpr bool operator!=(const err<E> &rhs) const { return !(*this == rhs); }

  bool contains(T rhs) const {
    if (is_ok()) {
      const auto &t = ok_unchecked();
      return t == rhs;
    }
    return false;
  }

  bool contains_err(E rhs) const {
    if (is_err()) {
      const auto &t = err_unchecked();
      return t == rhs;
    }
    return false;
  }

  [[maybe_unused]] constexpr std::optional<std::reference_wrapper<const T>>
  ok() const & {
    if (is_ok()) {
      return std::cref(ok_unchecked());
    }
    return std::nullopt;
  }

  [[maybe_unused]] constexpr std::optional<std::reference_wrapper<T>> ok() & {
    if (is_ok()) {
      return std::ref(ok_unchecked());
    }
    return std::nullopt;
  }
  [[maybe_unused]] constexpr std::optional<T> ok() && {
    if (is_ok()) {
      return ok_unchecked();
    }
    return std::nullopt;
  }

  [[maybe_unused]] constexpr std::optional<std::reference_wrapper<const E>>
  err() const & {
    if (is_err()) {
      return std::cref(err_unchecked());
    }
    return std::nullopt;
  };

  [[maybe_unused]] constexpr std::optional<std::reference_wrapper<E>> err() & {
    if (is_err()) {
      return std::ref(err_unchecked());
    }
    return std::nullopt;
  };

  [[maybe_unused]] constexpr std::optional<E> err() && {
    if (is_err()) {
      return err_unchecked();
    }
    return std::nullopt;
  };

  constexpr const T &ok_unchecked() const &noexcept { return get<T>(); }

  [[maybe_unused]] constexpr const E &err_unchecked() const &noexcept {
    return get<E>();
  }

  constexpr T &ok_unchecked() &noexcept { return get<T>(); }

  [[maybe_unused]] constexpr E &err_unchecked() &noexcept { return get<E>(); }

  constexpr T &&ok_unchecked() &&noexcept { return std::move(get<T>()); }

  constexpr E &&err_unchecked() &&noexcept { return std::move(get<E>()); }

  [[maybe_unused]] constexpr const T &try_ok() const {
    if (!is_ok()) {
      details::terminate("try_ok() was called on an err result.");
    }
    return ok_unchecked();
  }

  [[maybe_unused]] constexpr T &try_ok() {
    if (!is_ok()) {
      details::terminate("try_ok() was called on an err result.");
    }
    return ok_unchecked();
  }

  [[maybe_unused]] constexpr const E &try_err() const {
    if (!is_err()) {
      details::terminate("try_err() was called on an ok result.");
    }
    return err_unchecked();
  }

  [[maybe_unused]] constexpr E &try_err() {
    if (!is_err()) {
      details::terminate("try_err() was called on an ok result.");
    }
    return err_unchecked();
  }

  [[maybe_unused]] constexpr T &&expect(const std::string_view &msg) {
    if (is_err()) {
      details::terminate(msg);
    }
    return std::move(unwrap());
  }

  [[maybe_unused]] constexpr T &&expect_err(const std::string_view &msg) {
    if (is_ok()) {
      details::terminate(msg);
    }
    return std::move(unwrap_err());
  }

  [[maybe_unused]] constexpr T &&unwrap() {
    if (!is_ok()) {
      details::terminate("unwrap() was called on an err result.");
    }
    return std::move(*this).ok_unchecked();
  }

  constexpr T &&unwrap_or(T &&default_value) {
    if (!is_ok()) {
      return std::move(default_value);
    }
    return std::move(*this).ok_unchecked();
  }

  [[maybe_unused]] constexpr T &&unwrap_or_default() {
    static_assert(std::is_default_constructible_v<T>,
                  "result<T, E>::unwrap_or_default() requires T to be default "
                  "constructible");
    return std::move(unwrap_or(T()));
  }

  [[maybe_unused]] constexpr E &&unwrap_err() {
    if (!is_err()) {
      details::terminate("unwrap_err() was called on an ok result.");
    }
    return std::move(*this).err_unchecked();
  }

  [[maybe_unused]] constexpr E &&unwrap_err_or_default() {
    if (!is_err()) {
      return std::move(E());
    }
    return std::move(*this).err_unchecked();
  }

  template <typename F, typename R = std::invoke_result_t<F, T>,
            std::enable_if_t<std::is_invocable_r<R, F, T>::value, int> = 0>
  result<R, E> map(F &&fun) {
    if (is_ok()) {
      return result<R, E>(::result::ok(fun(std::move(*this).ok_unchecked())));
    }
    return result<R, E>(::result::err(std::move(*this).err_unchecked()));
  }

  template <typename F, typename R = std::invoke_result_t<F, E>,
            std::enable_if_t<std::is_invocable_r<R, F, E>::value, int> = 0>
  result<T, R> map_err(F &&fun) {
    if (is_ok()) {
      return result<T, R>(::result::ok(std::move(*this).ok_unchecked()));
    }
    return result<T, R>(::result::err(fun(std::move(*this).err_unchecked())));
  }

  template <typename F, typename D, typename R = std::invoke_result_t<F, T>,
            typename R2 = std::invoke_result_t<D, E>,
            std::enable_if_t<std::is_invocable_r<R, F, T>::value, int> = 0,
            std::enable_if_t<std::is_invocable_r<R2, D, E>::value, int> = 0,
            std::enable_if_t<std::is_same_v<R, R2>, int> = 0>
  R map_or_else(D default_fun, F &&fun) {
    if (is_ok()) {
      return fun(std::move(*this).ok_unchecked());
    }
    return default_fun(std::move(*this).err_unchecked());
  }

  template <typename U> result<U, E> and_(result<U, E> r) {
    if (is_ok()) {
      return r;
    }
    return result<U, E>(::result::err(std::move(*this).err_unchecked()));
  }

  template <
      typename F, typename U = typename std::invoke_result_t<F, T>::value_type,
      std::enable_if_t<std::is_invocable_r_v<result<U, E>, F, T>, int> = 0>
  result<U, E> and_then(F fun) {
    if (is_ok()) {
      return fun(std::move(*this).ok_unchecked());
    }
    return result<U, E>(::result::err(std::move(*this).err_unchecked()));
  }

#if defined(__clang__)
#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-identifier-naming"
#endif
  template <typename E2> [[maybe_unused]] result<T, E2> or_(result<T, E2> r) {
    if (is_ok()) {
      return result<T, E2>(::result::ok(std::move(*this).ok_unchecked()));
    }
    return r;
  }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

  template <
      typename F, typename E2 = typename std::invoke_result_t<F, E>::error_type,
      std::enable_if_t<std::is_invocable_r_v<result<T, E2>, F, E>, int> = 0>
  result<T, E2> or_else(F fun) {
    if (is_ok()) {
      return result<T, E2>(::result::ok(std::move(*this).ok_unchecked()));
    }
    return fun(std::move(*this).err_unchecked());
  }

private:
  template <typename U> constexpr const U &get() const &noexcept {
    static_assert(std::is_same<T, U>::value || std::is_same<E, U>::value);
    return *reinterpret_cast<const U *>(std::addressof(m_data));
  }

  template <typename U> constexpr U &get() &noexcept {
    static_assert(std::is_same<T, U>::value || std::is_same<E, U>::value);
    return *reinterpret_cast<U *>(std::addressof(m_data));
  }

  template <typename U>
  constexpr U &&get() &&noexcept(std::is_nothrow_move_assignable_v<U>) {
    static_assert(std::is_same<T, U>::value || std::is_same<E, U>::value);
    return std::move(*reinterpret_cast<U *>(std::addressof(m_data)));
  }

  void destroy() {
    if (m_type == result_type::ok) {
      auto *ptr = reinterpret_cast<T *>(std::addressof(m_data));
      if (ptr != nullptr)
        ptr->~T();
    } else {
      auto *ptr = reinterpret_cast<E *>(std::addressof(m_data));
      if (ptr != nullptr)
        ptr->~E();
    }
  }

private:
  data_type m_data;
  result_type m_type;
};

template <typename U, typename Er>
constexpr bool operator<(const result<U, Er> &lhs, const result<U, Er> &rhs) {
  if (lhs.is_ok() && rhs.is_ok()) {
    return lhs.ok_unchecked() < rhs.ok_unchecked();
  }
  if (lhs.is_err() && rhs.is_err()) {
    return lhs.err_unchecked() < rhs.err_unchecked();
  }
  return lhs.is_err() && rhs.is_ok();
}

} // namespace result

namespace std {
template <typename T, typename E> struct hash<::result::result<T, E>> {
  size_t operator()(const ::result::result<T, E> &result) const noexcept {
    bool is_ok = result.is_ok();
    size_t h1 = is_ok ? 1 : 0;
    size_t h2;
    if (result.is_ok()) {
      auto value = result.ok();
      if (value) {
        h2 = std::hash<T>{}(value.value().get());
      }
    } else {
      auto value = result.err();
      if (value) {
        h2 = std::hash<E>{}(value.value().get());
      }
    }
    return h1 ^ (h2 << 1);
  }
};
} // namespace std

#endif // RESULT_RESULT_HPP
