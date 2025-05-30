// Author: realyoavperetz@gmail.com

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "MyContainer.hpp"

#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>

using namespace ariel;

// ---------------------------------------------------------------------------
// helper: collect any iterator range into a std::vector<T>
template<typename It>
static auto collect(It first, It last) {
    using Val = typename std::iterator_traits<It>::value_type;
    std::vector<Val> out;
    for (; first != last; ++first) out.push_back(*first);
    return out;
}

// ---------------------------------------------------------------------------
// 1. Empty container
TEST_CASE("Empty container") {
    MyContainer<int> c;
    CHECK(c.size() == 0);
    CHECK(c.begin_order() == c.end_order());
    CHECK_THROWS_AS(c.removeElement(1), std::out_of_range);
}

// 2. Single element – every iterator should yield the same single value
TEST_CASE("Single element") {
    MyContainer<int> c;
    c.addElement(99);
    std::vector<int> expected{99};

    CHECK(collect(c.begin_order(),            c.end_order())            == expected);
    CHECK(collect(c.begin_reverse_order(),    c.end_reverse_order())    == expected);
    CHECK(collect(c.begin_ascending_order(),  c.end_ascending_order())  == expected);
    CHECK(collect(c.begin_descending_order(), c.end_descending_order()) == expected);
    CHECK(collect(c.begin_side_cross_order(), c.end_side_cross_order()) == expected);
    CHECK(collect(c.begin_middle_out_order(), c.end_middle_out_order()) == expected);
}

// 3. Middle-out iterator on an even-sized container (either mid-low or mid-high is OK)
TEST_CASE("MiddleOut iterator even count") {
    MyContainer<int> c;
    for (int v : {10, 20, 30, 40}) c.addElement(v);  // insertion order 10 20 30 40
    auto result = collect(c.begin_middle_out_order(), c.end_middle_out_order());

    std::vector<int> optA{30, 20, 40, 10};  // start at upper middle
    std::vector<int> optB{20, 30, 10, 40};  // start at lower middle
    bool ok = (result == optA) || (result == optB);
    CHECK(ok);
}

// 4. Canonical data set – verify all six iterator orders
TEST_CASE("Iterator orders on canonical data") {
    MyContainer<int> c;
    for (int v : {7, 15, 6, 1, 2}) c.addElement(v);

    CHECK(collect(c.begin_ascending_order(),  c.end_ascending_order())  == std::vector<int>{1,2,6,7,15});
    CHECK(collect(c.begin_descending_order(), c.end_descending_order()) == std::vector<int>{15,7,6,2,1});
    CHECK(collect(c.begin_side_cross_order(), c.end_side_cross_order()) == std::vector<int>{1,15,2,7,6});
    CHECK(collect(c.begin_reverse_order(),    c.end_reverse_order())    == std::vector<int>{2,1,6,15,7});
    CHECK(collect(c.begin_order(),            c.end_order())           == std::vector<int>{7,15,6,1,2});
    CHECK(collect(c.begin_middle_out_order(), c.end_middle_out_order()) == std::vector<int>{6,15,1,7,2});
}

// 5. operator<< formatting
TEST_CASE("operator<< formatting") {
    MyContainer<int> c;
    for (int v : {3, 1, 4}) c.addElement(v);
    std::ostringstream os;
    os << c;
    CHECK(os.str() == "[3, 1, 4]");
}

// 6. Template parameter: std::string
TEST_CASE("Template with std::string") {
    MyContainer<std::string> s;
    s.addElement("b");
    s.addElement("a");
    CHECK(collect(s.begin_ascending_order(), s.end_ascending_order())
          == std::vector<std::string>{"a", "b"});
}

// 7. Template parameter: custom comparable struct
TEST_CASE("Template with custom comparable struct") {
    struct Item {
        int key{};
        bool operator<(const Item& other) const  { return key < other.key; }
        bool operator==(const Item& other) const { return key == other.key; }
    };
    MyContainer<Item> c;
    c.addElement({2});
    c.addElement({1});
    auto asc = collect(c.begin_ascending_order(), c.end_ascending_order());
    CHECK(asc.front().key == 1);
    CHECK(asc.back().key  == 2);
}

// 8. Copy / move semantics
TEST_CASE("Copy and move semantics") {
    MyContainer<int> original;
    for (int i = 0; i < 5; ++i) original.addElement(i);

    MyContainer<int> copy = original;                 // copy-ctor
    CHECK(copy.size() == original.size());
    copy.removeElement(2);
    CHECK(copy.size() == original.size() - 1);

    MyContainer<int> moved = std::move(original);     // move-ctor
    CHECK(moved.size() == 5);
    CHECK(original.size() == 0);                      // moved-from should be empty
}

// 9. Large random container – ascending iterator should equal sorted vector
TEST_CASE("Large container ascending equals std::vector sorted") {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 1000);

    MyContainer<int> c;
    std::vector<int> reference;
    for (int i = 0; i < 1000; ++i) {
        int v = dist(rng);
        c.addElement(v);
        reference.push_back(v);
    }

    std::sort(reference.begin(), reference.end());
    auto asc = collect(c.begin_ascending_order(), c.end_ascending_order());
    CHECK(asc == reference);
}

// 10. Add and remove many elements – container should end empty
TEST_CASE("Add/remove many elements leaves empty") {
    MyContainer<int> c;
    for (int i = 0; i < 500; ++i) c.addElement(i);
    for (int i = 0; i < 500; ++i) c.removeElement(i);
    CHECK(c.size() == 0);
}