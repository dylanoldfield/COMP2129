#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "snapshot.h"

entry* entry_head = NULL;
entry* entry_tail = NULL;

snapshot* snapshot_head = NULL;
snapshot* snapshot_tail = NULL;

entry* getEntry(char* key){
	entry *current = entry_head;
	while(current!= NULL){
		if(strcmp(current->key, key) == 0){
			return current;
		}
		current = current->next;
	}
	return NULL;
}

snapshot* getSnapshot(int id){
	snapshot *current = snapshot_head;
	while(current!= NULL){
		if(current->id == id){
			return current;
		}
		current = current->next;
	}
	return NULL;
}

void deleteAllValue(value* value){
	if(value != NULL){
		deleteAllValue(value->next);
		free(value);
	}
}

void deleteAllEntry(entry* entry){
	if(entry != NULL){
		deleteAllValue(entry->values);	
		deleteAllEntry(entry->next);
		free(entry);
	}
}

void deleteAllSnapshot(snapshot* snapshot){
	if(snapshot != NULL){
		deleteAllEntry(snapshot->entries);
		deleteAllSnapshot(snapshot->next);
		free(snapshot);
	}
}

void deleteEntry(entry* entry){
	deleteAllValue(entry->values);
	
	if(entry_head == entry && entry_tail == entry){
		entry_head = NULL;
		entry_tail = NULL;
	}else if(entry_head != entry && entry_tail == entry){
		entry->prev->next = NULL;
		entry_tail = entry->prev;
	}else if(entry_head == entry && entry_tail != entry){
		entry->next->prev = NULL;
		entry_head = entry->next;
	}else{
		entry->next->prev = entry->prev;
		entry->prev->next = entry->next;
	}
	free(entry);
	
}

void deleteSnapshot(snapshot* snapshot){
	deleteAllEntry(snapshot->entries);

	if(snapshot_head == snapshot && snapshot_tail == snapshot){
		snapshot_head = NULL;
		snapshot_tail = NULL;
	}else if(snapshot_head != snapshot && snapshot_tail == snapshot){
		snapshot->prev->next = NULL;
		snapshot_tail = snapshot->prev;
	}else if(snapshot_head == snapshot && snapshot_tail != snapshot){
		snapshot->next->prev = NULL;
		snapshot_head = snapshot->next;
	}else{
		snapshot->next->prev = snapshot->prev;
		snapshot->prev->next = snapshot->next;
	}
	free(snapshot);
}


void cmd_help(void){
	printf("BYE   clear database and exit\n");
	printf("HELP  display this help message\n\n");

	printf("LIST KEYS       displays all keys in current state\n");
	printf("LIST ENTRIES    displays all entries in current state\n");
	printf("LIST SNAPSHOTS  displays all snapshots in the database\n\n");

	printf("GET <key>    displays entry values\n");
	printf("DEL <key>    deletes entry from current state\n");
	printf("PURGE <key>  deletes entry from current state and snapshots\n\n");

	printf("SET <key> <value ...>     sets entry values\n");
	printf("PUSH <key> <value ...>    pushes each value to the front one at a time\n");
	printf("APPEND <key> <value ...>  append each value to the back one at a time\n\n");

	printf("PICK <key> <index>   displays entry value at index\n");
	printf("PLUCK <key> <index>  displays and removes entry value at index\n");
	printf("POP <key>            displays and removes the front entry value\n\n");

	printf("DROP <id>      deletes snapshot\n");
	printf("ROLLBACK <id>  restores to snapshot and deletes newer snapshots\n");
	printf("CHECKOUT <id>  replaces current state with a copy of snapshot\n");
	printf("SNAPSHOT       saves the current state as a snapshot\n\n");

	printf("MIN <key>  displays minimum entry value\n");
	printf("MAX <key>  displays maximum entry value\n");
	printf("SUM <key>  displays sum of entry values\n");
	printf("LEN <key>  displays number of entry values\n\n");

	printf("REV <key>   reverses order of entry values\n");
	printf("UNIQ <key>  removes repeated adjacent entry values\n");
	printf("SORT <key>  sorts entry values in ascending order\n\n");

}

