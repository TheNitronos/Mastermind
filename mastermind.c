#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> // pow()
#include <assert.h>

/*
  It compiles without any problem...
  Knuth algo seems to work but for size 5, it may take some time...
*/

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

// --------------------------------------------------
void delete_bitset(BitSet* bitS)
{
  if (bitS != NULL) {
    free(bitS->bytes);
    bitS->size = 0;
  }
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

//return 1 if there is still a 0 value in the bitset, 0 otherwise
int bitset_available(const BitSet bitset) {
  for (size_t i = 0; i < bitset.size; ++i) {
    if (!bitset_get(bitset, i)) {
      return 1;
    }
  }

  return 0;
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
void delete_combination(Combination* combination) {
  if (combination != NULL) {
    if (combination->elements != NULL) {
      free(combination->elements);
    }
    combination->size = 0;
  }
}

// --------------------------------------------------
int next_combination(Combination* combination)
{
  int overflow = 1;

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
  if (combination == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < combination->size; ++i) {
    combination->elements[i] = 0;
  }

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

// --------------------------------------------------
void delete_solver_support(Solver_support* solverSup)
{
  if (solverSup != NULL) {
    delete_combination(&(solverSup->currentCombi));
    delete_combination(&(solverSup->tempCombi));
    delete_bitset(&(solverSup->bitS));
  }
}

// --------------------------------------------------
int ask(const Combination combination, Answer* const answer)
{
  if (answer == NULL) {
    printf("Answer* == NULL. Aborting.\n");
    fflush(stdout);

    return 1;
  }

  int rPositions = -1;
  int rColors = -1;
  int rLength = -1;

  print_combination(combination);
  printf("Please score attempt (positions, colors): ");
  fflush(stdout);

  rLength = scanf("%d %d", &rPositions, &rColors);
  answer->positions = rPositions;
  answer->colors = rColors;

  if ((rLength != 2) || (rPositions < 0) || (rColors < 0) || getc(stdin) != '\n') {
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

  //while we can play and there is still a next combination... play
  while ((!ask(combination, &answer)) && (!next_combination(&combination)));

  delete_combination(&combination);
}

// --------------------------------------------------
int review_combinations(Solver_support* s, size_t* count)
{
  assert(s != NULL); // review_combination() is a tool function so this should never happen
  Answer answerPlayer;
  Answer answerAttempt;

  if (ask(s->currentCombi, &answerPlayer)) {
    return 0;
  } else {
    //set the corresponding bit in BitSet to 1
    bitset_set(&(s->bitS), combination_to_index(s->currentCombi), 1);
    if (count != NULL) {
      --*count;
    }

    //go through all combinations and set to 1, which are not still possible
    for (size_t i = 0; i < (s->bitS.size); ++i) {
      if (!bitset_get(s->bitS, i)) {
        score_attempt(&(s->currentCombi), combination_from_index(i, &(s->tempCombi)), &answerAttempt);

        if (answerAttempt.positions != answerPlayer.positions ||
            answerPlayer.colors != answerAttempt.colors) {
              bitset_set(&(s->bitS), i, 1);
              if (count != NULL) {
                --*count;
              }
        }
      }
    }

    return 1;
  }
}

// --------------------------------------------------
void solve_with_bitset(size_t size)
{
  Solver_support solverSup = create_solver_support(size);

  while (review_combinations(&solverSup, NULL) &&
         !next_combination(&(solverSup.currentCombi)) &&
         bitset_available(solverSup.bitS)) {
    //find the next combination, which is possible according to the BitSet
    while (bitset_get(solverSup.bitS, combination_to_index(solverSup.currentCombi))) {
      next_combination(&(solverSup.currentCombi));
    }
  }

  delete_solver_support(&solverSup);
}

// --------------------------------------------------
void solve_knuth(size_t size)
{
  Solver_support solverSup = create_solver_support(size);
  const size_t TOTAL_SIZE = pow(6, size);
  const size_t SIZE_PLUS_ONE = size+1;

  Combination iterCombi = create_combination(size);
  Combination possibleCombi = create_combination(size);
  Answer answer;

  //chose the first combination : YB, YYB, YYBB or YYYBB
  switch (size) {
    case 2:
      combination_from_index(6, &(solverSup.currentCombi));
      break;
    case 3:
      combination_from_index(36, &(solverSup.currentCombi));
      break;
    case 4:
      combination_from_index(252, &(solverSup.currentCombi));
      break;
    case 5:
      combination_from_index(1512, &(solverSup.currentCombi));
      break;
  }

  /*
  while the combination has not been foud and there are still possible
  combinations in the Solver_support
  */
  while (review_combinations(&solverSup, NULL) &&
         bitset_available(solverSup.bitS)) {

    //initial values
    combination_from_index(0, &iterCombi);
    combination_from_index(0, &possibleCombi);
    answer.positions = 0;
    answer.colors = 0;

    //max of each 2d array
    int tempMax;
    //2d array for finding all max
    int tempArray[SIZE_PLUS_ONE][SIZE_PLUS_ONE];
    for (size_t i = 0; i < SIZE_PLUS_ONE; ++i) {
      for (size_t j = 0; j < SIZE_PLUS_ONE; ++j) {
        tempArray[i][j] = 0;
      }
    }
    //array of the max of each 2d arrary max
    int maxArray[TOTAL_SIZE];

    //iterate over all combinations
    for (size_t iterCombiIndex = 0; iterCombiIndex < TOTAL_SIZE; ++iterCombiIndex) {

      //initial values
      combination_from_index(iterCombiIndex, &iterCombi);
      tempMax = 0;

      //iterate over all possible combinations
      for (size_t possibleCombiIndex = 0; possibleCombiIndex < TOTAL_SIZE; ++possibleCombiIndex) {
        //if it is a possible combination
        if (!bitset_get(solverSup.bitS, possibleCombiIndex)) {
          //increment the corresponding location (score) in the 2d array
          combination_from_index(possibleCombiIndex, &possibleCombi);
          score_attempt(&iterCombi, &possibleCombi, &answer);
          ++tempArray[answer.positions][answer.colors];
        }
      }

      //find the max in the 2d array and reset its values to 0
      for (size_t i = 0; i < SIZE_PLUS_ONE; ++i) {
        for (size_t j = 0; j < SIZE_PLUS_ONE; ++j) {
          if (tempArray[i][j] > tempMax) {
            tempMax = tempArray[i][j];
          }
          tempArray[i][j] = 0;
        }
      }

      //save the the temporary max in the maxArray
      maxArray[iterCombiIndex] = tempMax;
    }

    int minCardinality = TOTAL_SIZE;
    size_t minCardinalityIndex = 0;

    //find the ideal combination
    for (size_t i = 0; i < TOTAL_SIZE; ++i) {
      /*
      either maxArray[i] is equal to minCardinality and need to select the first
      combination, which is still possible
      or the the maxArray[i] is smaller than minCardinality
      */
      if (maxArray[i] < minCardinality ||
          (maxArray[i] == minCardinality &&
           !bitset_get(solverSup.bitS, i) &&
           bitset_get(solverSup.bitS, minCardinalityIndex))) {

        minCardinality = maxArray[i];
        minCardinalityIndex = i;
      }
    }

    //set the new combination to be the next, which will be asked
    combination_from_index(minCardinalityIndex, &solverSup.currentCombi);
  }

  //free the memory
  delete_solver_support(&solverSup);
  delete_combination(&iterCombi);
  delete_combination(&possibleCombi);
}

// ==== main() ==========================================================
int main(void)
{
    size_t size = 0;
    do {
        printf("What size (2, 3, 4, 5)? ");
        fflush(stdout);
        scanf("%zu", &size);
        //scanf("%Iu", &size);
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
