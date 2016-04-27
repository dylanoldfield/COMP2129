#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "snapshot.h"

//prototypes

entry* getEntry(char* key);
snapshot* getSnap(int id);
void listValues(entry* entry);
void delAllSnapshots(snapshot* snapshot);
void delAllEntries(entry* entry);
void delValues(value* value);
void delEntry(entry* entry);
void delSnap(snapshot* snapshot);
void help(void);
void listKeys(void);
void listEntries(void);
void listSnapshots(void);
void purge(char* key);
void set(char* key, int* values, size_t size);
void push(char* key, int* values, size_t size);
void append(char* key, int* values, size_t size);
void pick(char* key, int index);
void pluck(char* key, int index);
void pop(char* key);
int drop(int id);
int checkout(int id);
int snap(void);
void rollback(int id);
void min(char* key);
void max(char* key);
void sum(char* key);
int len(char* key);
void rev(char* key);
int uniq(char* key);
int sort(char* key);
value* unicheck (value* oldVals, char* key);
entry* uni(char** keys);
value* intercheck (value* oldVals, char* key);
entry* intersect(char** keys);
entry* diff(char** keys);

//Globals for header and tail for entries and Snapshots

entry* entryhead = NULL; 
entry* entrytail = NULL; 

snapshot* snapshothead = NULL;
snapshot* snapshottail = NULL; 

