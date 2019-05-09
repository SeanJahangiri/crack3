#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#include "md5.h"

const int PASS_LEN=50;        // Maximum any password can be
const int HASH_LEN=33;        // Length of MD5 hash strings


// Stucture to hold both a plaintext password and a hash.
struct entry 
{
    // TODO: FILL THIS IN
    char password[PASS_LEN];
    char hash[HASH_LEN];
};


int file_length(char *fname)
{
    struct stat info;
    int res = stat(fname, &info);
    
    if (res == -1)
    {
        return -1;
    }
    else
    {
        return info.st_size;
    }
}


int cccomp(const void *a, const void *b)
{
    struct entry *ca = (struct entry *)a;
    struct entry *cb = (struct entry *)b;
    return strcmp(ca->password, (*cb).password);
}


int cbcomp(const void *target, const void *elem)
{
    char *target_str = (char *)target;
    struct entry *celem = (struct entry *)elem;
    return strcmp(target_str, (*celem).password);
}


// TODO:
// Read in the dictionary file and return an array of structs.
// Each entry should contain both the hash and the dictionary
// word.
struct entry *read_dictionary(char *filename, int *size)
{
    int filelength = file_length(filename);
    FILE *c = fopen(filename, "r");
    if (!c)
    {
        printf("Can't open %s for reading\n", filename);
        exit(1);
    }
    
    char *contents = malloc(filelength);
    fread(contents, 1, filelength, c);
    fclose(c);
    
    // Loop through contents, replace \n with \0
    int lines = 0;
    for (int i = 0; i < filelength; i++)
    {
        if (contents[i] == '\n')
        {
            contents[i] = '\0';
            lines++;
        }
    }
    //printf("lines %d\n", lines);
    
    // Allocate array of structs
    struct entry *pwords = malloc(lines * sizeof(struct entry));
    
    // Copy the first name into the cc array
    strcpy(pwords[0].password, &contents[0]);
    // pwords[0].hash = strlen(pwords[0].password);
    
    int count = 1;
    for (int i = 0; i < filelength; i++)
    {
        if (contents[i] == '\0')
        {
            char *nextcc = &contents[i] + 1;

            strcpy(pwords[count].password, nextcc);
            // pwords[count].hash = strlen(pwords[count].password);
            count++;
        }
    }
    
    // Sort dictionary by name
    //qsort(pwords, lines, sizeof(struct entry), cccomp);
    
    // print the sorted array
    // for (int i = 0; i < lines; i++)
    // {
    //     printf("%d %s\n", i, pwords[i].password);
    // }
    
    // Loop through the dictionary, convert to hash and load hash array
    for (int i = 0; i < lines; i++)
    {
        int length = strlen(pwords[i].password);
        //char *out = md5(pwords[i].password, length);
        strcpy(pwords[i].hash, md5(pwords[i].password, length));
        //printf("%c\n", *out);
        //printf("%d %s\n", i, pwords[i].password);
    }
    

    //print the password and hash arrays 
    // for (int i = 0; i < lines; i++)
    // {
    //     printf("%d %s %s\n", i, pwords[i].password, pwords[i].hash);
    // }

    
    *size = lines;
    return pwords;
}


int main(int argc, char *argv[])
{
    // TODO: Read the dictionary file into an array of entry structures
    int size;
    struct entry *dict = read_dictionary("rockyou100.txt", &size);
    
    // TODO: Sort the hashed dictionary using qsort.
    // You will need to provide a comparison function.
     qsort(dict->hash, size, sizeof(struct entry), cccomp);

    // TODO:
    // Open the hash file for reading.
    // Allocate array of structs
    int len = file_length("hashes.txt");
    char *contents = malloc(len);
    FILE *f = fopen("hashes.txt", "r");
    if (!f)
    {
        printf("Can't open file for reading\n");
        exit(1);
    }
    
    fread(contents, 1, len, f);
    fclose(f);
 
    int numwords = 0;
    
    for(int i = 0; i < len; i++)
    {
        if(contents[i] == '\n')
        {
            numwords++;
            contents[i] = '\0';
        }
    }
    
   
    // TODO:
    // For each hash, search for it in the dictionary using
    // binary search.
    // If you find 3, get the corresponding plaintext dictionary word.
    // Print out both the hash and the word.
    // Need only one loop. (Yay!)
    int count = 0;
    for (int i = 0; i < len; i++)
    {
        if (count < 3)
        {
            if (contents[i] == '\0')
            {
                char *nextcc = &contents[i] + 1;

                struct entry *found = bsearch(nextcc, dict->hash, size, sizeof(struct entry), cbcomp);
                if (found != NULL)
                {
                    printf("Found Hash: %s\n", nextcc);
                    printf("Found Password: %s\n", dict[i].password);
                    count++;
                }
            }
        }
    } 
    
    // for (int i = 0; i < size; i++)
    // {
    //     printf("%d %s %s\n", i, dict[i].password, dict[i].hash);
    // }
    
     printf("Printed first %d passwords\n", count);

}
