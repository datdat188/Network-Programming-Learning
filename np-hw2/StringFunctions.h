//
//  StringFunctions.h
//  Proxy Server
//
//  Created by Matt Zanchelli on 3/1/14.
//  Copyright (c) 2014 Matt Zanchelli. All rights reserved.
//

#ifndef Proxy_Server_StringFunctions_h
#define Proxy_Server_StringFunctions_h

/// Defintion of a Boolean.
typedef enum {
    false = 0,
    true  = 1
} bool;

/// Duplicate a string.
/// @param str The string to duplicate.
/// @return A newly allocated duplicated string.
char *stringDuplicate(const char *str);

/// Test the equality of two strings.
/// @param s1 The first string.
/// @param s2 The second string.
/// @return Whether or not the strings are equal.
bool stringEquality(const char *s1, const char *s2);

/// Create a lower-case version of a string.
/// @param s The string to convert to lowercase.
/// @return A newly allocated string.
char *lowerCaseVersionOfString(const char *s);

/// Compare two ascii strings and ignoring case.
/// @param s1 The first string.
/// @param s2 The second string.
/// @return An integer greater than, equal to, or less than 0 representing whether @c s1 is lexicographically greater than, equal to, or less than @c s2.
/// @discussion Comparison done after translation of each corresponding character to lower-case. The strings themselves are not modified. The comparison is done using unsigned characters, so that `\200' is greater than `\0'.
/// @see strncasecmp Modeled after @c strncasecmp as it is not available on all systems.
int caseInsensitiveStringComparison(const char *s1, const char *s2);

/// Compare two ascii strings and ignoring case.
/// @param s1 The first string.
/// @param s2 The second string.
/// @param n Compare at most n characters.
/// @return An integer greater than, equal to, or less than 0 representing whether @c s1 is lexicographically greater than, equal to, or less than @c s2.
/// @discussion Comparison done after translation of each corresponding character to lower-case. The strings themselves are not modified. The comparison is done using unsigned characters, so that `\200' is greater than `\0'.
/// @see strncasecmp Modeled after @c strncasecmp as it is not available on all systems.
int caseInsensitiveStringComparisonLimited(const char *s1, const char *s2, size_t n);

/// Find the index of the first occurance of a character in a string.
/// @param string The null-terminated byte stream.
/// @param character The character to search for.
/// @return The index at which the first instace of @c character exists in @c string.
/// @discussion Returns -1 if not found.
int indexOfFirstOccuranceOfCharacterInString(const char *string, const char character);

/// Create a substring.
/// @param begin The pointer to the beginning of the string.
/// @param end The pointer to the end of the substring.
/// @return A newly allocated string for the substring.
char *substring(const char *begin, const char *end);

/// Get the prefix of a string until a specific character.
/// @param string The null-terminated byte stream.
/// @param character The character to search for.
/// @return A newly allocated string containing the prefix of @c string up until the first occurance of @c character.
/// @discussion Returns @c NULL if @c character does not exist in @c string.
char *prefixOfStringUpUntilCharacter(const char *string, const char character);

/// Get the prefix of a string until a substring.
/// @param string The null-terminated byte stream.
/// @param end The substring to end at.
/// @return A newly allocated string containing the prefix of @c string up until the first cocurance of @c substring.
/// @discussion Returns @c NULL if @c substring does not exist in @c string.
char *prefixOfStringUpUntilString(const char *string, const char *end);

/// Split a string into two with a string separator.
/// @param fullString The full string to split.
/// @param delimiter The delimiter to split on the first occurance of in @c fullString.
/// @param firstString Will be the part of @c fullString that appears before @c delimiter.
/// @param secondString Will be the part of @c secondString that appears after @c delimiter.
/// @return Whether or not the split was successful.
bool splitStringAtString(const char *fullString, const char *delimiter, char **firstString, char **secondString);

#endif