int main(void)
{
    // intialise buffers
    char buffer[MAX_LINE];
    char command[MAX_LINE] = "";
    int  running = 1;
    
    while(running)
    {
           printf("> ");
           //stores input into buffer 
           fgets(buffer, MAX_LINE, stdin);
           // extracts command component from buffer 
           sscanf(buffer,"%s",(char*)&command);
           
           if(strcasecmp(command, "BYE") == 0)
           {
            delAllEntries(entryhead);
			delAllSnapshots(snapshothead);
			printf("bye\n");
			running = 0;
           }
           else if (strcasecmp(command, "HELP") == 0)
           {
               help();
           }
           else if(strcasecmp(command, "LIST") == 0)
           {
               sscanf(buffer + strlen(command), "%s", (char*)&command);
               if(strcasecmp(command,"KEYS") == 0)
               {
                   listKeys();
               }
               else if(strcasecmp(command,"ENTRIES") == 0)
               {
                   listEntries(); 
               }
                else if(strcasecmp(command,"SNAPSHOTS") == 0)
               {
                   listSnapshots();
               }
               
           }
           else if(strcasecmp(command, "GET") == 0)
           {
           		// gets key and corresponding entry
           		char key[MAX_KEY];
           		sscanf(buffer + strlen(command), "%s", (char*)&key );
           		entry* current = getEntry(key);
           		
           		if(current == NULL)
           		{
           			printf("no such key\n");
           		}
           		else 
           		{
           			listValues(current);
           		}
           		
           		printf("\n");
           }
           else if(strcasecmp( command, "DEL") == 0)
           {
           		// gets key and corresponding entry
           		char key[MAX_KEY];
           		sscanf(buffer + strlen(command), "%s", (char*)&key );
           		entry* current = getEntry(key);
           		if(current == NULL)
           		{
           			printf("no such key\n\n");
           		}
           		else
           		{
           			delEntry(current);
           		
           			printf("ok\n\n");
           		}
           }
           else if(strcasecmp( command, "PURGE") == 0)
           {
           		// gets key and corresponding entry
           		char key[MAX_KEY];
           		sscanf(buffer + strlen(command), "%s", (char*)&key );
           		purge(key);
           		
           }
           else if(strcasecmp(command, "SET") == 0)
           {	
	            //gets and sets key
	            char key[MAX_KEY];
	            sscanf(buffer + strlen(command), "%s", (char*)&key);
	           
	            int* values = (int*)malloc((MAX_LINE - strlen(command) - MAX_KEY)* sizeof(int));
	            // end pointer for strtol function +2 for whitespaces
	            char* endptr = buffer + strlen(command) + strlen(key) + 2; 
	            int i = 0;
	           
	            // iterates through remainder of string using strtol to update value array
	            while(1)
	            {
	                 values[i] = strtol(endptr,&endptr, 10);
	                 //checks if no values were inputted
	                 if(endptr == buffer + strlen(command) + strlen(key) + 2)
	                 {
	                 	int* vals = NULL;
	                 	set(key, vals, 0);
	                    break;
	                 }
	                 // else sends values
	                 if(*endptr == '\n')
	                 {
	                     set(key, values, i+1);
	                     break;
	                 }
	                 
	                 i++;
	            }
	            free(values);
	       }
           else if (strcasecmp(command, "PUSH") == 0)
           {
	           	 //gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);
               
               int* values = (int*)malloc((MAX_LINE - strlen(command) - MAX_KEY)* sizeof(int));
               // end pointer for strtol function +2 for whitespaces
               char* endptr = buffer + strlen(command) + strlen(key) + 2; 
               int i = 0;
               
               // iterates through remainder of string using strtol to update value array
               while(1)
               {
                    values[i] = strtol(endptr,&endptr, 10);
                    //checks if no values were inputted
	                if(endptr == buffer + strlen(command) + strlen(key) + 2)
	                {
	                	int* vals = NULL;
	                	set(key, vals, 0);
	                	break;
	                }
                    
                    if(*endptr == '\n')
                    {
                        push(key, values, i+1);
                        break;
                    }
                     
                    i++;
               }
               
               free(values);
           }
           else if (strcasecmp(command, "APPEND") == 0)
           {
	           //gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);
               
               int* values = (int*)malloc((MAX_LINE - strlen(command) - MAX_KEY)* sizeof(int));
               // end pointer for strtol function +2 for whitespaces
               char* endptr = buffer + strlen(command) + strlen(key) + 2; 
               int i = 0;
               
               // iterates through remainder of string using strtol to update value array
               while(1)
               {
                    values[i] = strtol(endptr,&endptr, 10);
                    //checks if no values were inputted
	                if(endptr == buffer + strlen(command) + strlen(key) + 2)
	                {
		                int* vals = NULL;
		                set(key, vals, 0);
		                break;
	                }
	                // else sends values 
                    if(*endptr == '\n')
                    {
                        append(key, values, i+1);
                        break;
                    }
                     
                    i++;
               }
               free(values);
           }
           else if( strcasecmp(command, "PICK") == 0)
           {
          		//gets and sets key & index
           		char key[MAX_KEY];
           		int index;
           		sscanf(buffer + strlen(command), "%s", (char*)&key);
           		index = strtol(buffer + strlen(command) + strlen(key) + 2, NULL, 10);
           		pick(key, index);
               
           }
           else if( strcasecmp(command,"PLUCK") == 0)
           {
           		//gets and sets key & index
           		char key[MAX_KEY];
           		int index;
           		sscanf(buffer + strlen(command), "%s", (char*)&key);
           		index = strtol(buffer + strlen(command) + strlen(key) + 2, NULL, 10);
           		pluck(key, index);
           }
           else if( strcasecmp(command, "POP") == 0)
           {
           		//gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);	
               
               pop(key);
           }
           else if( strcasecmp(command, "DROP") == 0 )
           {
           		//gets id 
           		int id = strtol(buffer + strlen(command) + 1, NULL, 10);
           		if(drop(id) == 1)
           		{
           			printf("no such snapshot\n\n");
           		}
           		else
           		{
           			printf("ok\n\n");	
           		}
           		
           }
            else if( strcasecmp(command, "ROLLBACK") == 0 )
           {
           		//gets id 
           		int id = strtol(buffer + strlen(command) + 1, NULL, 10);
           		rollback(id);
           		
           }
           else if( strcasecmp(command, "CHECKOUT") == 0 )
           {
           		//gets id 
           		int id = strtol(buffer + strlen(command) + 1, NULL, 10);
           		if( checkout(id) == 1)
           		{
           			printf("no such snapshot\n\n");	
           		}
           		else
           		{
           			printf("ok\n\n");
           		}
           		
           }
           
           else if (strcasecmp(command, "SNAPSHOT") == 0)
           {
           		int id = snap();
           		printf("saved as snapshot %d\n\n", id);
           }
           else if( strcasecmp(command, "MIN") == 0)
           {
           		//gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);	
               
               min(key);
           }
           else if( strcasecmp(command, "MAX") == 0)
           {
           		//gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);	
               
               max(key);
           }
           else if( strcasecmp(command, "SUM") == 0)
           {
           		//gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);	
               
               sum(key);
           }
           else if( strcasecmp(command, "LEN") == 0)
           {
           		//gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);	
               int length = len(key);
               if(length != -1)
               {
               		printf("%d\n\n", len(key));
               }
               else
               {
               		printf("no such key\n\n");
               }
           }
           else if( strcasecmp(command, "REV") == 0)
           {
           		//gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);	
               rev(key);	
           }
           else if( strcasecmp(command, "UNIQ") == 0)
           {
           		//gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);	
               int i = uniq(key);	
               if(i == 1)
               {
               		printf("no such key\n\n");
               }
               else
               {
               		printf("ok\n\n");
               }
           }
           else if( strcasecmp(command, "SORT") == 0)
           {
           		//gets and sets key
               char key[MAX_KEY];
               sscanf(buffer + strlen(command), "%s", (char*)&key);	
               if(sort(key) == 1)
               {
               		printf("no such key\n\n");
               }
               else
               {
               		printf("ok\n\n");
               }
           }
           else if( strcasecmp(command, "DIFF") == 0)
           {
	           char* keys[MAX_LINE];
	           keys[0] = strtok(buffer+strlen(command)," \n");
	           int i = 1;
	           
	           while( (keys[i] = strtok(NULL, " \n")) != NULL)
	           {
	               i++;
	           }
	           	
	           entry* vals = diff(keys);
	           if(vals == NULL)
	           {
	           		printf("[]\n\n");
	           }
	           else
	           {
			       listValues(vals);
			       printf("\n");
			       delValues(vals->values);
			       vals->values = NULL;
			       free(vals);
	           }
           }
            else if( strcasecmp(command, "UNION") == 0)
           {
	           char* keys[MAX_LINE];
	           keys[0] = strtok(buffer+strlen(command)," \n");
	           int i = 1;
	           
	           while( (keys[i] = strtok(NULL, " \n")) != NULL)
	           {
	               i++;
	           }
	           	
	           entry* vals = uni(keys);
		       if(vals == NULL)
	           {
	           		printf("[]\n\n");
	           }
	           else
	           {
			       listValues(vals);
			       printf("\n");
			       delValues(vals->values);
			       vals->values = NULL;
			       free(vals);
	           }
	          
           }
           else if( strcasecmp(command, "INTER") == 0)
           {
           	   char* keys[MAX_LINE];
	           keys[0] = strtok(buffer+strlen(command)," \n");
	           int i = 1;
	           
	           while( (keys[i] = strtok(NULL, " \n")) != NULL)
	           {
	               i++;
	           }
	           	
	           entry* vals = intersect(keys);
		       if(vals == NULL)
	           {
	           		printf("[]\n\n");
	           }
	           else
	           {
			       listValues(vals);
			       delValues(vals->values);
			       vals->values = NULL;
			       free(vals);
	           }
	          
           }
           
    }
}
/* --------------------FUNCTIONS in Alphabetical Order------------------------*/

/*
	APPEND: <key> <value ...> appends values to the back
*/
void append(char* key, int* values, size_t size)
{
	// finds corresponding entry
	entry* current = getEntry(key);
	if (current == NULL)
	{
		printf("no such key\n");
	}
	else 
	{
		value* cval = current->values;
		value* prevVal = NULL;
		
		//moves pointer to last value in current entry
		while(cval != NULL)
		{
			prevVal = cval;
			cval = cval->next;
		}
		
		//appends new values to end of current entry
		for (int i = 0; i < size; i++)
		{
			value* newValue = (value*)malloc(sizeof(*newValue));
			newValue->value = values[i];
			newValue->next = NULL;
			newValue->prev = NULL;
			
			if(prevVal != NULL)
			{
				prevVal->next = newValue;
				newValue->prev = prevVal;
			}
			else
			{
				current->values = newValue;
			}
			prevVal = newValue;
			
		}
		printf("ok\n");
	}
	printf("\n");
}
//------------------------------------------------------------------------------