void cmd_list_keys(void){
	if (entry_head == NULL){
		printf("no keys\n");
	}else {
		entry *current = entry_head;
		while(current != NULL){
			printf("%s\n",current->key);
			current = current->next;
		}
		printf("\n");
	}
}

void cmd_list_entries(void){
	if (entry_head == NULL){
		printf("no entries\n");
	}else {
		entry *current = entry_head;
		while(current != NULL){
			value *currentValue = current->values;
			if(currentValue == NULL){
				printf("%s []\n",current->key);
				return;
			}
			printf("%s [",current->key);
			while(currentValue != NULL){
				if(currentValue->next != NULL){
					printf("%d ",currentValue->value);
				}else{
					printf("%d]\n",currentValue->value);
				}
				currentValue = currentValue->next;
			}
			current = current->next;
		}
	}
}

void cmd_list_snapshots(void){
	if (snapshot_head == NULL){
		printf("no snapshots\n");
	}else {
		snapshot *current = snapshot_head;
		while(current != NULL){
			printf("%d ",current->id);
			current = current->next;
		}
		printf("\n");
	}
}

void cmd_get(char* key){
	entry *entry = getEntry(key);
	if (entry != NULL){
		value *currentValue = entry->values;
		if(currentValue == NULL){
			printf("[]\n");
			return;
		}
		printf("[");
		while(currentValue != NULL){
			if(currentValue->next != NULL){
				printf("%d ", currentValue->value);
				currentValue = currentValue->next;
			}else{
				printf("%d]\n", currentValue->value);
				return;
			}
		}
		
	}
	printf("no such key\n");
}

void cmd_del(char* key){
	entry *entry = getEntry(key);
	if(entry != NULL){
		deleteEntry(entry);
		printf("ok\n");
		return;
	}
	printf("no such key\n");
}

void cmd_purge(char* key){
	entry *currentEntry = getEntry(key);
	snapshot *currentSnapshot = snapshot_head;
	if(currentEntry != NULL){
		deleteEntry(currentEntry);
	}
	while (currentSnapshot != NULL){
		currentEntry = currentSnapshot->entries;
		while(currentEntry != NULL){
			if(strcmp(key, currentEntry->key) == 0){
				deleteAllValue(currentEntry->values);
				if(currentSnapshot->entries == currentEntry){
					currentSnapshot->entries = currentEntry->next;
					currentEntry->next->prev = NULL;
				}else{
					currentEntry->prev->next = currentEntry->next;
					currentEntry->next->prev = currentEntry->prev;
				}
				free(currentEntry);
			}
			currentEntry = currentEntry->next;
		}
		currentSnapshot = currentSnapshot->next;
	}
	printf("ok\n");
}

void cmd_set(char* key, int* values, size_t size){
	entry *newEntry = getEntry(key);
	value *prevValue = NULL;
	int i;	
	if ( newEntry == NULL ){
		newEntry = (entry*)malloc(sizeof(*newEntry));
		strcpy(newEntry->key, key);
		newEntry->next = NULL;
		newEntry->prev = NULL;
		newEntry->values = NULL;

		if(entry_head == NULL){
			entry_head = newEntry;
			entry_tail = newEntry;
		}else{
			entry_head->prev = newEntry;
			newEntry->next = entry_head;
			entry_head = newEntry;
		}
	}else{
		value *currentValue = newEntry->values;
		value *nextValue;
		while(currentValue != NULL){
			nextValue = currentValue->next;
			free(currentValue);
			currentValue = nextValue;
		}
	}
	for( i = 0; i < size; i++){
		value *newValue = (value*)malloc(sizeof(*newValue));
		newValue->prev = prevValue;
		newValue->next = NULL;
		newValue->value = values[i];
		if(prevValue == NULL){
			newEntry->values = newValue;
			prevValue = newValue;
		}else{
			prevValue->next = newValue;
			prevValue = newValue;
		}
	}
	printf("ok\n");
}

