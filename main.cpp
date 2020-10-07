#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cctype>
#include <cstdlib>
#define NOT !

enum TokenType
{
  IDENTIFIER = 34512,
  CONSTANT = 87232,
  SPECIAL = 29742
};

typedef char *CharPtr;

struct Column
{
  int column;
  Column *next;
};

typedef Column *ColumnPtr;

struct Line
{
  int line;
  ColumnPtr firstAppearAt;
  ColumnPtr lastAppearAt;
  Line *next;
};

typedef Line *LinePtr;

struct Token
{
  CharPtr tokenStr;
  TokenType type;
  LinePtr firstAppearOn;
  LinePtr lastAppearOn;
  Token *next;
};

typedef Token *TokenPtr;

TokenPtr gFront = NULL, gRear = NULL, gTokenTemp = NULL, gBetweenTemp = NULL;

typedef char Str100[100];

int gLine = 1;

int gColumn = 0;

int gIndex = 0;

int uIdentifier = 0;

int uConstant = 0;

int uSpecial = 0;

bool bSecond = false;

using namespace std;

char GetChar()
{
  char ch = '\0';
  scanf("%c", &ch);
  gColumn = gColumn + 1;
  if (ch == '\n' || ch == '\r')
  {
    gLine = gLine + 1;
    gColumn = 0;
  }
  return ch;
}

TokenType WhichType(char ch)
{
  char temp = '\0';
  if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || (ch == '_'))
    return IDENTIFIER;
  else if (('0' <= ch && ch <= '9') || ch == '"' || ch == '\'')
    return CONSTANT;
  else if (ch == '/' || ch == '+' || ch == '-' || ch == '*' || ch == '>' || ch == '<' || ch == '=' || ch == '?' || ch == '%' || ch == '&' ||
           ch == '|' || ch == '^' || ch == '.' || ch == ',' || ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' ||
           ch == '!' || ch == ':' || ch == ';' || ch == '#')
  {
    return SPECIAL;
  }
}

CharPtr ReadRemaining(char ch, TokenType type, int &column, int &line, bool &comment)
{
  comment = false;
  column = gColumn;
  line = gLine;
  CharPtr returnPtr = NULL;
  Str100 catched = "\0";
  char nextChar = '\0';
  char peek = '\0';
  int i = 0;
  catched[i] = ch;
  i = i + 1;
  if (type == IDENTIFIER)
  {
    peek = cin.peek();
    while (WhichType(peek) != SPECIAL && peek != ' ' && peek != '\n' && peek != '\r' && peek != -1)
    {
      catched[i] = GetChar();
      i = i + 1;
      peek = cin.peek();
    }
  }
  else if (type == CONSTANT)
  {
    bool quotationBackslash = false;
    bool evenSlash = false;
    if (ch == '"')
    {
      nextChar = GetChar();
      peek = cin.peek();
      if (nextChar == '\\' && peek == '\\')
        evenSlash = true;
      if (nextChar == '\\' && peek == '"' && NOT evenSlash)
        quotationBackslash = true;
      if (nextChar != '"')
      {
        while (peek != '"' || quotationBackslash)
        {
          catched[i] = nextChar;
          i = i + 1;
          nextChar = GetChar();
          peek = cin.peek();
          if (catched[i - 2] == '.' && nextChar == '\\' && peek == '"')
            quotationBackslash = false;
          else
          {
            if (nextChar == '\\' && peek == '"' && NOT evenSlash)
              quotationBackslash = true;
            else
              quotationBackslash = false;
          }
        }
      }
      if (peek == '"')
      {
        catched[i] = nextChar;
        catched[i + 1] = GetChar();
      }
      else
        catched[i] = nextChar;
    }
    else if (ch == '\'')
    {
      peek = cin.peek();
      if (peek == '\\')
      {
        catched[i] = GetChar();
        i = i + 1;
      }
      catched[i] = GetChar();
      i = i + 1;
      catched[i] = GetChar();
      i = i + 1;
    }
    else
    {
      peek = cin.peek();
      while (peek != ' ' && peek != '\n' && peek != '\r' && WhichType(peek) != SPECIAL)
      {
        catched[i] = GetChar();
        i = i + 1;
        peek = cin.peek();
      }
    }
  }
  else if (type == SPECIAL)
  {
    peek = cin.peek();
    if (ch == '<' || ch == '>' || ch == '+' || ch == '-' || ch == '=' || ch == '&' || ch == '|' || ch == '!')
    {
      if (peek == '<' || peek == '>' || peek == '+' || peek == '-' || peek == '=' || peek == '&' || peek == '|')
        catched[i] = GetChar();
    }
    else if (ch == '/')
    {
      if (peek == '/')
      {
        while (peek != '\n' && peek != '\r')
        {
          nextChar = GetChar();
          peek = cin.peek();
        }
        comment = true;
      }
      else if (peek == '*')
      {
        nextChar = GetChar();
        nextChar = GetChar();
        peek = cin.peek();
        while (1)
        {
          nextChar = GetChar();
          peek = cin.peek();
          if (nextChar == '*' && peek == '/')
            break;
        }
        nextChar = GetChar();
        comment = true;
      }
    }
  }
  returnPtr = new char[strlen(catched) + 1];
  strcpy(returnPtr, catched);
  return returnPtr;
}