/*
	CHECKOUT:  <id> replaces current state with a copy of snapshot 
*/

int checkout(int id)
{
	snapshot* currentSnap = getSnap(id);
	
	if (currentSnap == NULL)
	{
		return 1;
	}
	

	entry* newState = NULL;
	entry* currentEntry = currentSnap->entries;
	delAllEntries(entryhead);
	entryhead = NULL;
	entrytail = NULL;
	
	// checks to make sure there are entries
	if( currentEntry != NULL)
	{
		//iterates through the entries 
		while(currentEntry != NULL)
		{
			// make room for copy of entry and copy key
			entry* newEntry = (entry*)malloc(sizeof(*newEntry));
			strcpy(newEntry->key, currentEntry->key);
			newEntry->next = NULL;
			newEntry->prev = NULL;
			newEntry->values = NULL;
			
			// check if first entry, sets new entry, places pointer in newSnap
			if (currentEntry->prev == NULL)
			{
				newEntry->prev = NULL;
				entryhead = newEntry;
				
			}
			// else places entry in appropriate spot
			else
			{
				newEntry->prev = newState;
				newState->next = newEntry;
			}
			
			newState = newEntry;
			
			value* currentValue = currentEntry->values;
			value* stateValue; 
			
			// checks to make sure there are values
			if( currentValue != NULL)
			{
				// iterates through all values of current entry copying them
				while(currentValue != NULL)
				{
					value* newValue = (value*)malloc(sizeof(*newValue));
					newValue->value = currentValue->value;
					// check if its the first value
					if (currentValue->prev == NULL)
					{
						newValue->prev = NULL;
						newEntry->values = newValue;
					}
					// places it in appropriate spot
					else
					{
						newValue->prev = stateValue;
						stateValue->next = newValue;
					}
					stateValue = newValue;
					currentValue = currentValue->next;
				}
				// make sure final value points to null
				stateValue->next = NULL;
			}
			currentEntry = currentEntry->next;
			
		}
		// makes sure final entry points to null
		newState->next = NULL;
	}
	entrytail = newState;
	return 0;
}
//------------------------------------------------------------------------------

/*
	DELETE FUNCTIONS
*/

//	Recursively deletes values
void delValues(value* value)
{
	if(value != NULL)
	{
		delValues(value->next);
		free(value);
	}
}

//	Recursively deletes entries
void delAllEntries(entry* entry)
{
	if(entry != NULL)
	{
		delValues(entry->values);	
		delAllEntries(entry->next);
		free(entry);
	}
}

//	Recursivley deletes snapshots
void delAllSnapshots(snapshot* snapshot)
{
	if(snapshot != NULL)
	{
		delAllEntries(snapshot->entries);
		delAllSnapshots(snapshot->next);
		free(snapshot);
	}
}

// Deletes Entries, relinking linked list
void delEntry(entry* entry)
{
	if(entryhead == entry && entrytail == entry)
	{
		entryhead = NULL;
		entrytail = NULL;
	}
	else if (entryhead == entry && entrytail != entry)
	{
		entry->next->prev = NULL;
		entryhead = entry->next;
		
	}
	else if ( entryhead != entry && entrytail == entry)
	{
		entry->prev->next = NULL;
		entrytail = entry->prev;
	}
	else
	{
		entry->prev->next = entry->next;
		entry->next->prev = entry->prev;
	}
	
	delValues(entry->values);
	free(entry);
}

//	Deletes Snapshots, relinking linked list
void delSnap(snapshot* snapshot)
{
	if(snapshothead == snapshot && snapshottail == snapshot)
	{
		snapshothead = NULL;
		snapshottail = NULL;
	}
	else if (snapshothead == snapshot && snapshottail != snapshot)
	{
		snapshot->next->prev = NULL;
		snapshothead = snapshot->next;
		
	}
	else if ( snapshothead != snapshot && snapshottail == snapshot)
	{
		snapshot->prev->next = NULL;
		snapshottail = snapshot->prev;
	}
	else
	{
		snapshot->prev->next = snapshot->next;
		snapshot->next->prev = snapshot->prev;
	}
	
	delAllEntries(snapshot->entries);
	free(snapshot);
}
//------------------------------------------------------------------------------
/*
	DIFF <key> <key ...> displays set difference of values in keys
*/
entry* diff(char** keys)
{
	entry* unionEntry = uni(keys);
	entry* interEntry = intersect(keys);
	entry* uEntry = (entry*)malloc(sizeof(*uEntry));
	uEntry->values = NULL;
	
		
	if(unionEntry == interEntry)
	{
		return uEntry;
	}
	
	if(unionEntry != NULL)
	{

		value* unionValue = unionEntry->values;
		value* interValue = interEntry->values;
		value* prevValue = NULL;
		
		
		while(unionValue != NULL)
		{
			value* newValue = (value*)malloc(sizeof(*newValue));
			newValue->prev = NULL;
			newValue->next = NULL;
			

			if( unionValue != NULL && interValue != NULL)
			{
				if(unionValue->value < interValue->value)
				{
					if(prevValue == NULL)
					{
						uEntry->values = newValue;
					}
					newValue->value = unionValue->value;
					newValue->prev = prevValue;
					if(prevValue != NULL)
					{
						prevValue->next = newValue;
					}
					unionValue = unionValue->next;
					prevValue = newValue;
				}
				else
				{
					unionValue = unionValue->next;
					interValue = interValue->next;
					free(newValue);
				}
			}
			else if( unionValue != NULL)
			{
				if(prevValue == NULL)
					{
						uEntry->values = newValue;
					}
				newValue->value = unionValue->value;
				newValue->prev = prevValue;
				if(prevValue != NULL)
					{
						prevValue->next = newValue;
					}
				unionValue = unionValue->next;
				prevValue = newValue;
			}
			
		}
		if(prevValue != NULL)
		{
			prevValue->next = NULL;
		}
	}

	return uEntry;
}

