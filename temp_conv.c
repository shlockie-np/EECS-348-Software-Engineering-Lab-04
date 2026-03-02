/**
 * @file temp_conv.c
 * @brief Temperature conversion utility with categorization and advisory.
 *
 * Features:
 *  - Converts between Fahrenheit, Celsius, Kelvin
 *  - Validates input (numeric parsing, choices, absolute zero constraint)
 *  - Categorizes temperature based on Celsius value and prints advisory text
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <math.h>

/**
 * @enum Scale
 * @brief Supported temperature scales.
 */
typedef enum {
    SCALE_F = 1, /**< Fahrenheit */
    SCALE_C = 2, /**< Celsius */
    SCALE_K = 3  /**< Kelvin */
} Scale;

/**
 * @brief Convert Celsius to Fahrenheit.
 * @param celsius Temperature in Celsius.
 * @return Temperature in Fahrenheit.
 */
float celsius_to_fahrenheit(float celsius) {
    return (9.0f / 5.0f) * celsius + 32.0f;
}

/**
 * @brief Convert Fahrenheit to Celsius.
 * @param fahrenheit Temperature in Fahrenheit.
 * @return Temperature in Celsius.
 */
float fahrenheit_to_celsius(float fahrenheit) {
    return (5.0f / 9.0f) * (fahrenheit - 32.0f);
}

/**
 * @brief Convert Celsius to Kelvin.
 * @param celsius Temperature in Celsius.
 * @return Temperature in Kelvin.
 */
float celsius_to_kelvin(float celsius) {
    return celsius + 273.15f;
}

/**
 * @brief Convert Kelvin to Celsius.
 * @param kelvin Temperature in Kelvin.
 * @return Temperature in Celsius.
 */
float kelvin_to_celsius(float kelvin) {
    return kelvin - 273.15f;
}

/**
 * @brief Convert Fahrenheit to Kelvin.
 * @param fahrenheit Temperature in Fahrenheit.
 * @return Temperature in Kelvin.
 */
float fahrenheit_to_kelvin(float fahrenheit) {
    return celsius_to_kelvin(fahrenheit_to_celsius(fahrenheit));
}

/**
 * @brief Convert Kelvin to Fahrenheit.
 * @param kelvin Temperature in Kelvin.
 * @return Temperature in Fahrenheit.
 */
float kelvin_to_fahrenheit(float kelvin) {
    return celsius_to_fahrenheit(kelvin_to_celsius(kelvin));
}

/**
 * @brief Categorize temperature (based on Celsius) and print an advisory.
 *
 * Categories (example thresholds):
 *  - c < 0           : Freezing
 *  - 0 <= c < 10     : Cold
 *  - 10 <= c < 25    : Comfortable
 *  - 25 <= c <= 35   : Hot
 *  - c > 35          : Extreme Heat
 *
 * @param celsius Temperature in Celsius used for categorization.
 */
void categorize_temperature(float celsius) {
    const char *category = NULL;
    const char *advisory = NULL;

    if (celsius < 0.0f) {
        category = "Freezing";
        advisory = "Wear a heavy coat, gloves, and be careful of ice.";
    } else if (celsius < 10.0f) {
        category = "Cold";
        advisory = "Wear a jacket or sweater.";
    } else if (celsius < 25.0f) {
        category = "Comfortable";
        advisory = "Comfortable weather—light layers are fine.";
    } else if (celsius <= 35.0f) {
        category = "Hot";
        advisory = "Stay hydrated and avoid heavy activity in peak sun.";
    } else {
        category = "Extreme Heat";
        advisory = "Stay indoors if possible and watch for heat illness.";
    }

    printf("Category: %s\n", category);
    printf("Advisory: %s\n", advisory);
}

/**
 * @brief Read one line from stdin into a buffer.
 * @param buf Destination buffer.
 * @param n Buffer size in bytes.
 * @return 1 on success, 0 on EOF/error.
 */
static int read_line(char *buf, size_t n) {
    return fgets(buf, (int)n, stdin) != NULL;
}

/**
 * @brief Parse a float strictly from a string (reject trailing junk).
 *
 * Accepts leading/trailing whitespace but rejects any non-space trailing chars.
 * Also rejects overflow/underflow and non-finite results.
 *
 * @param s Input string.
 * @param[out] out Parsed float.
 * @return 1 if parsed successfully, 0 otherwise.
 */
static int parse_float_strict(const char *s, float *out) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return 0;

    errno = 0;
    char *end = NULL;
    float val = strtof(s, &end);

    if (end == s) return 0;        // no number parsed
    if (errno == ERANGE) return 0; // overflow/underflow
    if (!isfinite(val)) return 0;

    while (isspace((unsigned char)*end)) end++;
    if (*end != '\0' && *end != '\n') return 0; // trailing junk

    *out = val;
    return 1;
}

/**
 * @brief Parse an integer strictly from a string (reject trailing junk).
 * @param s Input string.
 * @param[out] out Parsed integer.
 * @return 1 on success, 0 on failure.
 */
