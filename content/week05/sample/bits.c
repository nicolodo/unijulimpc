#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// Construct a colour from its components.
// Similar to lecture notes
int colour(int r, int g, int b, int a) {
    unsigned int ur = r, ug = g, ub = b, ua = a;
    return (ur << 24) + (ug << 16) + (ub << 8) + ua;
}

// Unpack a colour into its components.
// Similar to lecture notes
void components(int c, int rgba[4]) {
    unsigned int uc = c;
    rgba[0] = (uc >> 24) & 0xFF;
    rgba[1] = (uc >> 16) & 0xFF;
    rgba[2] = (uc >> 8) & 0xFF;
    rgba[3] = uc & 0xFF;
}

// Form a 3D point from three signed 10-bit integers.
// A coordinate has to be masked so that if it is negative, it becomes ten bits
// with zeros elsewhere, e.g. x & 0x3FF. Technically, if n is -1,
// this changes it to 1023 as an int, but it remains -1 regarded as a ten-bit number.
// Then the patterns of ten bits can be merged.
int point(int x, int y, int z) {
    unsigned int ux = x, uy = y, uz = z;
    return ((ux & 0x3FF) << 20) | ((uy & 0x3FF) << 10) | (uz & 0x3FF);
}

// Separate a position into three signed 10-bit coordinates.
// The loop at the end deals with sign-extension. If the leftmost bit of the ten
// is a one, i.e. ... & 0x200 == 0x200 or ... & 0x200 != 0, then 22 ones need to
// be added in to get the right result as an int.
void coordinates(int p, int xyz[3]) {
    unsigned int up = p;
    xyz[0] = (up >> 20) & 0x3FF;
    xyz[1] = (up >> 10) & 0x3FF;
    xyz[2] = up & 0x3FF;
    for (int i=0; i<3; i++) {
        if ((xyz[i] & 0x200) != 0) xyz[i] = 0xFFFFFC00 | xyz[i];
    }
}

// Convert an int into a binary string of 32 bits.
// Existing functions such as itoa or sprintf (%b) could be used, but these are
// not standard across all platforms. So we create our own.
// There is a choice between working out the binary digits left-to-right or right-to-left (as here).
// Working right-to-left, you can extract the rightmost bit of n with n & 0x1,
// then shift n right to discard the rightmost bit with n = n >> 1.
// The code is like this if you treat i as counting the number of times round the loop.
// Note that n % 2 or n = n / 2 arithmetic will not work when n is negative.
void binary(int n, char s[33]) {
    unsigned int un = n;
    for (int i=0; i<32; i++) {
        int bit = un & 0x1;
        s[31 - i] = '0' + bit;
        un = un >> 1;
    }
    s[32] = '\0';
}

// Convert an int into a hex string of 8 hex digits.
// This can be accomplished using one line: sprintf(s, "%08X", n);
// However, below we roll our own, right-to-left, by extracting four bits
// using the 0xF mask, moving on to the next four with un >> 4 and so on.
// You get an integer d with value between 0 and 15, convert it to a hex
// character value from the digits[] array, adding it to the string, then
// finally adding the null marker.
void hex(int n, char s[9]) {
    unsigned int un = n;
    char digits[] = "0123456789ABCDEF";
    for (int i=0; i<8; i++) {
        int d = un & 0xF;
        s[7-i] = digits[d];
        un = un >> 4;
    }
    s[8] = '\0';
}

// ----------------------------------------------------------------------------
// Testing and user interface.

// Tests 1 to 5
void testColour() {
    assert(colour(255,0,0,0) == 0xFF000000);
    assert(colour(0,255,0,0) == 0x00FF0000);
    assert(colour(0,0,255,0) == 0x0000FF00);
    assert(colour(0,0,0,255) == 0x000000FF);
    assert(colour(1,2,3,4) == 0x01020304);
}

// Do a single test of the components function with given input and output.
bool checkComponents(unsigned int c, int r, int g, int b, int a) {
    int rgba[4];
    for (int i=0; i<4; i++) rgba[i] = -1;
    components(c, rgba);
    return rgba[0] == r && rgba[1] == g && rgba[2] == b && rgba[3] == a;
}

// Tests 6 to 10
void testComponents() {
    assert(checkComponents(0xFF000000, 255, 0, 0, 0));
    assert(checkComponents(0x00FF0000, 0, 255, 0, 0));
    assert(checkComponents(0x0000FF00, 0, 0, 255, 0));
    assert(checkComponents(0x000000FF, 0, 0, 0, 255));
    assert(checkComponents(0x01020304, 1, 2, 3, 4));
}