/*
	DROP <id> deletes snapshop
*/

int drop(int id)
{
	snapshot* currentSnap = getSnap(id);
	
	if (currentSnap == NULL)
	{
		
		return 1;
	}
	
	entry* currentEntry = currentSnap->entries;
	delAllEntries(currentEntry);
	
	if(snapshothead == currentSnap && snapshottail == currentSnap)
	{
		snapshottail = NULL;
		snapshothead = NULL;
		free(currentSnap);
	}
	else if(snapshothead == currentSnap)
	{
		currentSnap->next->prev = NULL;
		snapshothead = currentSnap->next;
		free(currentSnap);
	}
	else if(snapshottail == currentSnap)
	{
		currentSnap->prev->next = NULL;
		snapshottail = currentSnap->prev;
		free(currentSnap);
	}
	else
	{
		currentSnap->next->prev = currentSnap->prev;
		currentSnap->prev->next = currentSnap->next;
		free(currentSnap);
	}
	
	return 0;
	
}



//------------------------------------------------------------------------------
/*
	GET FUNCTIONS
*/

// gets entry to corresponding key
entry* getEntry(char* key)
{
	entry *current = entryhead;
	while(current!= NULL)
	{
		if(strcmp(current->key, key) == 0){
			return current;
		}
		current = current->next;
	}
	return NULL;
}

// gets snapshot to corresponding id
snapshot* getSnap(int id)
{
	snapshot* currentSnap = snapshothead;
	while (currentSnap != NULL)
	{
		if(currentSnap->id == id)
		{
			return currentSnap;
		}
		
		currentSnap = currentSnap->next;
	}
	
	return NULL;
	
}

//------------------------------------------------------------------------------
void help(void)
{
    printf("BYE   clear database and exit\n");
	printf("HELP  display this help message\n\n");

	printf("LIST KEYS       displays all keys in current state\n");
	printf("LIST ENTRIES    displays all entries in current state\n");
	printf("LIST SNAPSHOTS  displays all snapshots in the database\n\n");

	printf("GET <key>    displays entry values\n");
	printf("DEL <key>    deletes entry from current state\n");
	printf("PURGE <key>  deletes entry from current state and snapshots\n\n");

	printf("SET <key> <value ...>     sets entry values\n");
	printf("PUSH <key> <value ...>    pushes values to the front\n");
	printf("APPEND <key> <value ...>  appends values to the back\n\n");

	printf("PICK <key> <index>   displays value at index\n");
	printf("PLUCK <key> <index>  displays and removes value at index\n");
	printf("POP <key>            displays and removes the front value\n\n");

	printf("DROP <id>      deletes snapshot\n");
	printf("ROLLBACK <id>  restores to snapshot and deletes newer snapshots\n");
	printf("CHECKOUT <id>  replaces current state with a copy of snapshot\n");
	printf("SNAPSHOT       saves the current state as a snapshot\n\n");

	printf("MIN <key>  displays minimum value\n");
	printf("MAX <key>  displays maximum value\n");
	printf("SUM <key>  displays sum of values\n");
	printf("LEN <key>  displays number of values\n\n");

	printf("REV <key>   reverses order of values\n");
	printf("UNIQ <key>  removes repeated adjacent values\n");
	printf("SORT <key>  sorts values in ascending order\n\n");
	
	printf("DIFF <key> <key ...>   displays set difference of values in keys\n");
	printf("INTER <key> <key ...>  displays set intersection of values in keys\n");
	printf("UNION <key> <key ...>  displays set union of values in keys\n\n");
	
	
}
//------------------------------------------------------------------------------

/*
	LIST FUNCTIONS
*/

// list the entries 
void listEntries(void)
{
	// checks if there are any entries
	if(entryhead == NULL)
	{
		printf("no entries\n");
	}
	else
	{
		// cycles through entries
		entry* current = entryhead; 
		while(current != NULL)
		{
			printf("%s ", current->key);
			listValues(current);
			current = current ->next;
		}
	}
	printf("\n");
}

// list all keys 
void listKeys(void)
{
	// checks to see if any entries
	if(entryhead == NULL)
	{
		printf("no keys\n\n");
	}
	else
	{
		//prints keys to the entries 
		entry* current = entryhead; 
		
		while(current != NULL)
		{
			printf("%s\n", current->key);
			current = current -> next;
		}
		printf("\n");
	}
}

// list all snapshots
void listSnapshots(void)
{
	if(snapshothead == NULL)
	{
		printf("no snapshots\n");
	}
	else
	{
		snapshot* current = snapshothead;
		while(current != NULL)
		{
			printf("%d\n", current->id);
			current = current->next;
		}
	}
	printf("\n");
}

// list values for a given entry
void listValues(entry* entry)
{
	// prints the values in each entry according to formatting
	value* val = entry->values;
	
	if(val == NULL)
	{
		printf("[]\n");
		return;
	}
	
	printf("[");
	while (val != NULL)
	{
		if(val->next == NULL)
		{
			printf("%d]\n",val->value);
			return;	
		}
		else
		{
			printf("%d ", val->value);
		}
		
		val = val->next;
	}
	val = NULL;
}
//------------------------------------------------------------------------------
/*
	MATHEMATICAL FUNCTIONS
*/


//LEN <key> displays number of values
int len(char* key)
{
	entry* currentEntry = getEntry(key);
	if(currentEntry == NULL)
	{
		return -1;
	}
	value* currentValue = currentEntry->values;
	int len = 0;
	while(currentValue != NULL)
	{
		len++;
		currentValue = currentValue->next;
	}
	
	return len;
}

//MIN <key> displays minimum value
void min(char* key)
{
	entry* currentEntry = getEntry(key);
	if(currentEntry == NULL)
	{
		printf("no such key\n\n");
		return;
	}
	value* currentValue = currentEntry->values;
	int min = currentValue->value;
	while(currentValue != NULL)
	{
		if(currentValue->value < min)
		{
			min = currentValue->value;
		}
		
		currentValue = currentValue->next;
	}
	
	printf("%d\n\n", min);
}

