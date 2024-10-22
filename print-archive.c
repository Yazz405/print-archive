#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
* Ackowledegements: https://stackoverflow.com/questions/7009231/cast-unsigned-char-uint8-t-to-const-char
*/

typedef struct __attribute__((packed)) header {
  uint8_t* fileIdentifier;
  int fileSize;
}header_t;

void print_contents(uint8_t* data, size_t size);


int main(int argc, char** argv) {
  // Make sure we have a file input
  if (argc != 2) {
    fprintf(stderr, "Please specify an input filename.\n");
    exit(1);
  }

  // Try to open the file
  FILE* input = fopen(argv[1], "r");
  if (input == NULL) {
    perror("Unable to open input file");
    exit(1);
  }

  // Seek to the end of the file so we can get its size
  if (fseek(input, 0, SEEK_END) != 0) {
    perror("Unable to seek to end of file");
    exit(2);
  }

  // Get the size of the file
  size_t size = ftell(input);

  // Seek back to the beginning of the file
  if (fseek(input, 0, SEEK_SET) != 0) {
    perror("Unable to seek to beginning of file");
    exit(2);
  }

  // Allocate a buffer to hold the file contents. We know the size in bytes, so
  // there's no need to multiply to get the size we pass to malloc in this case.
  uint8_t* data = malloc(size);

  // Read the file contents
  if (fread(data, 1, size, input) != size) {
    fprintf(stderr, "Failed to read entire file\n");
    exit(2);
  }

  // Make sure the file starts with the .ar file signature
  if (memcmp(data, "!<arch>\n", 8) != 0) {
    fprintf(stderr, "Input file is not in valid .ar format\n");
    exit(1);
  }

  // Call the code to print the archive contents
  print_contents(data, size);

  // Clean up
  free(data);
  fclose(input);

  return 0;
}

/**
 * This function should print the name of each file in the archive followed by its contents.
 *
 * \param data This is a pointer to the first byte in the file.
 * \param size This is the number of bytes in the file.
 */
void print_contents(uint8_t* data, size_t size) {
  header_t header;
  int counter = 0;

  //counter to see how far in the file I am
  int counter2 = 0;

  //getting past file signature
  while (*data != 0x0A) {
    data++;
    counter2++;
  }
  data++;
  counter2++;

  while (counter2 < size) {

    //putting the file identifire into header_t
    header.fileIdentifier = data;

    while (*data != '/') {
      data++;
      counter++;
      counter2++;
    }
    *data = '\0';

    //move data to the start of File Size
    data += (16 - counter) + 32;
    counter2 += (16 - counter) + 32;

    //puting the file size into header_t
    header.fileSize = atoi((const char *)data);

    //move data to the start of File Data
    data += 12;
    counter2 += 12;

    //print out the information
    printf("%s\n", header.fileIdentifier);
    for (int i = 0; i < header.fileSize; i ++){
      printf("%c", *data);
      data++;
      counter2++;
    }
    printf("\n");
    
    //start of the next file to be read
    //counter2 += header.fileSize;

    //when file data has odd length
    if ((header.fileSize % 2) != 0){
      data++;
      counter2++;
    }

    //reset counter
    counter = 0;
  
  } 
}
