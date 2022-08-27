#ifndef FDN__MEMORY__STORAGE_HPP_
#define FDN__MEMORY__STORAGE_HPP_

#include <cstddef>

#include <new>
#include <type_traits>
#include <utility>

namespace fdn {
/**
 * A type suitable for use as uninitialized storage for an object of type @p T.
 *
 * The @c Storage class is a trivial standard-layout type and can be used to
 * create uninitialized memory blocks suitable to hold the objects of the given
 * type. Like other uninitialized storage utilities, the objects must be
 * constructed and destructed explicitly. However, unlike the utilities of the
 * standard library, this @c Storage class provides type-restricted member
 * functions for those operations.
 *
 * @warning Unlike @c std::optional, this class does not keep track of whether
 *     the wrapped object has been initialized or not.
 *
 * @tparam T The type to create storage for.
 */
template <typename T>
class Storage {
  public:
    /**
     * Access the stored object.
     * @warning It is undefined behavior if an object has not yet been
     *     constructed in this @c Storage.
     * @return A reference to the object.
     */
    constexpr T& object () & noexcept {
        return *std::launder(reinterpret_cast<T*>(this));
    }

    /**
     * @overload Storage::object()
     * @return A const reference to the object.
     */
    constexpr T const& object () const& noexcept {
        return *std::launder(reinterpret_cast<T const*>(this));
    }

    /**
     * @overload Storage::object()
     * @return An rvalue reference to the object.
     */
    constexpr T&& object () && noexcept {
        return std::move(*std::launder(reinterpret_cast<T*>(this)));
    }

    /**
     * Creates a @p T object initialized with @p args... in the storage.
     *
     * When @c construct is called in the evaluation of some constant
     * expression @c e, the lifetime of the @c Storage object must have begun
     * within the evaluation of @c e.
     *
     * @warning Does not destruct any existing object in the storage.
     *
     * @param[in] args... Arguments used for initialization.
     */
    template <typename... Args>
    constexpr void construct (Args&&... args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>) {
        ::new((void*)data_) T{std::forward<Args>(args)...};
    }

    /**
     * Calls the destructor of the object in storage.
     *
     * When called in the evaluation of some constant expression @c e, the
     * lifetime of the storage object must have begun within the evaluation
     * of @c e.
     *
     * @warning It is undefined behavior to call this function if there is no
     *     object constructed in the storage.
     */
    constexpr void destroy () noexcept {
        object().~T();
    }

  private:
    alignas(T) std::byte data_[sizeof(T)];
};
}  // fdn

#endif  // FDN__MEMORY__STORAGE_HPP_