//MAX <key> displays maximum value
void max(char* key)
{
	entry* currentEntry = getEntry(key);
	if(currentEntry == NULL)
	{
		printf("no such key\n\n");
		return;
	}
	value* currentValue = currentEntry->values;
	int max = currentValue->value;
	while(currentValue != NULL)
	{
		if(currentValue->value > max)
		{
			max = currentValue->value;
		}
		
		currentValue = currentValue->next;
	}
	
	printf("%d\n\n", max);
}

//SUM <key> displays sum of values
void sum(char* key)
{
	entry* currentEntry = getEntry(key);
	if(currentEntry == NULL)
	{
		printf("no such key\n\n");
		return;
	}
	value* currentValue = currentEntry->values;
	int sum = 0;
	while(currentValue != NULL)
	{
		sum += currentValue->value;
		currentValue = currentValue->next;
	}
	
	printf("%d\n\n", sum);
}
//------------------------------------------------------------------------------

/*
	PICK <key> <index> displays value at index
*/
void pick(char* key, int index)
{
	// gets corresponding entry
	entry* currentEntry = getEntry(key);
	if(currentEntry == NULL)
	{
		printf("no such key\n");
	}
	else 
	{
		// creates pointer to start of values
		value* currentValue = currentEntry->values;
		int i = 1;
		
		// iterates to matching index
		while(currentValue != NULL)
		{
			if(i == index)
			{
				printf("%d\n\n", currentValue->value);
				return;
			}
			
			currentValue = currentValue->next;
			i++;
		}
		
		printf("index out of range\n");
	}
	
	printf("\n");
}
//------------------------------------------------------------------------------

/*
	PLUCK <key> <index> displays and removes value at index
*/
void pluck(char* key, int index)
{
	// gets corresponding entry
	entry* currentEntry = getEntry(key);
	if(currentEntry == NULL)
	{
		printf("no such key\n");
	}
	else 
	{
		// creates pointer to start of values
		value* currentValue = currentEntry->values;
		int i = 1;
		// iterates to matching index
		while(currentValue != NULL)
		{
			if(i == index)
			{
				printf("%d\n\n", currentValue->value);
				
				if(index == 1 && currentValue->next == NULL)
				{
					free(currentValue);
					currentEntry->values = NULL;
					return;
				}
				
				// checks to see if first value in entry's values
				// deletes value
				if(index == 1)
				{
					currentValue->next->prev = NULL;
					currentEntry->values = currentValue->next;
					free(currentValue);
					return;
				}
				else if(currentValue->next == NULL)
				{
					currentValue->prev->next = NULL;
					free(currentValue);
					return;
				}
				else
				{
					currentValue->next->prev = currentValue->prev;
					currentValue->prev->next = currentValue->next;
					free(currentValue);
					return;
				}
			}
			
			currentValue = currentValue->next;
			i++;
		}
		
		printf("index out of range\n");
	}
	
	printf("\n");
}
//------------------------------------------------------------------------------

/*
	POP <key> displays and removes the front value
*/
void pop(char* key)
{
	entry* currentEntry = getEntry(key);
	
	if( currentEntry == NULL)
	{
		printf("no such key\n\n");
		return;
	}
	else
	{
		value* currentValue = currentEntry->values;
		
		if(currentValue == NULL)
		{
			printf("nil\n\n");
			return;
		}
		else
		{
			printf("%d\n\n", currentValue->value);
			if(currentValue->next != NULL)
			{
				currentValue->next->prev = NULL;
				currentEntry->values = currentValue->next;
			}
			else
			{
				currentEntry->values = NULL;	
			}
			free(currentValue);
			return;
		}
	}
}
//------------------------------------------------------------------------------

/*
	PURGE <key> deletes entry from current state and snapshots
*/
void purge(char* key)
{
	entry* currentEntry = getEntry(key);
	
	if(currentEntry == NULL)
	{
		printf("ok\n\n");
		return;
	}
	else
	{
		delEntry(currentEntry);
		
		snapshot* currentSnap = snapshothead;
		
		//iterates through all snapshots
		while(currentSnap != NULL)
		{
			entry* snapEntry = currentSnap->entries;
			// iterates through all entries in snapshot
			while(snapEntry != NULL)
			{
				// checks for key and deletes entry
				if(strcmp(snapEntry->key, key) == 0)
				{
					delValues(snapEntry->values);
					
					// checks if it's the only entry
					if (snapEntry->prev == NULL && snapEntry->next == NULL)
					{
						currentSnap->entries = NULL;
					}
					// checks if first entry
					else if(snapEntry->prev == NULL)
					{
						currentSnap->entries = snapEntry->next;
						snapEntry->next->prev = NULL;
					}
					// checks if it's the final entry
					else if (snapEntry->next == NULL)
					{
						snapEntry->prev->next = NULL;
					}
					// reorgainizes current snapshot entries
					else
					{
						snapEntry->next->prev = snapEntry->prev;
						snapEntry->prev->next = snapEntry->next;
					}
					free(snapEntry);
					break;
				}
				snapEntry = snapEntry->next;
			}
			
			currentSnap = currentSnap->next;
		}
		
		printf("ok\n\n");
	}
	
}
//------------------------------------------------------------------------------

/*
	PUSH <key> <value ...> pushes values to the front
*/

void push(char* key, int* values, size_t size)
{
	entry* current = getEntry(key);
	
	if( current == NULL)
	{
		printf("no such key\n");
	}
	else
	{
		value* cval = current->values;
		//set values for entry
		for(int i = 0; i < size; i++)
		{
			value* newValue = (value*)malloc(sizeof(*newValue));
			newValue->value = values[i]; 
			newValue->prev = NULL;
			newValue->next = NULL;
			current->values = newValue;
			
			if(cval != NULL)
			{
				cval->prev = newValue;
				newValue->next = cval;
				
			}
			
			cval = newValue;
		}
		printf("ok\n");
	}
	printf("\n");
}
//------------------------------------------------------------------------------

