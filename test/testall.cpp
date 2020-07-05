#define UNUSED(a) (void)a
// #define DEBUG

#include "version.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "slvr_lib_factorial.h"
#include "slvr_allocator.h"
#include "slvr_container.h"

TEST(version, version_test)
{
    const char *version_by_source = PROJECT_VERSION;
    const char *fn_version_result = slvr::allocator::version();

    std::string ver_string(fn_version_result);

    EXPECT_STREQ(version_by_source, fn_version_result);
    EXPECT_STRCASEEQ(version_by_source, fn_version_result);
    EXPECT_GE(ver_string.length(), 5);
}

TEST(lib_factorial, total)
{
    long long fn_result_from_0 = slvr::lib::llfactorial(0);
    long long fn_result_from_1 = slvr::lib::llfactorial(1);
    long long fn_result_from_10 = slvr::lib::llfactorial(10);
    EXPECT_THROW(slvr::lib::llfactorial(21), std::overflow_error);

    EXPECT_EQ(1, fn_result_from_0);
    EXPECT_EQ(1, fn_result_from_1);
    EXPECT_EQ(3628800, fn_result_from_10);
}

TEST(allocator, total)
{
    struct char4
    {
        char text[4] = "abc";
        char4(const char txt[4])
        {
            text[0] = txt[0];
            text[1] = txt[1];
            text[2] = txt[2];
            text[3] = '\0';
        }
        ~char4() = default;
    };

    slvr::allocator::superK<long> test_allocator_l;
    slvr::allocator::superK<int> test_allocator_i;
    slvr::allocator::superK<char> test_allocator_c;
    using vect = std::vector<char, slvr::allocator::superK<char>>;
    slvr::allocator::superK<char4> test_allocator_obj;

    auto palloc_l = test_allocator_l.allocate(6);
    auto palloc_i = test_allocator_i.allocate(6);
    auto palloc_obj = test_allocator_obj.allocate(6);

    vect *vect_test = new vect({'a', 'b', 'c', 'd', 'e', 'f'});

    for (int i = 0; i < 6; ++i)
    {
        test_allocator_l.construct(palloc_l + i, i * 10);
        test_allocator_i.construct(palloc_i + i, i);
        test_allocator_obj.construct(palloc_obj + i, "xyz");
    }

    int sum_l = 0;
    int sum_i = 0;
    std::string str_result{};
    for (int i = 0; i < 6; ++i)
    {
        sum_l += *(palloc_l + i);
        sum_i += *(palloc_i + i);
        str_result += (*vect_test)[i];
        str_result += (palloc_obj + i)->text;
    }

    EXPECT_EQ(150, sum_l);
    EXPECT_EQ(15, sum_i);
    EXPECT_STRCASEEQ("axyzbxyzcxyzdxyzexyzfxyz", str_result.c_str());

    EXPECT_EQ(1024 / sizeof(long), test_allocator_l.get_limit());
    EXPECT_EQ(1024 / sizeof(int), test_allocator_i.get_limit());
    EXPECT_EQ(1024 / sizeof(char), test_allocator_c.max_size());
    EXPECT_EQ(1024 / sizeof(char4), test_allocator_obj.max_size());

    EXPECT_THROW(test_allocator_l.set_limit(5), std::logic_error);
    EXPECT_THROW(test_allocator_i.set_limit(5), std::logic_error);
    EXPECT_THROW(test_allocator_c.set_limit(5), std::logic_error);
    EXPECT_THROW(test_allocator_obj.set_limit(5), std::logic_error);
    EXPECT_NO_THROW(test_allocator_l.set_limit(6));
    EXPECT_NO_THROW(test_allocator_i.set_limit(6));
    EXPECT_NO_THROW(test_allocator_c.set_limit(6));
    EXPECT_NO_THROW(test_allocator_obj.set_limit(6));

    EXPECT_EQ(6, test_allocator_l.get_limit());
    EXPECT_EQ(6, test_allocator_i.get_limit());
    EXPECT_EQ(6, test_allocator_c.max_size());
    EXPECT_EQ(6, test_allocator_obj.max_size());

    test_allocator_l.set_limit(0);
    auto palloc_l2 = test_allocator_l.allocate(6);

    for (auto i = 6; i != 0; i--)
        test_allocator_i.destroy(palloc_i + i - 1);
    test_allocator_i.deallocate(palloc_i, 6);
    palloc_i = test_allocator_i.allocate(6);
    for (int i = 0; i < 6; ++i)
    {
        test_allocator_l.construct(palloc_l2 + i, i * 10);
        test_allocator_i.construct(palloc_i + i, -i);
    }

    sum_l = 0;
    sum_i = 0;
    for (int i = 0; i < 6; ++i)
    {
        sum_l += *(palloc_l + i) + *(palloc_l2 + i);
        sum_i += *(palloc_i + i);
    }

    EXPECT_EQ(300, sum_l);
    EXPECT_EQ(-15, sum_i);

    for (auto i = 6; i != 0; i--)
    {
        test_allocator_l.destroy(palloc_l + i - 1);
        test_allocator_l.destroy(palloc_l2 + i - 1);
        test_allocator_i.destroy(palloc_i + i - 1);
        test_allocator_obj.destroy(palloc_obj + i - 1);
    }
    vect_test->clear();
    delete vect_test;
    test_allocator_i.deallocate(palloc_i, 6);
    test_allocator_l.deallocate(palloc_l, 6);
    test_allocator_l.deallocate(palloc_l2, 6);
    test_allocator_obj.deallocate(palloc_obj, 6);

    int max_n_long = test_allocator_l.max_size();
    EXPECT_NO_THROW(palloc_l = test_allocator_l.allocate(max_n_long));
    for (int i = 0; i < max_n_long; ++i)
        test_allocator_l.construct(palloc_l + i, i);
    EXPECT_THROW(palloc_l2 = test_allocator_l.allocate(1), std::bad_alloc);
    for (int i = max_n_long; i >= 0; --i)
        test_allocator_l.destroy(palloc_l + i);
    test_allocator_l.deallocate(palloc_l, max_n_long);
}