// Tests 11 to 16
void testPoint() {
    assert(point(0,0,0) == 0);
    assert(point(1,3,7) == 0x00100C07);
    assert(point(1,3,-7) == 0x00100FF9);
    assert(point(1,-3,7) == 0x001FF407);
    assert(point(-1,3,7) == 0x3FF00C07);
    assert(point(-1,-3,-7) == 0x3FFFF7F9);
}

// Do a single test of the coordinates function with given input and output.
bool checkCoordinates(unsigned int p, int x, int y, int z) {
    int xyz[3];
    for (int i=0; i<3; i++) xyz[i] = 1000000000;
    coordinates(p, xyz);
    return xyz[0] == x && xyz[1] == y && xyz[2] == z;
}

// Tests 17 to 22
void testCoordinates() {
    assert(checkCoordinates(0, 0, 0, 0));
    assert(checkCoordinates(0x00100C07, 1, 3, 7));
    assert(checkCoordinates(0x00100FF9, 1, 3, -7));
    assert(checkCoordinates(0x001FF407, 1, -3, 7));
    assert(checkCoordinates(0x3FF00C07, -1, 3, 7));
    assert(checkCoordinates(0x3FFFF7F9, -1, -3, -7));
}

// Do a single test of the binary function with given input and output.
// Initialise s with incorrect values to give predictable results.
bool checkBinary(int in, char out[33]) {
    char s[33];
    for (int i=0; i<33; i++) s[i] = 'x';
    binary(in, s);
    return strcmp(s, out) == 0;
}

// Tests 23 to 35
void testBinary() {
    assert(checkBinary(0, "00000000000000000000000000000000"));
    assert(checkBinary(1, "00000000000000000000000000000001"));
    assert(checkBinary(2, "00000000000000000000000000000010"));
    assert(checkBinary(3, "00000000000000000000000000000011"));
    assert(checkBinary(4, "00000000000000000000000000000100"));
    assert(checkBinary(100, "00000000000000000000000001100100"));
    assert(checkBinary(2147483647, "01111111111111111111111111111111"));
    assert(checkBinary(-1, "11111111111111111111111111111111"));
    assert(checkBinary(-2, "11111111111111111111111111111110"));
    assert(checkBinary(-3, "11111111111111111111111111111101"));
    assert(checkBinary(-4, "11111111111111111111111111111100"));
    assert(checkBinary(-100, "11111111111111111111111110011100"));
    assert(checkBinary(-2147483648, "10000000000000000000000000000000"));
}

// Do a single test of the hex function with given input and output.
bool checkHex(int in, char out[9]) {
    char s[9];
    for (int i=0; i<9; i++) s[i] = 'x';
    hex(in, s);
    return strcmp(s, out) == 0;
}

// Tests 36 to 50
void testHex() {
    assert(checkHex(0, "00000000"));
    assert(checkHex(1, "00000001"));
    assert(checkHex(2, "00000002"));
    assert(checkHex(10, "0000000A"));
    assert(checkHex(15, "0000000F"));
    assert(checkHex(16, "00000010"));
    assert(checkHex(100, "00000064"));
    assert(checkHex(2147483647, "7FFFFFFF"));
    assert(checkHex(-1, "FFFFFFFF"));
    assert(checkHex(-2, "FFFFFFFE"));
    assert(checkHex(-3, "FFFFFFFD"));
    assert(checkHex(-4, "FFFFFFFC"));
    assert(checkHex(-16, "FFFFFFF0"));
    assert(checkHex(-100, "FFFFFF9C"));
    assert(checkHex(-2147483648, "80000000"));
}

void test() {
    testColour();
    testComponents();
    testPoint();
    testCoordinates();
    testBinary();
    testHex();
    printf("All tests pass\n");
}

// Print a number in binary.
void printBinary(int n) {
    char s[33];
    binary(n, s);
    printf("%s\n", s);
}

// Print a number in hex.
void printHex(int n) {
    char s[9];
    hex(n, s);
    printf("%s\n", s);
}

// Print a number in hex or binary, or run tests.
int main(int n, char *args[n]) {
    if (n == 1) test();
    else if (n == 3 && strcmp(args[1],"-h") == 0) printHex(atoi(args[2]));
    else if (n == 3 && strcmp(args[1],"-b") == 0) printBinary(atoi(args[2]));
    else printf("Use:   ./bits   or   ./bits -h n   or   ./bits -b n");
    return 0;
}