/*
	ROLLBACK <id> restores to snapshot and deletes newer snapshot
*/

void rollback(int id)
{
	snapshot* currentSnap = snapshothead;
	snapshot* headSnap = getSnap(id);
	
	if(currentSnap == NULL)
	{
		printf("no such snapshot\n\n");
		return;
	}
	
	if(headSnap == NULL)
	{
		printf("no such snapshot\n\n");
		return;
	}
	
	while( currentSnap != headSnap)
	{
		snapshot* nextSnap = currentSnap->next;
		delSnap(currentSnap);
		currentSnap = nextSnap;
	}

	checkout(id);
	printf("ok\n\n");
	return;
}

//------------------------------------------------------------------------------


/*
	REV <key> reverses order of value
*/
void rev(char* key)
{
	entry* currentEntry = getEntry(key);
	
	if(currentEntry == NULL)
	{
		printf("no such key\n\n");
		return;
	}
	
	value* currentValue = currentEntry->values;
	int vals[len(key)];
	int i = 0;
	
	while(currentValue != NULL)
	{
		vals[i] = currentValue->value;
		currentValue = currentValue->next;
		i++;
	}
	
	currentValue = currentEntry->values;
	i--;
	while( currentValue != NULL)
	{
		currentValue->value = vals[i];
		currentValue = currentValue->next;
		i--;
		
	}
	
	printf("ok\n\n");
	
}
//------------------------------------------------------------------------------

/*
	SNAPSHOT saves the current state as a snapshot
*/
int snap(void)
{
	//allocate new memory of snapshot
	snapshot* newSnap = (snapshot*)malloc(sizeof(*newSnap));
	entry* currentEntry = NULL; 
	entry* copyEntry = NULL;
	newSnap->prev = NULL;
	
	//checks if it is the first snapshot 
	if(snapshothead == NULL)
	{
		snapshothead = newSnap;
		snapshottail = newSnap;
		newSnap->next = NULL;
		newSnap->id = 1;
	}
	// otherwise places it in the correct spot
	else
	{
		newSnap->id = snapshothead->id+1;
		snapshothead->prev = newSnap;
		newSnap->next = snapshothead;
		snapshothead = newSnap;
	}
	
	// start to copy current state
	currentEntry = entryhead;
	//iterate through all entries in current state
	while(currentEntry != NULL)
	{
		// make room for copy of entry and copy key
		entry* newEntry = (entry*)malloc(sizeof(*newEntry));
		strcpy(newEntry->key, currentEntry->key);
		
		// check if first entry, sets new entry, places pointer in newSnap
		if (currentEntry->prev == NULL)
		{
			newEntry->prev = NULL;
			newSnap->entries = newEntry;
			
		}
		// else places entry in appropriate spot
		else
		{
			newEntry->prev = copyEntry;
			copyEntry->next = newEntry;
		}
		
		copyEntry = newEntry;
		
		value* currentValue = currentEntry->values;
		value* copyValue; 
	
		// iterates through all values of current entry copying them
		while(currentValue != NULL)
		{
			value* newValue = (value*)malloc(sizeof(*newValue));
			newValue->value = currentValue->value;
			// check if its the first value
			if (currentValue->prev == NULL)
			{
				newValue->prev = NULL;
				newEntry->values = newValue;
			}
			// places it in appropriate spot
			else
			{
				newValue->prev = copyValue;
				copyValue->next = newValue;
			}
			copyValue = newValue;
			currentValue = currentValue->next;
		}
		copyValue->next = NULL;
		currentEntry = currentEntry->next;
	}
	if(copyEntry != NULL)
	{
		copyEntry->next = NULL;
	}
	
	return newSnap->id;
}
//------------------------------------------------------------------------------

/*
	SET <key> <value ...> sets entry values
*/

// function is used to set values for an entry given a unique key 
void set(char* key, int* values, size_t size)
{
	// gets the entry struct to corresponding key
	entry *newEntry = getEntry(key);
	value *prevValue = NULL;
	int i;	
	// checks to see if entry exists if not creates it
	if ( newEntry == NULL )
	{
		newEntry = (entry*)malloc(sizeof(*newEntry));
		strcpy(newEntry->key, key);
		newEntry->next = NULL;
		newEntry->prev = NULL;
		newEntry->values = NULL;

		// checks to see if it is the first entry
		if(entryhead == NULL)
		{
			entryhead = newEntry;
			entrytail = newEntry;
		}
		else
		{
			entryhead->prev = newEntry;
			newEntry->next = entryhead;
			entryhead = newEntry;
		}
	}
	// otherwise places it in it's respective place
	else
	{
			delValues(newEntry->values);
			newEntry->values = NULL;
	}
	//set values for entry
	for( i = 0; i < size; i++)
	{
		value *newValue = (value*)malloc(sizeof(*newValue));
		newValue->prev = prevValue;
		newValue->next = NULL;
		newValue->value = values[i];
		
		if(prevValue == NULL)
		{
			newEntry->values = newValue;
			prevValue = newValue;
		}
		else
		{
			prevValue->next = newValue;
			prevValue = newValue;
		}
	}
	printf("ok\n\n");
}

//------------------------------------------------------------------------------

/*
	SORT <key> sorts values in ascending order
*/

int sort(char* key)
{
	entry* currentEntry = getEntry(key);
	
	if (currentEntry == NULL)
	{
		return 1;
	}
	
	int size = len(key);
	value* currentValue = currentEntry->values;
	
	if( currentValue != NULL)
	{
		//bubble sort
		// iterate once through all values
		for(; size > 0; size--)
		{
			currentValue = currentEntry->values;
			
			// iterate n-1 times
			for(int i = 1; i < size; i++)
			{
				// checks and swaps if current is larger than next
				if(currentValue->value > currentValue->next->value)
				{
					int temp = currentValue->value;
					currentValue->value = currentValue->next->value;
					currentValue->next->value = temp;
				}
				currentValue = currentValue->next;
			}
		}
	}
	
	return 0;
}

