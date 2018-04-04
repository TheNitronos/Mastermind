#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> // pow()
#include <assert.h>

// ==== Color ===========================================================
enum color {YELLOW, BLUE, GREEN, RED, PINK, ORANGE};
typedef enum color color;
const unsigned int colorEnumSize = 6;

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
  Combination newCombi;

  newCombi.size = size;
  newCombi.elements = calloc(size, sizeof(color));

  for(size_t i=0; i < size; ++i) {
    newCombi.elements[i] = YELLOW;
  }

  return newCombi;
}

// --------------------------------------------------
int next_combination(Combination* const combination)
{
  unsigned int overflow = 1;
  for (size_t i=0; i < combination->size; ++i) {
    combination->elements[i] = (combination->elements[i]+overflow)%colorEnumSize;

    if (overflow && (combination->elements[i] == YELLOW)) {
      overflow = 1;
    } else {
      overflow = 0;
    }
  }

  return overflow;
}

// --------------------------------------------------
size_t combination_to_index(const Combination combination)
{
  size_t index = 0;

  for (size_t i = 0; i < combination.size; ++i) {
    index += combination.elements[i]*pow(6, i);
  }

  return index;
}

// --------------------------------------------------
Combination* combination_from_index(size_t index, Combination* combination)
{
  *combination = create_combination(combination->size);
  int j;

  for (int i = (combination->size - 1); i >= 0; --i) {
    j = 5;

    while ((j >= 0) && (index >= pow(6, i))) {
      if ((index - j*pow(6, i)) >= 0) {
        combination->elements[i] = j;
        index -= j*pow(6, i);
      }

      --j;
    }
  }

  return combination;
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
  Combination currentCombi;
  Combination tempCombi;
  BitSet bitS;
} Solver_support;

// --------------------------------------------------
Solver_support create_solver_support(size_t size)
{
  Solver_support solverSup;

  solverSup.currentCombi = create_combination(size);
  solverSup.tempCombi = create_combination(size);
  solverSup.bitS = create_bitset(pow(6, size));

  return solverSup;
}

void set_current_combi_solver_support(Solver_support* const solverSup, const Combination combination)
{
  solverSup->currentCombi = combination;
}

void set_temp_combi_solver_support(Solver_support* const solverSup, const Combination combination)
{
  solverSup->tempCombi = combination;
}

// --------------------------------------------------
int ask(const Combination combination, Answer* const answer)
{
  int rPositions = -1;
  int rColors = -1;
  int rLength = -1;

  print_combination(combination);

  printf("Please score attempt (positions, colors): ");
  fflush(stdout);

  rLength = scanf("%d %d", &rPositions, &rColors);
  answer->positions = rPositions;
  answer->colors = rColors;

  if ((rLength != 2) || (rPositions < 0) || (rColors < 0)) {
    printf("Unable to parse answer. Aborting.\n");
    fflush(stdout);

    return 1;
  }

  if ((rPositions == combination.size) && (rColors == 0)) {
    printf("Found solution: ");
    print_combination(combination);
    fflush(stdout);

    return 1;
  }

  return 0;
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
void solve_brute_force(size_t size)
{
  Combination combination = create_combination(size);
  Answer answer;

  while ((!ask(combination, &answer)) && (!next_combination(&combination)));
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
        scanf("%Iu", &size);
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