static int parse_int_strict(const char *s, int *out) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return 0;

    errno = 0;
    char *end = NULL;
    long val = strtol(s, &end, 10);

    if (end == s) return 0;
    if (errno == ERANGE || val < INT_MIN || val > INT_MAX) return 0;

    while (isspace((unsigned char)*end)) end++;
    if (*end != '\0' && *end != '\n') return 0;

    *out = (int)val;
    return 1;
}

/**
 * @brief Prompt until the user enters a valid float.
 * @param prompt Text shown to the user.
 * @param[out] out Parsed float.
 * @return 1 on success, 0 on EOF.
 */
static int prompt_float(const char *prompt, float *out) {
    char buf[256];
    for (;;) {
        fputs(prompt, stdout);
        if (!read_line(buf, sizeof(buf))) return 0;
        if (parse_float_strict(buf, out)) return 1;
        puts("Invalid input. Please enter a valid number (e.g., 98.6).");
    }
}

/**
 * @brief Prompt until the user enters a valid integer choice in [min, max].
 * @param prompt Text shown to the user.
 * @param min Minimum allowed integer.
 * @param max Maximum allowed integer.
 * @param[out] out Parsed choice.
 * @return 1 on success, 0 on EOF.
 */
static int prompt_choice(const char *prompt, int min, int max, int *out) {
    char buf[256];
    for (;;) {
        fputs(prompt, stdout);
        if (!read_line(buf, sizeof(buf))) return 0;
        int x;
        if (parse_int_strict(buf, &x) && x >= min && x <= max) {
            *out = x;
            return 1;
        }
        printf("Invalid choice. Enter an integer from %d to %d.\n", min, max);
    }
}

/**
 * @brief Convert a Scale enum to a human-readable name.
 * @param s Scale value.
 * @return Constant string describing the scale.
 */
static const char* scale_name(Scale s) {
    switch (s) {
        case SCALE_F: return "Fahrenheit";
        case SCALE_C: return "Celsius";
        case SCALE_K: return "Kelvin";
        default:      return "Unknown";
    }
}

/**
 * @brief Convert a temperature from its input scale to Celsius.
 * @param value Temperature value in the input scale.
 * @param in_scale Scale of @p value.
 * @return Temperature in Celsius (may be NAN for invalid scale).
 */
static float to_celsius(float value, Scale in_scale) {
    switch (in_scale) {
        case SCALE_C: return value;
        case SCALE_F: return fahrenheit_to_celsius(value);
        case SCALE_K: return kelvin_to_celsius(value);
        default:      return NAN;
    }
}

/**
 * @brief Convert a Celsius temperature to a target output scale.
 * @param celsius Temperature in Celsius.
 * @param out_scale Desired output scale.
 * @return Temperature in the output scale (may be NAN for invalid scale).
 */
static float from_celsius(float celsius, Scale out_scale) {
    switch (out_scale) {
        case SCALE_C: return celsius;
        case SCALE_F: return celsius_to_fahrenheit(celsius);
        case SCALE_K: return celsius_to_kelvin(celsius);
        default:      return NAN;
    }
}

/**
 * @brief Program entry point for the temperature conversion utility.
 *
 * Validates:
 *  - numeric temperature input
 *  - scale choices
 *  - disallows same input/output scale
 *  - disallows temperatures below absolute zero (-273.15 C)
 *
 * @return 0 on normal termination.
 */
int main(void) {
    puts("Temperature Conversion Utility");
    puts("Scales: 1 = Fahrenheit, 2 = Celsius, 3 = Kelvin\n");

    for (;;) {
        float temp_in;
        int in_choice, out_choice;

        if (!prompt_float("Enter the temperature value (or Ctrl+D to quit): ", &temp_in)) {
            puts("\nGoodbye.");
            return 0;
        }

        if (!prompt_choice("Enter input scale (1=F, 2=C, 3=K): ", 1, 3, &in_choice)) {
            puts("\nGoodbye.");
            return 0;
        }

        if (!prompt_choice("Enter target scale (1=F, 2=C, 3=K): ", 1, 3, &out_choice)) {
            puts("\nGoodbye.");
            return 0;
        }

        Scale in_scale = (Scale)in_choice;
        Scale out_scale = (Scale)out_choice;

        if (in_scale == out_scale) {
            puts("Invalid conversion choice: input and target scales must be different.\n");
            continue;
        }

        // Convert to Celsius (common baseline)
        float c = to_celsius(temp_in, in_scale);

        // Absolute zero validation: -273.15 C
        if (!isfinite(c) || c < -273.15f) {
            puts("Invalid temperature: below absolute zero (or negative Kelvin equivalent). Please re-enter.\n");
            continue;
        }

        float temp_out = from_celsius(c, out_scale);

        printf("\nInput:  %.2f %s\n", temp_in, scale_name(in_scale));
        printf("Output: %.2f %s\n", temp_out, scale_name(out_scale));

        categorize_temperature(c);

        char again[32];
        fputs("\nConvert another? (y/n): ", stdout);
        if (!read_line(again, sizeof(again))) {
            puts("\nGoodbye.");
            return 0;
        }
        if (tolower((unsigned char)again[0]) != 'y') {
            puts("Goodbye.");
            return 0;
        }
        puts("");
    }
}