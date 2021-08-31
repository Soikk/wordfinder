#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct lineData{
	int lineNumber;
	int occurrences;
	int *positionList;
} lineData;

typedef struct Data{
	char *name;
	int lines;
	lineData *lineList;
} Data;

typedef struct dataNode{
	Data *data;
	struct dataNode *next;
} dataNode;

typedef dataNode* dataHead;


lineData *createLineData(int lineNumber, int occurrences, int *positionList){
	lineData *newLineData = malloc(sizeof(lineData));
	newLineData->lineNumber = lineNumber;
	newLineData->occurrences = occurrences;
	newLineData->positionList = positionList;
	return newLineData;
}

Data *createData(char *name, int lines, lineData *lineList){
	Data *newData = malloc(sizeof(Data));
	newData->name = name;
	newData->lines = lines;
	newData->lineList = lineList;
	return newData;
}

dataNode * createNode(Data *data, dataNode *next){
	dataNode *newNode = malloc(sizeof(dataNode));
	newNode->data = data;
	newNode->next = next;
	return newNode;
}

void addNode(dataHead *head, Data *data){
	dataNode *newNode = createNode(data, NULL);
	if(*head){
		dataNode *temp = *head;
		while(temp->next){
			temp = temp->next;
		}
		temp->next = newNode;
	}else{
		(*head) = newNode;
	}
}

void printLineData(lineData lineData){
	for(int i = 0; i < lineData.occurrences; i++){
		printf("\t\t-At position %d\n", lineData.positionList[i]);
	}
}

void printData(Data data){
	printf("Found %s in %d lines:\n", data.name, data.lines);
	for(int i = 0; i < data.lines; i++){
		printf("\t-At line %d:\n", data.lineList[i].lineNumber);
		printLineData(data.lineList[i]);
	}
}

void printNodes(dataHead head){
	dataNode *node = head;
	while(node){
		printData(*node->data);
		node = node->next;
	}
	free(node);
}

size_t getLineCount(FILE *fp){
	size_t lines = 0;
	while(!feof(fp)){
		char ch = fgetc(fp);
		if(ch == '\n') lines++;
	}
	rewind(fp);
	return ++lines;
}

char **fileToLineArray(FILE *fp, int *fileLines){
	size_t len, l = 0;
	(*fileLines) = getLineCount(fp);
	char **lineArray = (char**)malloc((*fileLines)*sizeof(char*)), *line = NULL;
	while((len = getline(&line, &len, fp)) != -1){
		if(len > 0 && line[len-1] == '\n'){
			line[len-1] = '\0';
		}else if(line[len-1] != '\n'){
			line = (char*)realloc(line, len+1);
			line[len] = '\0';
		}
		lineArray[l++] = line;
		line = NULL;
	}
	rewind(fp);
	return lineArray;
}

int main(int argc, char **argv){
	if(argc < 3){
		return printf("Usage : wordfinder <document> <list>");
	}
	char *docName = argv[1], *listName = argv[2];
	
	FILE *document = fopen(docName, "r"), *list = fopen(listName, "r");
	if(!document) printf("Error: \"%s\" not found\n", document);
	if(!list) printf("Error: \"%s\" not found\n", list);
	if(!document || !list){
		return 0;
	}

	size_t docLines = 0, listLines = 0;
	char **fdocument = fileToLineArray(document, &docLines);
	char **flist = fileToLineArray(list, &listLines);
	fclose(document);
	fclose(list);

	dataNode *head = NULL;
	for(int i = 0; i < listLines; i++){
		int lineCount = 0;
		lineData *lines = malloc(sizeof(lines));
		for(int j = 0; j < docLines; j++){
			char *p1 = fdocument[j], *p2 = fdocument[j];
			int positionCount = 0;
			int *positions = malloc(sizeof(int));
			while(p2 = strstr(p2, flist[i])){
				positions = realloc(positions, (positionCount+1)*sizeof(int));
				int pos = p2 - fdocument[j];
				positions[positionCount++] = pos;
				p2 += strlen(flist[i]);
			}
			if(positionCount){
				lineData *line = createLineData(j+1, positionCount, positions);
				lines = realloc(lines, (lineCount+1)*sizeof(lineData));
				lines[lineCount++] = (*line);	
			}
		}
		if(lineCount){
			Data *data = createData(flist[i], lineCount, lines);
			addNode(&head, data);
		}
	}
	printNodes(head);

	free(head);
	free(fdocument);
	free(flist);

	return 0;
}