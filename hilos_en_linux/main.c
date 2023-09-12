#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef uint64_t u64;

struct file_scan_data {
  char* path;
  char* word;
  u64 start;
  u64 end;
  u64 count;  // return val
};

bool is_delim_char(int c) {
  int delim_chars[] = {' ', '\r', '\n', EOF, '.', ',', '!',
                       '?', '(',  ')',  '[', ']', '-', ';'};
  int chars = sizeof(delim_chars) / sizeof(int);
  for (int i = 0; i < chars; i++) {
    if (delim_chars[i] == c) {
      return true;
    }
  }
  return false;
}

void* file_scan_word_thread(void* args) {
  struct file_scan_data* data = (struct file_scan_data*)args;
  FILE* file = fopen(data->path, "r");
  fseek(file, data->start, SEEK_SET);
  char word[64];
  int cursor = 0;

  int c = fgetc(file);
  // try to read word if the current position is behind end
  while (ftell(file) < data->end) {
    cursor = 0;
    // account for multiple spaces and other special chars to separate words
    while (is_delim_char(c)) {
      c = fgetc(file);
      if (c == EOF) {
        break;
      }
    }
    do {
      word[cursor] = (char)c;
      c = fgetc(file);
      cursor++;
    } while (!is_delim_char(c));
    word[cursor] = 0;  // term char
    if (strcmp(data->word, word) == 0) {
      data->count++;
    }
  }
}

int main(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Uso: count [ruta de archivo] [palabra] [threads]\r\n");
    return 1;
  }
  char* file_path = argv[1];
  char* word = argv[2];
  int threads = atoi(argv[3]);
  struct stat st;
  stat(file_path, &st);
  u64 block_size = st.st_size / threads;
  u64 current_addr = 0;

  struct file_scan_data* fsds =
      (struct file_scan_data*)calloc(threads, sizeof(struct file_scan_data));
  pthread_t* thread_ids = (pthread_t*)calloc(threads, sizeof(pthread_t));
  for (int i = 0; i < threads; i++) {
    fsds[i].path = file_path;
    fsds[i].word = word;
    fsds[i].start = current_addr;
    current_addr += block_size;
    fsds[i].end = current_addr;
    pthread_create(&thread_ids[i], NULL, file_scan_word_thread,
                   (void*)&fsds[i]);
  }
  u64 total_count = 0;
  for (int i = 0; i < threads; i++) {
    pthread_join(thread_ids[i], NULL);
    total_count += fsds[i].count;
  }
  printf("Count: %" PRIu64 " \r\n", total_count);
  free((void*)fsds);
  free((void*)thread_ids);
  return 0;
}
