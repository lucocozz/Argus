#define _GNU_SOURCE  // NOLINT

#include "argus/internal/compiler.h"
#include "argus/internal/cross_platform.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * Checks if a string starts with a specific prefix.
 * @param prefix The prefix string to search for.
 * @param str The string to search in.
 * @return A pointer to the first character after the prefix if found,
 *         NULL otherwise or if any parameter is NULL.
 */
char *starts_with(const char *prefix, const char *str)
{
    if (prefix == NULL || str == NULL)
        return (NULL);

    size_t prefix_len = strlen(prefix);

    if (strncmp(str, prefix, prefix_len) == 0)
        return ((char *)(str + prefix_len));
    return (NULL);
}

static size_t count_words(const char *str, const char *charset)
{
    size_t count = 0;
    size_t i     = 0;

    while (str[i] != '\0') {
        if (strchr(charset, str[i]) == NULL) {
            count++;
            while (str[i] && strchr(charset, str[i]) == NULL)
                i++;
        } else
            i++;
    }
    return (count);
}

static size_t word_len(const char *str, const char *charset)
{
    size_t size = 0;

    while (str[size] != '\0' && strchr(charset, str[size]) == NULL)
        size++;
    return (size);
}

static int skip_charset(const char *str, const char *charset)
{
    int i = 0;

    while (str[i] != '\0' && strchr(charset, str[i]) != NULL)
        ++i;
    return (i);
}

static void cleanup_split(char **split, size_t nb_words)
{
    for (size_t i = 0; i < nb_words; ++i)
        free(split[i]);
    free(split);
}

/**
 * Splits a string into an array of strings using a charset.
 * @param str The string to split.
 * @param charset The charset used to split the string.
 * @return An array of strings or NULL if an error occurred.
 */
char **split(const char *str, const char *charset)
{
    char  *tmp      = (char *)str;
    size_t nb_words = count_words(str, charset);
    char **result   = malloc(sizeof(char *) * (nb_words + 1));

    if (result == NULL)
        return (NULL);

    for (size_t i = 0; i < nb_words; ++i) {
        tmp += skip_charset(tmp, charset);

        size_t len  = word_len(tmp, charset);
        char  *word = safe_strndup(tmp, len);
        if (word == NULL) {
            cleanup_split(result, i);
            return (NULL);
        }
        result[i] = word;
        tmp += len;
    }
    result[nb_words] = NULL;
    return (result);
}

void free_split(char **split)
{
    for (size_t i = 0; split[i] != NULL; ++i)
        free(split[i]);
    free(split);
}
