#ifndef _ERRORS_H
#define _ERRORS_H

// Like printf, but prints message to stderr and then aborts the program
extern void custom_err(const char *format, ...);

// Like custom error but also reports the information stored in errno before
// abort
extern void errno_err(const char *format, ...);

#endif