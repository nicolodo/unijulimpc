/* Text library module. */

// A text object is a string, wrapped as an object to add memory management.
struct text;
typedef struct text text;

// Create a new text object containing a dynamic copy of the given string.
text *newText(char *s);

// Free up the memory for the text object.
void freeText(text *t);

// Find the length of a text (equivalent to strlen).
int length(text *t);

// Extract the i'th character (equivalent to s[i]).
char get(text *t, int i);

// Set the i'th character (equivalent to s[i] = c).
void set(text *t, int i, char c);

// Make a copy (equivalent to strcpy).
text *copy(text *t);

// Compare two texts (equivalent to strcmp).
int compare(text *t1, text *t2);

// Join a second text to the end of a first (equivalent to strcat).
void append(text *t1, text *t2);

// Extract a subtext (no exact equivalent for strings).
text *slice(text *t, int i, int j);

// Search for a subtext (roughly equivalent to strstr).
int find(text *t, text *p);
