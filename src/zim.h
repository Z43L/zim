#ifndef __ZIM_H__
#define __ZIM_H__

#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>


#define CTRL_KEY(k) ((k) & 0x1f)
#define ZIM_VERSION "0.0.1"
#define ZIM_TAB_STOP 8
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE
#define ZIM_QUIT_TIMES 3
#define HL_HIGHLIGHT_STRINGS (1<<1)
#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))
#define ABUF_INIT {NULL, 0}

struct editorConfig E;


typedef struct erow {
  int size;
  char *chars;
  int rsize;
  char *render;
  unsigned char *hl;
  int idx;
  int hl_open_comment;
} erow;

struct editorSyntax {
  char *filetype;
  char **filematch;
  char **keywords;
  char *singleline_comment_start;
  char *multiline_comment_start;
  char *multiline_comment_end;
  int flags;
};

struct editorConfig {
  int cx, cy;
  int rx;
  int rowoff;
  int coloff;
  int screenrows;
  int screencols;
  char *filename;
  int dirty;
  int numrows;
  char statusmsg[80];
  time_t statusmsg_time;
  erow *row;
  struct editorSyntax *syntax;
  struct termios orig_termios;
};

struct abuf {
  char *b;
  int len;
};

enum editorKey {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT  ,
  ARROW_UP  ,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};
enum editorHighlight {
  HL_NORMAL = 0,
  HL_KEYWORD1,
  HL_KEYWORD2,
  HL_NUMBER,
  HL_MATCH,
  HL_STRING,
  HL_COMMENT,
  HL_MLCOMMENT,
};




void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt, void (*callback)(char *, int));
int editorRowCxToRx(erow *row, int cx);
int editorRowRxToCx(erow *row, int rx);
void editorScroll();
void die(const char *s);
void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);
int editorReadKey();
void enableRawMode(void);
void disableRawMode(void);
int getWindowSize(int *rows, int *cols);
int getCursorPosition(int *rows, int *cols);
int editorSyntaxToColor(int hl);
void editorUpdateSyntax(erow *row);
int is_separator(int c);
void editorSelectSyntaxHighlight();
char *editorRowsToString(int *buflen);
void editorDelChar();
void editorInsertChar(int c);
void editorRowDelChar(erow *row, int at);
void editorRowAppendString(erow *row, char *s, size_t len);
void editorInsertNewline();
void editorRowInsertChar(erow *row, int at, int c);
void editorDelRow(int at);
void editorFreeRow(erow *row);
void editorInsertRow(int at, char *s, size_t len);
void editorUpdateRow(erow *row);
void editorSave();
void editorOpen(char *filename);
void editorFind();
void editorFindCallback(char *query, int key);
void editorRefreshScreen();
void editorDrawMessageBar(struct abuf *ab);
void editorDrawStatusBar(struct abuf *ab);
void editorDrawRows(struct abuf *ab);
void editorSetStatusMessage(const char *fmt, ...);
char *editorPrompt(char *prompt, void (*callback)(char *, int));
void editorProcessKeypress();
void editorMoveCursor(int key);



#endif