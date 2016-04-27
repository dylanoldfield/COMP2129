#ifndef SNAPSHOT_H 
#define SNAPSHOT_H
#define MAX_KEY 16 
#define MAX_LINE 1024

typedef struct value value;
struct value {
  value* prev;
  value* next;
  int value;
};
 
typedef struct entry entry;
struct entry {
  entry* prev;
  entry* next;
  value* values;
  char key[MAX_KEY];
};
 
typedef struct snapshot snapshot;
struct snapshot {
  snapshot* prev;
  snapshot* next;
  entry* entries;
  int id;
};
 
#endif