void cmd_push(char* key, int* values, size_t size){
	entry *newEntry = getEntry(key);
	value *currentValue;
	int i;

	if(newEntry == NULL){
		printf("no such key\n");
		return;
	}
	currentValue = newEntry->values;
	
	for( i = 0; i < size; i++ ){
		value *newValue = (value*)malloc(sizeof(*newValue));
		newValue->value = values[i];
		newValue->prev = NULL;
		newValue->next = NULL;
		newEntry->values = newValue;
		
		if(currentValue != NULL){
			currentValue->prev = newValue;
			newValue->next = currentValue;
		}
		currentValue = newValue;
	}
	printf("ok\n");
	

}

void cmd_append(char* key, int* values, size_t size){
	entry *newEntry = getEntry(key);
	value *currentValue, *previousValue;
	int i;

	if(newEntry == NULL){
		printf("no such key\n");
		return;
	}
	
	currentValue = newEntry->values;
	previousValue = NULL;

	while(currentValue != NULL){
		previousValue = currentValue;
		currentValue = currentValue->next;
	}
		
	for( i = 0; i < size; i++ ){
		value *newValue = (value*)malloc(sizeof(*newValue));
		newValue->value = values[i];
		newValue->prev = NULL;
		newValue->next = NULL;
		if(previousValue != NULL){
			previousValue->next = newValue;
			newValue->prev = previousValue;
		}else{
			newEntry->values = newValue;
		}
		previousValue = newValue;
	}
	printf("ok\n");
	
}

void cmd_pick(char* key, int index){
	entry* entry = getEntry(key);
	if(entry != NULL){
		value* currentValue = entry->values;
		int i = 1;
		while(currentValue != NULL){
			if(i == index){
				printf("%d\n", currentValue->value);
				return;
			}
			currentValue = currentValue->next;
			i++;
		}
		printf("index out of range\n");
		return;
	}
	printf("no such key\n");
}

void cmd_pluck(char* key, int index){
	entry* entry = getEntry(key);
	if(entry != NULL){
		value* currentValue = entry->values;
		int i = 1;
		while(currentValue != NULL){
			if(i == index){
				printf("%d", currentValue->value);
				if(index == 1){
					if( currentValue->next != NULL ){
						entry->values = currentValue->next;
						currentValue->next->prev = NULL;	
					}else{
						entry->values = NULL;
					}
				}else if(currentValue->next == NULL){
					currentValue->prev->next = currentValue->next;
				}else{
					currentValue->prev->next = currentValue->next;
					currentValue->next->prev = currentValue->prev;
				}
				free(currentValue);
				return;
			}
			currentValue = currentValue->next;
			i++;
		}
		printf("index out of range\n");
		return;
	}
	printf("no such key\n");
}

void cmd_pop(char* key){
	entry* entry = getEntry(key);
	value* value;
	if(entry != NULL){
		if( entry->values != NULL ){
			value = entry->values;
			printf("%d\n", value->value);
			if(value->next != NULL){
				entry->values = value->next;
				value->next->prev = NULL;
			}else{
				entry->values = NULL;
			}
			free(value);
			return;
		}
		printf("nil\n");
		return;
	}
	printf("no such key\n");
}

void cmd_drop(int id){
	snapshot* snapshot = getSnapshot(id);
	if(snapshot != NULL){
		deleteSnapshot(snapshot);
		printf("ok\n");
		return;
	}
	printf("no such snapshot\n");
}

void cmd_checkout(int id){
	snapshot* checkoutSnapshot = getSnapshot(id);
	entry* currentEntry = entry_head;
	entry* previousEntry;
	entry* duplicateEntry;

	if(checkoutSnapshot == NULL){
		printf("no such snapshot\n");
		return;
	}

	while(currentEntry != NULL){
		previousEntry = currentEntry;
		currentEntry = currentEntry->next;
		deleteEntry(previousEntry);
	}
	currentEntry = checkoutSnapshot->entries;
	
	while(currentEntry != NULL){
		value *currentValue, *duplicateValue = NULL;

		entry* newEntry = (entry*)malloc(sizeof(*newEntry));
		strcpy(newEntry->key, currentEntry->key);
		newEntry->next = NULL;
		newEntry->prev = NULL;
		newEntry->values = NULL;
		if(currentEntry->prev == NULL){
			newEntry->prev = NULL;
			entry_head = newEntry;
		}else{
			newEntry->prev = duplicateEntry;
			duplicateEntry->next = newEntry;
		}
		duplicateEntry = newEntry;
		currentValue = currentEntry->values;
		
		while(currentValue != NULL){
			value* newValue = (value*)malloc(sizeof(*newValue));
			newValue->value = currentValue->value;
			newValue->next = NULL;
			newValue->prev = NULL;
			if(currentValue->prev == NULL){
				newEntry->values = newValue;
			}else{
				newValue->prev = duplicateValue;
				duplicateValue->next = newValue;
			}
			duplicateValue = newValue;
			currentValue = currentValue->next;
		}
		currentEntry = currentEntry->next;
	}
	entry_tail = duplicateEntry;
	printf("ok\n");
}

