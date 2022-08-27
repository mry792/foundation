#ifndef FDN__TESTING__REGULAR_OBJECT_HPP_
#define FDN__TESTING__REGULAR_OBJECT_HPP_

#include <cstddef>

#include <compare>
#include <concepts>
#include <memory>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>
#include <trompeloeil.hpp>

namespace fdn::testing {
/**
 * Type used as a stand-in for a [C++ Concept] "regular" type.
 *
 * This type is inteded to be used to test templates with very generic
 * requirements, such as containers. It conforms to the @c std::regular
 * concept. A regular type is one that can be copied, moved, swapped, default
 * constructed, and is equality comparable.
 *
 * Each instance has two member fields: (1) a unique ID assigned to each
 * instance and (2) an @c int representing the "data identity" of the object.
 * The data field is copied in copy/move constructors, changed through object
 * assignment, and swapped in swap operations. Two instances are semantically
 * equal (re. `operator==`) if they have the same "data". The ID field on the
 * other hand, remains constant for the lifetime of the object.
 *
 * The class must be initialized at the start of each test. The @c initialize
 * function returns a @c Mock object that is used to track constructor and
 * other function calls. See below for more details.
 */
class Regular_Object {
  public:
    /**
     * Mock type used to track function calls of all instances of @c
     * Regular_Object.
     */
    struct Mock {
        MAKE_MOCK2(default_construct, void(int, int));
        MAKE_MOCK2(copy_construct, void(int, int));
        MAKE_MOCK2(move_construct, void(int, int));
        MAKE_MOCK2(value_construct, void(int, int));
        MAKE_MOCK4(copy_assign, void(int, int, int, int));
        MAKE_MOCK4(move_assign, void(int, int, int, int));
        MAKE_MOCK2(destruct, void(int, int));
    };

    /**
     * Set up the class for use in a test.
     *
     * Must be called at the start of each test case and does the following:
     *   - reset the ID sequence
     *   - create a @c Mock object
     *
     * @note The class holds onto a reference to the @c Mock object via a @c
     *     std::weak_ptr. The caller must hold onto the returned @c
     *     std::shared_ptr for as long as they want the @c Mock object to live.
     *
     * @returns A shared pointer that is the sole owner of a new @c Mock
     *     object.
     */
    static std::shared_ptr<Mock> initialize () {
        next_id_ = 0;
        auto mock_obj = std::make_shared<Mock>();
        mock_obj_weak_ = mock_obj;
        return mock_obj;
    }

    /**
     * Default construct a new object. The data value is equal to the new ID.
     */
    Regular_Object () : id_{next_id_++}, data_{id_} {
        mock_obj_()->default_construct(id_, data_);
    }

    /**
     * Copy construct a new object. The new object will have it's own ID but
     * will share the same data value as @p other_obj.
     */
    Regular_Object (Regular_Object const& other_obj)
      : id_{next_id_++},
        data_{other_obj.data_}
    {
        mock_obj_()->copy_construct(id_, data_);
    }

    /**
     * "Move" construct a new object. The new object will have it's own ID but
     * will share the same data value as @p other_obj.
     */
    Regular_Object (Regular_Object&& other_obj) noexcept
      : id_{next_id_++},
        data_{other_obj.data_}
    {
        mock_obj_()->move_construct(id_, data_);
    }

    /**
     * Construct a new object using the given value. The new object will have
     * it's own ID independent of @p data.
     */
    explicit Regular_Object (int data) noexcept
      : id_{next_id_++},
        data_{data}
    {
        mock_obj_()->value_construct(id_, data_);
    }

    /**
     * "Destroy" the object. Nothing is actually cleaned up. This just logs the
     *  destructor call with the static @c Mock object.
     */
    ~Regular_Object () {
        mock_obj_()->destruct(id_, data_);
    }

    /**
     * Copy assign from @p other_obj. This object will take the data value of @p
     * other_obj but retain it's own ID.
     */
    Regular_Object& operator = (Regular_Object const& other_obj) {
        mock_obj_()->copy_assign(id_, data_, other_obj.id_, other_obj.data_);
        data_ = other_obj.data_;

        return *this;
    }

    /**
     * "Move" assign from @p other_obj. This object will take the data value
     * of @p other_obj but retain it's own ID.
     */
    Regular_Object& operator = (Regular_Object&& other_obj) {
        mock_obj_()->move_assign(id_, data_, other_obj.id_, other_obj.data_);
        data_ = other_obj.data_;

        return *this;
    }

    /**
     * The ID of this instance. This is unique among all instances created since
     * the last call to @c initialize().
     */
    int id () const { return id_; }

    /**
     * Value representing the "data identity" of the object.
     */
    int data () const { return data_; }

  private:
    static inline int next_id_ = 0;

    static inline std::weak_ptr<Mock> mock_obj_weak_;

    int id_;
    int data_;

    /**
     * Helper function to get a @c std::shared_ptr from the static @c Mock
     * object.
     */
    static std::shared_ptr<Mock> mock_obj_ () {
        return std::shared_ptr{mock_obj_weak_};
    }
};

/**
 * Compare two @c Regular_Object instances for semantic equality.
 * @return @c true if they have the same data, else @c false.
 */
inline bool operator ==
(Regular_Object const& obj_a, Regular_Object const& obj_b) {
    // return std::compare_three_way{}(obj_a.data(), obj_b.data());
    return obj_a.data() == obj_b.data();
}

/**
 * Compare two @c Regular_Object instances for semantic inequality.
 * @return @c true if they have different data, else @c false.
 */
inline bool operator !=
(Regular_Object const& obj_a, Regular_Object const& obj_b) {
    return obj_a.data() != obj_b.data();
}

static_assert(std::regular<Regular_Object>);
}  // namespace fdn::testing

#endif  // FDN__TESTING__REGULAR_OBJECT_HPP_