TEST(container, total)
{
    slvr::container::massive<long> arr1_stdalloc;
    slvr::container::massive<int, std::allocator<int>> arr2_stdalloc;
    slvr::container::massive<long long, slvr::allocator::superK<long long>> arr3_myalloc;

    for (int i = 0; i < 6; ++i)
    {
        arr1_stdalloc.push_back(i);
        arr2_stdalloc.push_back(i * 10);
        arr3_myalloc.push_back(i * 100);
    }

    std::string result1{};
    std::string result2{};
    std::string result3{};
    for (auto el : arr1_stdalloc)
        result1 += std::to_string(el);
    for (auto it = arr2_stdalloc.begin(); it != arr2_stdalloc.end(); ++it)
        result2 += std::to_string(*it);
    for (auto it = arr3_myalloc.end(); it != arr3_myalloc.begin();)
    {
        --it;
        result3 += std::to_string(*it);
    }

    EXPECT_STRCASEEQ("012345", result1.c_str());
    EXPECT_STRCASEEQ("01020304050", result2.c_str());
    EXPECT_STRCASEEQ("5004003002001000", result3.c_str());

    result1.clear();
    int new_capacity = 4 * arr1_stdalloc.capacity();
    arr1_stdalloc.reserve(new_capacity);
    EXPECT_EQ(new_capacity, arr1_stdalloc.capacity());

    int new_size = arr1_stdalloc.size() / 2;
    arr1_stdalloc.resize(new_size);
    for (auto el : arr1_stdalloc)
        result1 += std::to_string(el);
    EXPECT_EQ(new_size, arr1_stdalloc.size());
    EXPECT_STRCASEEQ("012", result1.c_str());

    result2.clear();
    for (size_t i = 0; i < arr1_stdalloc.size(); ++i)
    {
        long temp = arr1_stdalloc[i];
        temp = -temp - 1;
        arr1_stdalloc[i] = temp;
    }
    for (auto el : arr1_stdalloc)
        result2 += std::to_string(el);

    EXPECT_STRCASEEQ("-1-2-3", result2.c_str());
    EXPECT_THROW(arr1_stdalloc[-1], std::range_error);
    EXPECT_THROW(arr1_stdalloc[arr1_stdalloc.size()], std::range_error);

    arr1_stdalloc.resize(0);
    EXPECT_THROW(arr1_stdalloc[0], std::range_error);

    auto some_allocator = arr3_myalloc.get_allocator();
    int allocated = arr3_myalloc.capacity();
    some_allocator.set_limit(allocated);
    EXPECT_THROW(some_allocator.allocate(1), std::bad_alloc);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}