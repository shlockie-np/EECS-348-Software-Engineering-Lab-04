/**
 * @file nfl_scores.c
 * @brief Enumerates all possible NFL scoring combinations for a given score.
 *
 * Scoring plays used:
 *  - Touchdown + 2pt conversion = 8
 *  - Touchdown + Field Goal     = 7
 *  - Touchdown                  = 6
 *  - Field Goal                 = 3
 *  - Safety                     = 2
 *
 * The program repeatedly prompts for a score and prints all valid combinations.
 * Enter 0 or 1 to stop.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

/**
 * @brief Read a validated non-negative integer from stdin.
 *
 * This function prompts the user until a valid integer is entered.
 * It rejects:
 *  - non-integers (e.g., "abc")
 *  - overflow / out-of-range values
 *  - trailing characters (e.g., "12xyz")
 *  - negative values
 *
 * @param prompt Text shown to the user.
 * @param[out] out_value Parsed integer on success.
 * @return 1 on success, 0 on EOF/input stream end.
 */
static int read_valid_int(const char *prompt, int *out_value) {
    char buf[256];

    for (;;) {
        fputs(prompt, stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            return 0; // EOF or input error
        }

        // Trim leading whitespace
        char *p = buf;
        while (isspace((unsigned char)*p)) p++;

        // Reject empty input
        if (*p == '\0' || *p == '\n') {
            puts("Invalid input: please enter an integer.");
            continue;
        }

        errno = 0;
        char *end = NULL;
        long val = strtol(p, &end, 10);

        // No digits parsed
        if (end == p) {
            puts("Invalid input: please enter an integer (e.g., 25).");
            continue;
        }

        // Range/overflow check
        if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
            puts("Invalid input: number out of range for an int.");
            continue;
        }

        // Ensure remaining characters are whitespace only
        while (*end && isspace((unsigned char)*end)) end++;
        if (*end != '\0') {
            puts("Invalid input: trailing characters detected. Enter only an integer.");
            continue;
        }

        // Non-negative constraint
        if (val < 0) {
            puts("Invalid input: score must be non-negative.");
            continue;
        }

        *out_value = (int)val;
        return 1;
    }
}

/**
 * @brief Print all valid scoring-play combinations that sum to the given score.
 *
 * Uses brute-force bounded loops based on the maximum count possible for each play.
 *
 * @param score Team score (must be >= 0).
 */
static void print_combinations(int score) {
    int found = 0;

    printf("Possible combinations of scoring plays if a team's score is %d:\n", score);

    for (int td2 = 0; td2 <= score / 8; td2++) {          // TD + 2pt
        for (int tdfg = 0; tdfg <= score / 7; tdfg++) {   // TD + FG
            for (int td = 0; td <= score / 6; td++) {     // TD
                for (int fg = 0; fg <= score / 3; fg++) { // 3pt FG
                    int used = 8 * td2 + 7 * tdfg + 6 * td + 3 * fg;
                    if (used > score) continue;

                    int remaining = score - used;
                    if (remaining % 2 != 0) continue;

                    int safety = remaining / 2;

                    printf("%d TD + 2pt, %d TD + FG, %d TD, %d 3pt FG, %d Safety\n",
                           td2, tdfg, td, fg, safety);
                    found = 1;
                }
            }
        }
    }

    if (!found) {
        puts("(No combinations)");
    }
}

/**
 * @brief Program entry point.
 *
 * Prompts repeatedly for NFL scores until user enters 0 or 1 (or EOF).
 *
 * @return 0 on normal termination.
 */
int main(void) {
    puts("Enter 0 or 1 to STOP");

    for (;;) {
        int score;
        if (!read_valid_int("Enter the NFL score: ", &score)) {
            break; // EOF
        }

        if (score <= 1) {
            break; // stop condition
        }

        print_combinations(score);
    }

    return 0;
}