//------------------------------------------------------------------------------

/*
	UNIQ <key> removes repeated adjacent values
*/
int uniq(char* key)
{
	entry* currentEntry = getEntry(key);
	
	if( currentEntry == NULL)
	{
		return 1;
	}
	value* currentValue = currentEntry->values;
	
	if( currentValue != NULL)
	{
		value* nextVal = currentValue->next;
		
		while (nextVal != NULL)
		{
			if(currentValue->value == nextVal->value)
			{
				if(currentValue == currentEntry->values)
				{
					currentValue->next->prev = NULL;
					currentEntry->values= currentValue->next;
					
				}
				else
				{
					currentValue->next->prev = currentValue->prev;
					currentValue->prev->next = currentValue->next;
				}
				free(currentValue);
			}
			currentValue = nextVal;
			nextVal = nextVal->next;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------

/*
	UNION 
*/

entry* uni(char** keys)
{
	int id = snap();
	entry* uEntry = NULL;
	entry* currentEntry = NULL;
	entry* nextEntry = NULL;
	value* nextValue = NULL;
	char** startkey = keys;
	if(*startkey != NULL)
	{
		currentEntry = getEntry(keys[0]);
	}
	else
	{
		checkout(id);
		drop(id);
		return uEntry;
	}
	
	startkey = startkey + 1;
	
	if(*startkey != NULL)
	{
		nextEntry = getEntry(keys[1]);
	}
	
	if(currentEntry == NULL && nextEntry == NULL)
	{
		checkout(id);
		drop(id);
		return uEntry;
	}
	
	if(currentEntry == nextEntry)
	{
	 	uEntry = (entry*)malloc(sizeof(*uEntry));
	 	uEntry-> values = NULL;
	 	value* prevValue = NULL;
	 	sort(keys[0]);
	 	uniq(keys[0]);
	 	value* currentValue = currentEntry->values;
	 	
	 	while(currentValue != NULL)
	 	{
	 		
	 		value* newValue = (value*)malloc(sizeof(*newValue));
	 		newValue->value = currentValue->value;
	 		newValue->prev = prevValue;
	 		newValue->next = NULL;
	 		if(prevValue == NULL)
	 		{
	 			uEntry->values = newValue;
	 		}
	 		if(prevValue != NULL)
	 		{
	 			prevValue->next = newValue;
	 		}
	 		prevValue = newValue;
	 		currentValue = currentValue->next;
	 	}
	 	
	 	keys = keys + 2;
		while(*keys != NULL)
		{
			uEntry->values = unicheck(uEntry->values, *keys);
			keys = keys + 1;
		}
		
		checkout(id);
		drop(id);
		return uEntry;
	
	}
	else
	{
		
		uEntry = (entry*)malloc(sizeof(*uEntry));
		uEntry->values = NULL;
		
		if(currentEntry != NULL)
		{
			if(nextEntry != NULL)
			{	
				sort(keys[1]);
				uniq(keys[1]);
				nextValue = nextEntry->values;
			}

			sort(keys[0]);
			uniq(keys[0]);

			value* currentValue = currentEntry->values;
			
			value* prevValue = NULL;
			
			
			while(currentValue != NULL || nextValue != NULL)
			{
				value* newValue = (value*)malloc(sizeof(*newValue));
				newValue->prev = NULL;
				newValue->next = NULL;
				
				if(prevValue == NULL)
						{
							uEntry->values = newValue;
						}
	
				if( currentValue != NULL && nextValue != NULL)
				{
					if(currentValue->value < nextValue->value)
					{
						newValue->value = currentValue->value;
						newValue->prev = prevValue;
						if(prevValue != NULL)
						{
							prevValue->next = newValue;
						}
						currentValue = currentValue->next;
					}
					else if (currentValue->value > nextValue->value)
					{
						newValue->value = nextValue->value;
						newValue->prev = prevValue;
						if(prevValue != NULL)
						{
							prevValue->next = newValue;
						}
						nextValue = nextValue->next;
						
					}
					else
					{
						newValue->value = currentValue->value;
						newValue->prev = prevValue;
						if(prevValue != NULL)
						{
							prevValue->next = newValue;
						}
						currentValue = currentValue->next;
						nextValue = nextValue->next;
					}
					prevValue = newValue;	
				}
				else if( currentValue != NULL)
				{
					newValue->value = currentValue->value;
					newValue->prev = prevValue;
					if(prevValue != NULL)
						{
							prevValue->next = newValue;
						}
					currentValue = currentValue->next;
					prevValue = newValue;
				}
				else if( nextValue != NULL )
				{
					newValue->value = nextValue->value;
					newValue->prev = prevValue;
					if(prevValue != NULL)
						{
							prevValue->next = newValue;
						}
					nextValue = nextValue->next;
					prevValue = newValue;
				}
				
			}
			
			prevValue->next = NULL;
	
		}
		
		keys = keys + 2;
		while(*keys != NULL)
		{
			uEntry->values = unicheck(uEntry->values, *keys);
			keys = keys + 1;
		}
		
		checkout(id);
		drop(id);
		
		return uEntry;
			
		
	}
	

}

value* unicheck(value* oldVals, char* key)
{
	entry* currentEntry = getEntry(key);
	
	if(currentEntry == NULL)
	{
		return oldVals;
	}
	
	sort(key);
	uniq(key);
	value* currentValue = currentEntry->values;
	value* nextValue = oldVals;
	value* prevValue = NULL;
	value* firstValue = NULL;
	
	while(currentValue != NULL || nextValue != NULL)
		{
			value* newValue = (value*)malloc(sizeof(*newValue));
			newValue->prev = NULL;
			newValue->next = NULL;
			
			if(prevValue == NULL)
					{
						firstValue = newValue;
					}

			if( currentValue != NULL && nextValue != NULL)
			{
				if(currentValue->value < nextValue->value)
				{
					newValue->value = currentValue->value;
					newValue->prev = prevValue;
					if(prevValue != NULL)
					{
						prevValue->next = newValue;
					}
					currentValue = currentValue->next;
				}
				else if (currentValue->value > nextValue->value)
				{
					newValue->value = nextValue->value;
					newValue->prev = prevValue;
					if(prevValue != NULL)
					{
						prevValue->next = newValue;
					}
					nextValue = nextValue->next;
					
				}
				else
				{
					newValue->value = currentValue->value;
					newValue->prev = prevValue;
					if(prevValue != NULL)
					{
						prevValue->next = newValue;
					}
					currentValue = currentValue->next;
					nextValue = nextValue->next;
				}
				prevValue = newValue;	
			}
			else if( currentValue != NULL)
			{
				newValue->value = currentValue->value;
				newValue->prev = prevValue;
				if(prevValue != NULL)
					{
						prevValue->next = newValue;
					}
				currentValue = currentValue->next;
				prevValue = newValue;
			}
			else if( nextValue != NULL )
			{
				newValue->value = nextValue->value;
				newValue->prev = prevValue;
				if(prevValue != NULL)
					{
						prevValue->next = newValue;
					}
				nextValue = nextValue->next;
				prevValue = newValue;
			}
		
		}
		prevValue->next = NULL;
		delValues(oldVals);
		return firstValue;
}
//------------------------------------------------------------------------------
/*
	INTERSECTION
*/
entry* intersect(char** keys)
{

	int id = snap();
	entry* uEntry = NULL;
	entry* currentEntry = NULL;
	entry* nextEntry = NULL;
	value* nextValue = NULL;
	
	char** startkey = keys;
	if(*startkey != NULL)
	{
		currentEntry = getEntry(keys[0]);
	}
	else
	{
		checkout(id);
		drop(id);
		return uEntry;
	}
	
	startkey = startkey + 1;
	
	if(*startkey != NULL)
	{
		nextEntry = getEntry(keys[1]);
	}

	if(currentEntry == NULL && nextEntry == NULL)
	{
		checkout(id);
		drop(id);
		return uEntry;
	}
	
	
	if(currentEntry == nextEntry)
	{
		uEntry = (entry*)malloc(sizeof(*uEntry));
	 	uEntry-> values = NULL;
	 	value* prevValue = NULL;
	 	sort(keys[0]);
	 	uniq(keys[0]);
	 	value* currentValue = currentEntry->values;
	 	
	 	while(currentValue != NULL)
	 	{
	 		value* newValue = (value*)malloc(sizeof(*newValue));
	 		newValue->value = currentValue->value;
	 		newValue->prev = prevValue;
	 		if(prevValue == NULL)
	 		{
	 			uEntry->values = newValue;
	 		}
	 		newValue->next = NULL;
	 		if(prevValue != NULL)
	 		{
	 			prevValue->next = newValue;
	 		}
	 		prevValue = newValue;
	 		currentValue = currentValue->next;
	 	}
	 	
	 	keys = keys + 2;
		while(*keys != NULL)
		{
			uEntry->values = intercheck(uEntry->values, *keys);
			keys = keys + 1;
		}
		
		checkout(id);
		drop(id);
		return uEntry;
	}
	else
	{
		uEntry = (entry*)malloc(sizeof(*uEntry));
		uEntry->values = NULL;
		
		if(currentEntry != NULL)
		{
			if(nextEntry != NULL)
			{	
				sort(keys[1]);
				uniq(keys[1]);
				nextValue = nextEntry->values;
			}

			sort(keys[0]);
			uniq(keys[0]);
			value* currentValue = currentEntry->values;
			value* prevValue = NULL;
			
			while( currentValue != NULL && nextValue != NULL)
			{
				value* newValue = (value*)malloc(sizeof(*newValue));
				newValue->prev = NULL;
				newValue->next = NULL;
		
				if(currentValue->value < nextValue->value)
				{
					currentValue = currentValue->next;
					free(newValue);
				}
				else if (currentValue->value > nextValue->value)
				{
					nextValue = nextValue->next;
					free(newValue);
				}
				else
				{
							
					if(prevValue == NULL)
					{
						uEntry->values = newValue;
					}
					
					newValue->value = currentValue->value;
					newValue->prev = prevValue;
					
					if(prevValue != NULL)
					{
						prevValue->next = newValue;
					}
					currentValue = currentValue->next;
					nextValue = nextValue->next;
					prevValue = newValue;
				}
			}
			
			if (prevValue != NULL)
			{
				prevValue->next = NULL;
			}
		}
		
		keys = keys + 2;
		while(*keys != NULL)
		{
			uEntry->values = intercheck(uEntry->values, *keys);
			keys = keys + 1;
		}
		
		checkout(id);
		drop(id);
		
		return uEntry;
	}
	
	

}

value* intercheck(value* oldVals, char* key)
{
	entry* currentEntry = getEntry(key);
	
	if(currentEntry == NULL)
	{
		return oldVals;
	}
	
	sort(key);
	uniq(key);
	value* currentValue = currentEntry->values;
	
	if( currentValue == oldVals)
	{
		return oldVals;
	}
	value* nextValue = oldVals;
	value* prevValue = NULL;
	value* firstValue = NULL;
	
			
	while( currentValue != NULL && nextValue != NULL)
	{
		value* newValue = (value*)malloc(sizeof(*newValue));
		newValue->prev = NULL;
		newValue->next = NULL;

		if(currentValue->value < nextValue->value)
		{
			currentValue = currentValue->next;
			free(newValue);
		}
		else if (currentValue->value > nextValue->value)
		{
			nextValue = nextValue->next;
			free(newValue);
		}
		else
		{
					
			if(prevValue == NULL)
			{
				firstValue = newValue;
			}
			
			newValue->value = currentValue->value;
			newValue->prev = prevValue;
			
			if(prevValue != NULL)
			{
				prevValue->next = newValue;
			}
			currentValue = currentValue->next;
			nextValue = nextValue->next;
			prevValue = newValue;
		}

	}
	if(prevValue != NULL)
	{
		prevValue->next = NULL;
	}
	delValues(oldVals);
	return firstValue;
}