void cmd_rollback(int id){
	snapshot* rollbackSnapshot = getSnapshot(id);
	if(rollbackSnapshot != NULL){
		snapshot* currentSnapshot = snapshot_head;
		while(currentSnapshot != rollbackSnapshot){
			snapshot* nextSnapshot = currentSnapshot->next;
			deleteSnapshot(currentSnapshot);
			currentSnapshot = nextSnapshot;
		}
		cmd_checkout(id);
	}else{
		printf("no such snapshot\n");
	}
}

void cmd_snapshot(){
	snapshot *snapshot = malloc(sizeof(*snapshot));
	entry *currentEntry, *duplicateEntry;
	snapshot->prev = NULL;
	if(snapshot_head == NULL){
		snapshot->id = 1;
		snapshot->next = NULL;
		snapshot_tail = snapshot;
		snapshot_head = snapshot;
	}else{
		snapshot->id = snapshot_head->id+1;
		snapshot->next = snapshot_head;
		snapshot_head->prev = snapshot;
		snapshot_head = snapshot;
	}
	currentEntry = entry_head;
	while(currentEntry != NULL){
		value *currentValue, *duplicateValue = NULL;
		entry* newEntry = (entry*)malloc(sizeof(*newEntry));
		strcpy(newEntry->key, currentEntry->key);
		newEntry->next = NULL;
		newEntry->prev = NULL;
		if(currentEntry->prev == NULL){
			newEntry->prev = NULL;
			snapshot->entries = newEntry;
		}else{
			newEntry->prev = duplicateEntry;
			duplicateEntry->next = newEntry;
		}
		duplicateEntry = newEntry;
		currentValue = currentEntry->values;

		while(currentValue != NULL){
			value* newValue = (value*)malloc(sizeof(*newValue));
			newValue->value = currentValue->value;
			newValue->next = NULL;
			newValue->prev = NULL;
			if(currentValue->prev == NULL){
				newEntry->values = newValue;
			}else{
				newValue->prev = duplicateValue;
				duplicateValue->next = newValue;
			}
			duplicateValue = newValue;
			currentValue = currentValue->next;
		}
		currentEntry = currentEntry->next;
	}
	printf("saved as snapshot %d\n", snapshot->id);

}

void cmd_min(char* key){
	entry* entry = getEntry(key);
	if(entry != NULL){
		int min = INT_MAX;
		value* currentValue = entry->values;
		while(currentValue != NULL){
			if(currentValue->value < min){
				min = currentValue->value;
			}
			currentValue = currentValue->next;
		}
		printf("%d\n", min);
		return;
	}
	printf("no such key\n");
}

void cmd_max(char* key){
	entry* entry = getEntry(key);
	if(entry != NULL){
		int max = INT_MIN;
		value* currentValue = entry->values;
		while(currentValue != NULL){
			if(currentValue->value > max){
				max = currentValue->value;
			}
			currentValue = currentValue->next;
		}
		printf("%d\n", max);
		return;
	}
	printf("no such key\n");
}

void cmd_sum(char* key){
	entry* entry = getEntry(key);
	if(entry != NULL){
		int total = 0;
		value* currentValue = entry->values;
		while(currentValue != NULL){
			total += currentValue->value;
			currentValue = currentValue->next;
		}
		printf("%d\n", total);
		return;
	}
	printf("no such key\n");
}

void cmd_len(char* key){
	entry* entry = getEntry(key);
	if(entry != NULL){
		int total = 0;
		value* currentValue = entry->values;
		while(currentValue != NULL){
			total++;
			currentValue = currentValue->next;
		}
		printf("%d\n", total);
		return;
	}
	printf("no such key\n");
}