bool FindAnySame(CharPtr catched)
{
  if (strcmp(catched, gTokenTemp->tokenStr) != 0 && gTokenTemp->next != NULL)
  {
    gTokenTemp = gTokenTemp->next;
    FindAnySame(catched);
  }
  if (strcmp(catched, gTokenTemp->tokenStr) == 0)
    return true;
  else
    return false;
}

void InsertSameToken(CharPtr catched, int line, int column)
{
  if (line == gTokenTemp->lastAppearOn->line)
  {
    gTokenTemp->lastAppearOn->lastAppearAt->next = new Column;
    gTokenTemp->lastAppearOn->lastAppearAt = gTokenTemp->lastAppearOn->lastAppearAt->next;
    gTokenTemp->lastAppearOn->lastAppearAt->next = NULL;
    gTokenTemp->lastAppearOn->lastAppearAt->column = column;
  }
  else
  {
    gTokenTemp->lastAppearOn->next = new Line;
    gTokenTemp->lastAppearOn = gTokenTemp->lastAppearOn->next;
    gTokenTemp->lastAppearOn->next = NULL;
    gTokenTemp->lastAppearOn->line = line;
    gTokenTemp->lastAppearOn->firstAppearAt = new Column;
    gTokenTemp->lastAppearOn->firstAppearAt->next = NULL;
    gTokenTemp->lastAppearOn->firstAppearAt->column = column;
    gTokenTemp->lastAppearOn->lastAppearAt = gTokenTemp->lastAppearOn->firstAppearAt;
  }
}

void CompareToken(CharPtr catched)
{
  if (strcmp(catched, gTokenTemp->tokenStr) < 0)
  {
    return;
  }
  else
  {
    if (strcmp(catched, gTokenTemp->tokenStr) > 0 && gTokenTemp->next == NULL)
      return;
    while (gTokenTemp->next != NULL)
    {
      if (strcmp(catched, gTokenTemp->tokenStr) > 0 && strcmp(catched, gTokenTemp->next->tokenStr) < 0)
        return;
      gTokenTemp = gTokenTemp->next;
    }
  }
}

