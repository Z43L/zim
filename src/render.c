#include "zim.h"

void editorDrawRows(struct abuf *ab) {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    int filerow = y + E.rowoff;
    if (filerow >= E.numrows) {
      if (E.numrows == 0 && y == E.screenrows / 3) {
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome),
          "david moreno editor -- version %s", ZIM_VERSION);
        if (welcomelen > E.screencols) welcomelen = E.screencols;
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) {
          abAppend(ab, "~", 1);
          padding--;
        }
        while (padding--) abAppend(ab, " ", 1);
        abAppend(ab, welcome, welcomelen);
      } else {
        abAppend(ab, "~", 1);
      }
    } else {
      int len = E.row[filerow].rsize - E.coloff;
      if (len < 0) len = 0;
      if (len > E.screencols) len = E.screencols;
      char *c = &E.row[filerow].render[E.coloff];
      unsigned char *hl = &E.row[filerow].hl[E.coloff];
      int current_color = -1;
      int j;
      for (j = 0; j < len; j++) {
        if (filerow == E.cy && j == E.cx - E.coloff) { // Check for cursor position
           abAppend(ab, "|", 1);
        }
        if (iscntrl(c[j])) {
          char sym = (c[j] <= 26) ? '@' + c[j] : '?';
          abAppend(ab, "\x1b[7m", 4);
          abAppend(ab, &sym, 1);
          abAppend(ab, "\x1b[m", 3);
          if (current_color != -1) {
            char buf[16];
            int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
            abAppend(ab, buf, clen);
          }
        } else if (hl[j] == HL_NORMAL) {
          if (current_color != -1) {
            abAppend(ab, "\x1b[39m", 5);
            current_color = -1;
          }
          abAppend(ab, &c[j], 1);
        } else {
          int color = editorSyntaxToColor(hl[j]);
          if (color != current_color) {
            current_color = color;
            char buf[16];
            int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
            abAppend(ab, buf, clen);
          }
          abAppend(ab, &c[j], 1);
        }
      }
      abAppend(ab, "\x1b[39m", 5);
      if (filerow == E.cy) { // Current row
      //int cursorcol = E.cx - E.coloff;
      //if (cursorcol >= 0 && cursorcol <= len) {
        //char cursor_str = {'|'}; // The cursor character (now an array)
        //abAppend(ab, &cursor_str, 1); // Pass the array (decays to pointer)
        //len++; // Account for the cursor in the visible length
        //}
      }
    }
    abAppend(ab, "\x1b[K", 3);
    abAppend(ab, "\r\n", 2);
  }
}


void editorDrawStatusBar(struct abuf *ab) {
  abAppend(ab, "\x1b[7m", 4);
  char status[80], rstatus[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
    E.filename ? E.filename : "[No Name]", E.numrows,
    E.dirty ? "(modified)" : "");

  int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d",
    E.syntax ? E.syntax->filetype : "no ft", E.cy + 1, E.numrows);
  if (len > E.screencols) len = E.screencols;
  abAppend(ab, status, len);
  while (len < E.screencols) {
    if (E.screencols - len == rlen) {
      abAppend(ab, rstatus, rlen);
      break;
    } else {
      abAppend(ab, " ", 1);
      len++;
    }
  }
  abAppend(ab, "\x1b[m", 3);
  abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab) {
  abAppend(ab, "\x1b[K", 3);
  int msglen = strlen(E.statusmsg);
  if (msglen > E.screencols) msglen = E.screencols;
  if (msglen && time(NULL) - E.statusmsg_time < 5)
    abAppend(ab, E.statusmsg, msglen);
}

void editorRefreshScreen() {
  editorScroll(); // Ensure proper scrolling before drawing

  struct abuf ab = ABUF_INIT; // Initialize the output buffer

  // ANSI escape sequences to hide the cursor and position it at the top-left
  abAppend(&ab, "\x1b[?25l", 6); 
  abAppend(&ab, "\x1b[H", 3);

  // Draw the main text content, status bar, and message bar
  editorDrawRows(&ab);
  editorDrawStatusBar(&ab);
  editorDrawMessageBar(&ab);

  // Position the cursor using an ANSI escape sequence
  // Note the inclusion of E.coloff for correct horizontal positioning
  char buf;
  snprintf(&buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, (E.rx - E.coloff) + 1);
  abAppend(&ab, &buf, strlen(&buf));

  // Show the cursor
  abAppend(&ab, "\x1b[?25h", 6);

  // Write the accumulated output to the terminal
  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab); // Free the output buffer
}