void cmd_rev(char* key){
	entry *currentEntry = getEntry(key);
	if(currentEntry != NULL){
		value *currentValue = currentEntry->values;
		while (currentValue != NULL){
			value *temp = currentValue->next;
			currentValue->next = currentValue->prev;
			currentValue->prev = temp;
			currentEntry->values = currentValue;
			currentValue = currentValue->prev;
		}
		printf("ok\n");
	}else{
		printf("no such key\n");
	}
}

void cmd_uniq(char* key){
	entry *currentEntry = getEntry(key);
	if(currentEntry != NULL){
		value *currentValue = currentEntry->values;
		value *currentSearchValue = NULL;

		while (currentValue != NULL){
			currentSearchValue = currentValue->next;
			while(currentSearchValue != NULL){
				value *nextSearchValue = currentSearchValue->next;
				if(currentValue->value == currentSearchValue->value){
					if(currentSearchValue->next == NULL && currentSearchValue->prev == NULL){
						currentEntry->values = NULL;
						free(currentSearchValue);
					}else if(currentSearchValue->next != NULL && currentSearchValue->prev == NULL){
						currentSearchValue->next->prev = NULL;
						currentEntry->values = currentSearchValue->next;
						free(currentSearchValue);
					}else if(currentSearchValue->next == NULL && currentSearchValue->prev != NULL){
						currentSearchValue->prev->next = NULL;
						free(currentSearchValue);
					}else{
						currentSearchValue->prev->next = currentSearchValue->next;
						currentSearchValue->next->prev = currentSearchValue->prev;
						free(currentSearchValue);
					}
				}else{
					break;
				}
				currentSearchValue = nextSearchValue;
			}
			currentValue = currentValue->next;
		}
		printf("ok\n");
	}else{
		printf("no such key\n");
	}
}

void cmd_sort(char* key){
	entry *currentEntry = getEntry(key);
	if(currentEntry != NULL){
		//implementation of bubble sort
		//first sort with unknown number of elements
		value *currentValue = currentEntry->values;
		int count = 0, i = 1;
		while(currentValue != NULL && currentValue->next != NULL){
			if(currentValue->value > currentValue->next->value){
				int temp = currentValue->value;
				currentValue->value = currentValue->next->value;
				currentValue->next->value = temp;
			}
			currentValue = currentValue->next;
			count++;
		}
		//sort count-1 more times
		for(; count > 0; count--){
			currentValue = currentEntry->values;
			for( i = 1; i < count; i++){
				if(currentValue->value > currentValue->next->value){
					int temp = currentValue->value;
					currentValue->value = currentValue->next->value;
					currentValue->next->value = temp;
				}
				currentValue = currentValue->next;
			}
		}
		printf("ok\n");
	}else{
		printf("no such key\n");
	}
}