void InsertDifferentToken(CharPtr catched, TokenType type, int line, int column)
{
  if (gTokenTemp->next == NULL && strcmp(catched, gTokenTemp->tokenStr) >= 0)
  {
    gTokenTemp->next = new Token;
    if (type == IDENTIFIER)
      uIdentifier = uIdentifier + 1;
    else if (type == CONSTANT)
      uConstant = uConstant + 1;
    else if (type == SPECIAL)
      uSpecial = uSpecial + 1;
    gTokenTemp->next->next = NULL;
    gTokenTemp->next->tokenStr = new char[strlen(catched) + 1];
    strcpy(gTokenTemp->next->tokenStr, catched);
    gTokenTemp->next->type = type;
    gTokenTemp->next->firstAppearOn = new Line;
    gTokenTemp->next->firstAppearOn->next = NULL;
    gTokenTemp->next->firstAppearOn->line = line;
    gTokenTemp->next->lastAppearOn = gTokenTemp->next->firstAppearOn;
    gTokenTemp->next->firstAppearOn->firstAppearAt = new Column;
    gTokenTemp->next->firstAppearOn->firstAppearAt->next = NULL;
    gTokenTemp->next->firstAppearOn->firstAppearAt->column = column;
    gTokenTemp->next->firstAppearOn->lastAppearAt = gTokenTemp->next->firstAppearOn->firstAppearAt;
  }
  else if (strcmp(catched, gTokenTemp->tokenStr) > 0 && strcmp(catched, gTokenTemp->next->tokenStr) < 0)
  {
    gBetweenTemp = gTokenTemp->next;
    gTokenTemp->next = new Token;
    if (type == IDENTIFIER)
      uIdentifier = uIdentifier + 1;
    else if (type == CONSTANT)
      uConstant = uConstant + 1;
    else if (type == SPECIAL)
      uSpecial = uSpecial + 1;
    gTokenTemp->next->next = NULL;
    gTokenTemp->next->next = gBetweenTemp;
    gTokenTemp->next->tokenStr = new char[strlen(catched) + 1];
    strcpy(gTokenTemp->next->tokenStr, catched);
    gTokenTemp->next->type = type;
    gTokenTemp->next->firstAppearOn = new Line;
    gTokenTemp->next->firstAppearOn->next = NULL;
    gTokenTemp->next->firstAppearOn->line = line;
    gTokenTemp->next->lastAppearOn = gTokenTemp->next->firstAppearOn;
    gTokenTemp->next->firstAppearOn->firstAppearAt = new Column;
    gTokenTemp->next->firstAppearOn->firstAppearAt->next = NULL;
    gTokenTemp->next->firstAppearOn->firstAppearAt->column = column;
    gTokenTemp->next->firstAppearOn->lastAppearAt = gTokenTemp->next->firstAppearOn->firstAppearAt;
  }
  else if (strcmp(catched, gTokenTemp->tokenStr) < 0)
  {
    gTokenTemp = new Token;
    if (type == IDENTIFIER)
      uIdentifier = uIdentifier + 1;
    else if (type == CONSTANT)
      uConstant = uConstant + 1;
    else if (type == SPECIAL)
      uSpecial = uSpecial + 1;
    gTokenTemp->next = gFront;
    gFront = gTokenTemp;
    gTokenTemp->tokenStr = new char[strlen(catched) + 1];
    strcpy(gTokenTemp->tokenStr, catched);
    gTokenTemp->type = type;
    gTokenTemp->firstAppearOn = new Line;
    gTokenTemp->firstAppearOn->next = NULL;
    gTokenTemp->firstAppearOn->line = line;
    gTokenTemp->lastAppearOn = gTokenTemp->firstAppearOn;
    gTokenTemp->firstAppearOn->firstAppearAt = new Column;
    gTokenTemp->firstAppearOn->firstAppearAt->next = NULL;
    gTokenTemp->firstAppearOn->firstAppearAt->column = column;
    gTokenTemp->firstAppearOn->lastAppearAt = gTokenTemp->firstAppearOn->firstAppearAt;
  }
}

bool InsertToken(CharPtr catched, TokenType type, int column, int line)
{
  bool same = false;
  if (strcmp(catched, "END_OF_FILE") != 0)
  {
    if (gFront == NULL)
    {
      Token *firstToken = NULL;
      firstToken = new Token;
      if (type == IDENTIFIER)
        uIdentifier = uIdentifier + 1;
      else if (type == CONSTANT)
        uConstant = uConstant + 1;
      else if (type == SPECIAL)
        uSpecial = uSpecial + 1;
      gFront = firstToken;
      firstToken->next = NULL;
      firstToken->tokenStr = new char[strlen(catched) + 1];
      strcpy(firstToken->tokenStr, catched);
      firstToken->type = type;
      firstToken->firstAppearOn = new Line;
      firstToken->firstAppearOn->next = NULL;
      firstToken->firstAppearOn->line = line;
      firstToken->lastAppearOn = firstToken->firstAppearOn;
      firstToken->firstAppearOn->firstAppearAt = new Column;
      firstToken->firstAppearOn->firstAppearAt->next = NULL;
      firstToken->firstAppearOn->firstAppearAt->column = column;
      firstToken->firstAppearOn->lastAppearAt = firstToken->firstAppearOn->firstAppearAt;
    }
    else
    {
      gTokenTemp = gFront;
      same = FindAnySame(catched);
      if (same)
        InsertSameToken(catched, line, column);
      else
      {
        gTokenTemp = gFront;
        CompareToken(catched);
        InsertDifferentToken(catched, type, line, column);
      }
    }
    return false;
  }
  else
    return true;
}

