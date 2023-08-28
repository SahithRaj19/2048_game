#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/termios.h>
#include <unistd.h>

#define SIZE 4

struct game {
  int tiles[SIZE][SIZE];
};

enum {
  DOWN,
  UP,
  LEFT,
  RIGHT,
  MOVES
};

int random_spot(struct game* g)
{
    // Generate a random number between 0 and SIZE - 1....
  return rand() % SIZE;
}

int random_value(struct game* g)
{
    // Generate a random number between 0 and 9 (inclusive)....
    // If the result is not equal to 0, return 2. Otherwise, return 4....
  return (rand() % 10) ? 2 : 4;
}

void print(struct game* g)
{
  int i,j;
  printf("--------------------------\n");
  for (j = SIZE - 1; j >= 0; --j) {
    printf("|");
    for (i = 0; i < SIZE; ++i) {
      if (g->tiles[i][j])
        printf("%4d  ", g->tiles[i][j]);// used to access a specific tile value within a 2D array....
      else
        printf("      ");
    }
    printf("|\n");
  }
  printf("--------------------------\n");
}

void twist(struct game* g)
{
  int i,j;
  struct game g2;// Create a temporary game structure....
  for (i = 0; i < SIZE; ++i)
  for (j = 0; j < SIZE; ++j)
    g2.tiles[i][j] = g->tiles[j][i];// Swap rows and columns....
  *g = g2;
}

void flip(struct game* g)
{
  int i,j;
  struct game g2;
  for (i = 0; i < SIZE; ++i)
  for (j = 0; j < SIZE; ++j)
    g2.tiles[i][j] = g->tiles[i][SIZE - j - 1]; // Reverse order of tiles....
  *g = g2;
}

void begin(struct game* g)
{
  struct game g2 = {};// Create a new game structure g2 and initialize it with zeros....
  *g = g2;// Assign the values of g2 to the game structure pointed to by g....
  g->tiles[random_spot(g)][random_spot(g)] = random_value(g);// Place two random tiles on the board....
  g->tiles[random_spot(g)][random_spot(g)] = random_value(g);
}

void fall_column(int* a, int* b)
{
  int i,j;
  int prev = 0;
  j = 0;
  for (i = 0; i < SIZE; ++i)
    if (a[i]) {
      if (a[i] == prev) {
        b[j - 1] *= 2;
        prev = 0;
      } else {
        b[j++] = a[i];
        prev = a[i];
      }
    }
}

void fall(struct game* g)
{
  struct game g2 = {};
  int i;
  for (i = 0; i < SIZE; ++i)
    fall_column(g->tiles[i], g2.tiles[i]);
  *g = g2;
}

int same(struct game* a, struct game* b)
{
  int i,j;
  for (i = 0; i < SIZE; ++i)
  for (j = 0; j < SIZE; ++j)
    if (a->tiles[i][j] != b->tiles[i][j])
      return 0;
  return 1;
}

int tryfalling(struct game* g)
{
  struct game g2 = *g;
  fall(g);
  if (same(g, &g2))
    return 0;
  return 1;
}

void popup(struct game* g)
{
  int i,j;
  while (1) {
    i = random_spot(g);
    j = random_spot(g);
    if (!g->tiles[i][j]) {
      g->tiles[i][j] = random_value(g);
      return;
    }
  }
}

void move(struct game* g, int way)
{
  if (way / 2)// If way is even, twist the game board....
    twist(g);
    //   If way is odd....
  if (way % 2)// The function calls the flip function to flip the game board horizontally....
    flip(g);
  if (tryfalling(g))
    popup(g);
  if (way % 2)
    flip(g);
  if (way / 2)
    twist(g);
}

int read_move(void)
{
  char keys[MOVES] = {'k','i','j','l'};
  int c;
  int i;
  //isspace is a standard library function....
  //that checks whether a character is a whitespace character (space, tab, newline, etc.)....
  //checks if the read character is a whitespace character. If it is, the loop continues....
  while (isspace(c = getchar()));
  if (c == EOF)
    return c;
  // Compare the character with predefined keys....
  for (i = 0; i < MOVES; ++i)
    if (c == keys[i])
      return i; // Return the index corresponding to the mov....
  return EOF;  // If the input character doesn't match any predefined key, return EOF....
}

static struct termios backup;
static struct termios current;

> Rohan Ambig:
void take_stdin(void)
{
  tcgetattr(STDIN_FILENO, &backup); // Backup the current terminal attributes and it's helpful for storing the terminal attributes in backup structure
    //   STDIN_FILENO : allows read the input from the user....
  current = backup;
  // This line copies the attributes from the backup structure to the current structure....
  
  current.c_lflag &= (~ICANON & ~ECHO);// this will helpful for avoiding the character display on the terminal after pressing the keys....
  tcsetattr(STDIN_FILENO, TCSANOW, &current);//is used to apply modified terminal attributes after press the key....
  //TCSANOW indicates that the changes should take effect immediately in terminal....
  // &current : which holds the modified terminal attributes that you want to apply....
}

void give_stdin(void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &backup);
}

int main()
{
  int c;
  struct game g;// Declare an instance of the struct game called g....
  begin(&g);// Call the begin function to initialize the game g....
  print(&g);
  take_stdin();
  // Read characters from standard input until EOF is encountered.....
  
  while ((c = read_move()) != EOF) {
    move(&g, c);
    print(&g);
  }
  give_stdin();
  return 0;
}
