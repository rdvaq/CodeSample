#ifndef _MORE_FILE_H
#define _MORE_FILE_H

// Tries to open a file for reading with appropriate flags, if it fails program
// aborts
int checked_open_for_reading(char *filename);

// Tries to open a file for writing with appropriate flags, if it fails program
// aborts
int checked_open_for_writing(char *filename);

#endif