void PrintTokenInfo(int i)
{
  TokenPtr current = gFront;
  for (int j = 0; current != NULL && j < i; j++)
    current = current->next;
  printf("%s ", current->tokenStr);
  LinePtr lineWalk = NULL;
  ColumnPtr columnWalk = NULL;
  for (lineWalk = current->firstAppearOn; lineWalk != NULL; lineWalk = lineWalk->next)
    for (columnWalk = lineWalk->firstAppearAt; columnWalk != NULL; columnWalk = columnWalk->next)
      printf("(%d,%d)", lineWalk->line, columnWalk->column);
  printf("\n");
}

bool PrintChosenToken(Str100 chosenToken)
{
  while (gTokenTemp != NULL && strcmp(gTokenTemp->tokenStr, chosenToken) != 0)
  {
    gTokenTemp = gTokenTemp->next;
  }
  if (gTokenTemp == NULL)
    return false;
  else
  {
    printf("%s ", gTokenTemp->tokenStr);
    LinePtr lineWalk = NULL;
    ColumnPtr columnWalk = NULL;
    for (lineWalk = gTokenTemp->firstAppearOn; lineWalk != NULL; lineWalk = lineWalk->next)
      for (columnWalk = lineWalk->firstAppearAt; columnWalk != NULL; columnWalk = columnWalk->next)
        printf("(%d,%d)", lineWalk->line, columnWalk->column);
    printf("\n\n");
    return true;
  }
}

bool PrintChosenLine(int line)
{
  bool printOrNot = false;
  while (gTokenTemp != NULL)
  {
    LinePtr lineWalk = NULL;
    lineWalk = gTokenTemp->firstAppearOn;
    while (lineWalk != NULL)
    {
      if (lineWalk->line == line)
      {
        printf("%s\n", gTokenTemp->tokenStr);
        printOrNot = true;
      }
      lineWalk = lineWalk->next;
    }
    gTokenTemp = gTokenTemp->next;
  }
  return printOrNot;
}

int main()
{
  char ch = '\0';
  bool endOfFile = false;
  bool comment = false;
  int column = 0;
  int line = 0;
  int i = 0;
  int command = 0;
  int chosenLine = 0;
  Str100 chosenToken = "\0";
  while (NOT endOfFile)
  {
    ch = GetChar();
    CharPtr currentToken = "\0";
    if (WhichType(ch) == IDENTIFIER)
    {
      currentToken = ReadRemaining(ch, IDENTIFIER, column, line, comment);
      endOfFile = InsertToken(currentToken, IDENTIFIER, column, line);
    }
    else if (WhichType(ch) == CONSTANT)
    {
      currentToken = ReadRemaining(ch, CONSTANT, column, line, comment);
      endOfFile = InsertToken(currentToken, CONSTANT, column, line);
    }
    else if (WhichType(ch) == SPECIAL)
    {
      currentToken = ReadRemaining(ch, SPECIAL, column, line, comment);
      if (NOT comment)
        endOfFile = InsertToken(currentToken, SPECIAL, column, line);
    }
  }
  gTokenTemp = gFront;
  while (gTokenTemp != NULL)
  {
    PrintTokenInfo(i);
    i = i + 1;
    gTokenTemp = gTokenTemp->next;
  }

  scanf("%d", &command);
  while (command != 5)
  {
    if (command == 1)
      printf("%d\n\n", uIdentifier + uConstant + uSpecial);
    else if (command == 2)
      printf("Identifier %d\nConstant %d\nSpecial %d\n\n", uIdentifier, uConstant, uSpecial);
    else if (command == 3)
    {
      printf("token\n");
      scanf(" %[^\r\n]s", chosenToken);
      gTokenTemp = gFront;
      if (NOT PrintChosenToken(chosenToken))
      {
        printf("%s", chosenToken);
        printf(" token : %s\n\n", chosenToken);
      }
    }
    else if (command == 4)
    {
      printf(" line :\n");
      scanf("%d", &chosenLine);
      gTokenTemp = gFront;
      if (NOT PrintChosenLine(chosenLine))
        printf("\n\n");
    }
    scanf("%d", &command);
  }
  printf("byebye\n");
}
