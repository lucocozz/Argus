#include <criterion/criterion.h>
#include "argus/internal/utils.h"

Test(strings, starts_with_valid)
{
    char *test_str = "prefix";
    // Test basic prefix matching
    cr_assert_not_null(starts_with("pre", test_str));
    
    // Test the returned pointer position
    char *result = starts_with("pre", test_str);
    cr_assert_eq(result, test_str + 3);
    cr_assert_str_eq(result, "fix");
    
    // Test with empty prefix
    cr_assert_not_null(starts_with("", "text"));
    char *result2 = starts_with("", "text");
    cr_assert_str_eq(result2, "text");
    
    // Additional tests: starting with single character
    cr_assert_not_null(starts_with("a", "abc"));
    cr_assert_str_eq(starts_with("a", "abc"), "bc");
    
    // Test case sensitivity
    cr_assert_null(starts_with("Pre", test_str));
    cr_assert_not_null(starts_with("Pre", "Prefix"));
}

Test(strings, starts_with_invalid)
{
    // Test with non-matching prefix
    cr_assert_null(starts_with("abc", "def"));
    
    // Test with NULL inputs
    cr_assert_null(starts_with(NULL, "text"));
    cr_assert_null(starts_with("pre", NULL));
    cr_assert_null(starts_with(NULL, NULL));
    
    // Test with prefix longer than string
    cr_assert_null(starts_with("long_prefix", "short"));
    
    // Additional tests: almost matching but not quite
    cr_assert_null(starts_with("abc", "ab"));
    cr_assert_null(starts_with("abc", "abx"));
}

Test(strings, split_basic)
{
    char *test_str = "one,two,three";
    char **result = split(test_str, ",");
    
    cr_assert_not_null(result, "split() should return a non-NULL result");
    cr_assert_str_eq(result[0], "one", "First split element should be 'one'");
    cr_assert_str_eq(result[1], "two", "Second split element should be 'two'");
    cr_assert_str_eq(result[2], "three", "Third split element should be 'three'");
    cr_assert_null(result[3], "Result array should be NULL-terminated");
    
    free_split(result);
}

Test(strings, split_edge_cases)
{
    // Empty string test
    char *empty_str = "";
    char **result1 = split(empty_str, ",");
    cr_assert_not_null(result1, "split() should handle empty string");
    cr_assert_null(result1[0], "Result for empty string should be NULL-terminated empty array");
    free_split(result1);
    
    // Empty parts test
    char *empty_parts_str = "one,,three";
    char **result2 = split(empty_parts_str, ",");
    cr_assert_not_null(result2, "split() should handle empty parts");
    cr_assert_str_eq(result2[0], "one", "First split element should be 'one'");
    cr_assert_str_eq(result2[1], "three", "Second split element should be 'three'");
    cr_assert_null(result2[2], "Result array should be NULL-terminated");
    free_split(result2);
    
    // Leading separator
    char *leading_sep_str = ",one,two";
    char **result3 = split(leading_sep_str, ",");
    cr_assert_not_null(result3, "split() should handle leading separator");
    cr_assert_str_eq(result3[0], "one", "First split element should be 'one'");
    cr_assert_str_eq(result3[1], "two", "Second split element should be 'two'");
    cr_assert_null(result3[2], "Result array should be NULL-terminated");
    free_split(result3);
    
    // Trailing separator
    char *trailing_sep_str = "one,two,";
    char **result4 = split(trailing_sep_str, ",");
    cr_assert_not_null(result4, "split() should handle trailing separator");
    cr_assert_str_eq(result4[0], "one", "First split element should be 'one'");
    cr_assert_str_eq(result4[1], "two", "Second split element should be 'two'");
    cr_assert_null(result4[2], "Result array should be NULL-terminated");
    free_split(result4);
}

Test(strings, split_multiple_separators)
{
    char *test_str = "a:b;c:d";
    char **result = split(test_str, ":;");
    
    cr_assert_not_null(result, "split() should handle multiple separators");
    cr_assert_str_eq(result[0], "a", "First split element should be 'a'");
    cr_assert_str_eq(result[1], "b", "Second split element should be 'b'");
    cr_assert_str_eq(result[2], "c", "Third split element should be 'c'");
    cr_assert_str_eq(result[3], "d", "Fourth split element should be 'd'");
    cr_assert_null(result[4], "Result array should be NULL-terminated");
    
    free_split(result);
}
