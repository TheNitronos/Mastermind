#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> // pow()
#include <assert.h>

enum color {YELLOW, BLUE, GREEN, RED, PINK, ORANGE};
typedef enum color color;

// ==== BitSet ==========================================================

typedef struct {
  size_t size;
  char* bytes;
} BitSet;

/** --------------------------------------------------
 * @brief creates a BitSet structure of a given size, specified as the number of wanted bits.
 * @param size number of wanted bits
 * @return the newly created (dynamically allocated) BitSet structure
 */
BitSet create_bitset(size_t size)
{
    BitSet result = { 0, NULL };
    const size_t extra = size % 8 > 0 ? 1 : 0;
    result.bytes = calloc(size / 8 + extra, sizeof(char));
    if (result.bytes != NULL) {
        result.size = size;
    }
    return result;
}

/** --------------------------------------------------
 * @brief gets the bit value at bit position `index`
 * @param bitset the BitSet structure to read from
 * @param index the bit position to read
 * @return 0 or 1 according to bit value
 */
int bitset_get(const BitSet bitset, size_t index)
{
    if (index >= bitset.size) return 0;
    return (bitset.bytes[index / 8] >> (index % 8)) & 0x1;
}

/** --------------------------------------------------
 * @brief sets the bit value at bit position `index`
 * @param bitset the BitSet structure to write to
 * @param index the bit position to overwrite
 * @param bit 0 or 1 value to be set
 */
void bitset_set(BitSet* bitset, size_t index, int bit)
{
    assert(bitset != NULL); // this is a tool function
    if (index > bitset->size) return;
    char* prev = bitset->bytes + (index / 8);
    char shifted = 0x1 << (index % 8);
    *prev = bit ? (*prev | shifted) : (*prev & (-1 ^ shifted));
}


// ==== Combination =====================================================

typedef struct {
  size_t size;
  color* elements;
} Combination;

// --------------------------------------------------
Combination create_combination(size_t size)
{
}

// --------------------------------------------------
size_t combination_to_index(const Combination combination)
{
}

// --------------------------------------------------
Combination* combination_from_index(size_t index, Combination* combination)
{
}

// --------------------------------------------------
void print_combination(const Combination combination)
{
    for (size_t i = 0; i < combination.size; ++i) {
        if (i > 0) putchar(' ');
        switch(combination.elements[i]) {
        case YELLOW:
            putchar('Y');
            break;
        case BLUE:
            putchar('B');
            break;
        case GREEN:
            putchar('G');
            break;
        case RED:
            putchar('R');
            break;
        case PINK:
            putchar('P');
            break;
        case ORANGE:
            putchar('O');
            break;
        default:
            printf("\nERROR\n");
            printf("Unexpected color %d\n", combination.elements[i]);
        }
    }
    putchar('\n');
}

// ==== Answer ==========================================================

typedef struct {
  unsigned int positions;
  unsigned int colors;
} Answer;

// ==== Solvers =========================================================

typedef struct {
  Combination combiOne;
  Combination combiTwo;
  BitSet bits;
} Solver_support;

// --------------------------------------------------
void solve_brute_force(size_t size)
{
}

// --------------------------------------------------
int score_attempt(const Combination* attempt, const Combination* result, Answer* ans)
{
    if (ans     == NULL) return 0;
    ans->colors    = 0;
    ans->positions = 0;
    if (result  == NULL) return 0;
    if (attempt == NULL) return 0;
    if (attempt->size != result->size) return 0;

    int* used = calloc(attempt->size, sizeof(int));
    if (used == NULL) return 0;

    // Scoring positions
    for (size_t i = 0; i < attempt->size; ++i) {
        if (attempt->elements[i] == result->elements[i]) {
            ++(ans->positions);
            used[i] = 1;
        }
    }

    // Scoring colors
    for (size_t i = 0; i < attempt->size; ++i) {
        if (attempt->elements[i] != result->elements[i]) {
            int found = 0;
            for (size_t j = 0; !found && (j < attempt->size); ++j) {
                if (i != j && !used[j] && (attempt->elements[i] == result->elements[j])) {
                    ++(ans->colors);
                    used[j] = 1;
                    found = 1;
                }
            }
        }
    }

    free(used);
    return 1;
}

// --------------------------------------------------
int review_combinations(Solver_support* s, size_t* count)
{
    assert(s != NULL); // review_combination() is a tool function so this should never happen
}

// --------------------------------------------------
void solve_with_bitset(size_t size)
{
}


// --------------------------------------------------
void solve_knuth(size_t size)
{
}

// ==== main() ==========================================================

int main(void)
{
    size_t size = 0;
    do {
        printf("What size (2, 3, 4, 5)? ");
        fflush(stdout);
        scanf("%zu", &size);
    } while ((size != 2) && (size != 3) && (size != 4) && (size != 5));

    char strategy = '\0';
    do {
        printf("What strategy ([B]rute force, B[i]tfield, [K]nuth)? ");
        fflush(stdout);
        scanf(" %c", &strategy);
    } while ((strategy != 'B') && (strategy != 'i') && (strategy != 'K'));

    switch(strategy) {
    case 'B':
        solve_brute_force(size);
        break;

    case 'i':
        solve_with_bitset(size);
        break;

    case 'K':
        solve_knuth(size);
        break;
    }

    return 0;
}