int main(void) {
	int isRunning = 1;
	char buffer[MAX_LINE_LENGTH];
	char command[MAX_LINE_LENGTH];
	while(isRunning){	
		//int i;
		printf("> ");
		fgets(buffer, MAX_LINE_LENGTH, stdin);
		/*for( i = 0; i < MAX_LINE_LENGTH; i++ ){
		  buffer[i] = tolower(buffer[i]);
		}*/
		sscanf(buffer, " %s ", (char*)&command);
		//Switch for commands
		if(strcmp(command, "BYE") == 0){					/*BYE*/
			deleteAllEntry(entry_head);
			deleteAllSnapshot(snapshot_head);
			printf("bye");
			isRunning = 0;
		}else if(strcmp(command, "HELP") == 0){				/*HELP*/
			cmd_help();
		}else if(strcmp(command, "LIST") == 0){
			sscanf(buffer + strlen(command), "%s", (char*)&command);
			if(strcmp(command, "KEYS") == 0){				/*LIST KEYS*/
				cmd_list_keys();
			}else if(strcmp(command, "ENTRIES") == 0){		/*LIST ENTRIES*/
				cmd_list_entries();
			}else if(strcmp(command, "SNAPSHOTS") == 0){	/*LIST SNAPSHOTS*/
				cmd_list_snapshots();
			}
		}else if(strcmp(command, "GET") == 0){				/*GET*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_get(key);
		}else if(strcmp(command, "DEL") == 0){				/*DEL*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_del(key);
		}else if(strcmp(command, "PURGE") == 0){			/*PURGE*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_purge(key);
		}else if(strcmp(command, "SET") == 0){				/*SET*/
			int i = 0;
			char key[MAX_KEY_LENGTH];
			char *endptr;
			int *values = (int*)malloc(MAX_LINE_LENGTH/2*sizeof(int));
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			endptr = buffer + strlen(command) + strlen(key) + 2;
			while(1){
				values[i] = strtol(endptr, &endptr, 10);
				if(*endptr == '\n'){
					cmd_set(key, values, i+1);
					break;
				}
				i++;
			}
			free(values);
		}else if(strcmp(command, "PUSH") == 0){				/*PUSH*/
			int i = 0;
			char key[MAX_KEY_LENGTH];
			char *endptr;
			int *values = (int*)malloc(MAX_LINE_LENGTH/2*sizeof(int));
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			endptr = buffer + strlen(command) + strlen(key) + 2;
			while(1){
				values[i] = strtol(endptr, &endptr, 10);
				if(*endptr == '\n'){
					cmd_push(key, values, i+1);
					break;
				}
				i++;
			}
			free(values);
		}else if(strcmp(command, "APPEND") == 0){			/*APPEND*/ 
			int i = 0;
			char key[MAX_KEY_LENGTH];
			char *endptr;
			int *values = (int*)malloc(MAX_LINE_LENGTH/2*sizeof(int));
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			endptr = buffer + strlen(command) + strlen(key) + 2;
			while(1){
				values[i] = strtol(endptr, &endptr, 10);
				if(*endptr == '\n'){
					cmd_append(key, values, i+1);
					break;
				}
				i++;
			}
			free(values);
		}else if(strcmp(command, "PICK") == 0){				/*PICK*/
			char key[MAX_KEY_LENGTH];
			int index;
			sscanf(buffer + strlen(command)+1, "%s", (char*)&key);
			index = strtol(buffer + strlen(command) + strlen(key)+2, (char**)&command, 0);
			cmd_pick(key, index);
		}else if(strcmp(command, "PLUCK") == 0){			/*PLUCK*/
			char key[MAX_KEY_LENGTH];
			int index;
			sscanf(buffer + strlen(command)+1, "%s", (char*)&key);
			index = strtol(buffer + strlen(command) + strlen(key)+2, (char**)&command, 0);
			cmd_pluck(key, index);
		}else if(strcmp(command, "POP") == 0){				/*POP*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_pop(key);
		}else if(strcmp(command, "DROP") == 0){				/*DROP*/
			int id = strtol(buffer + strlen(command) +1 , (char**)&command, 10);
			cmd_drop(id);
		}else if(strcmp(command, "ROLLBACK") == 0){			/*ROLLBACK*/
			int id = strtol(buffer + strlen(command) +1 , (char**)&command, 10);
			cmd_rollback(id);
		}else if(strcmp(command, "CHECKOUT") == 0){			/*CHECKOUT*/
			int id = strtol(buffer + strlen(command) +1 , (char**)&command, 10);
			cmd_checkout(id);
		}else if(strcmp(command, "SNAPSHOT") == 0){			/*SNAPSHOT*/
			cmd_snapshot();
		}else if(strcmp(command, "MIN") == 0){				/*MIN*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_min(key);
		}else if(strcmp(command, "MAX") == 0){				/*MAX*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_max(key);
		}else if(strcmp(command, "SUM") == 0){				/*SUM*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_sum(key);
		}else if(strcmp(command, "LEN") == 0){				/*LEN*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_len(key);
		}else if(strcmp(command, "REV") == 0){				/*REV*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_rev(key);
		}else if(strcmp(command, "UNIQ") == 0){				/*UNIQ*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_uniq(key);
		}else if(strcmp(command, "SORT") == 0){				/*SORT*/
			char key[MAX_KEY_LENGTH];
			sscanf(buffer + strlen(command), "%s", (char*)&key);
			cmd_sort(key);
		}else{
			printf("\n");
		}
	}
	return 0;
}