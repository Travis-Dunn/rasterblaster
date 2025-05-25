#ifndef MOUSE_H
#define MOUSE_H

#include "stdlib.h"
#include "string.h"

extern int* pickbuf;

int InitPickbuf(int w, int h);
void ClearPickbuf();
void FreePickbuf();
void UpdatePickbuf(int x, int y, int id);
int GetClicked(int mouseX, int mouseY);

#endif /* MOUSE_H */
