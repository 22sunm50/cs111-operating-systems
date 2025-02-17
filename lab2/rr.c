#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;            // process ID
  u32 arrival_time;   // when the process arrives
  u32 burst_time;     // total execution time required

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */

  u32 response_time;
  u32 waiting_time;
  u32 time_started;
  u32 time_left;
  
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process); // doubly linked list of process

// reads next int from a character buffer
// skips non-digit characters
// extracts and constructs an integer value from the file data
u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

// converts a C-string number --> an integer
// stops if it finds a non-numeric character
u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

// opens the input file (processes.txt)
// stores for each process: .pid .arrival_time .burst_time
void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;   // pointer to the beginning of the memory-mapped file
  u32 size;               // size of input file (in bytes)
  init_processes(argv[1], &data, &size);  // read processes from input file

  u32 quantum_length = next_int_from_c_str(argv[2]); // quantum time 

  struct process_list list;
  TAILQ_INIT(&list);          // initialize process queue

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  for (u32 i = 0; i < size; i++)// loop thru all processes --> set up all processes
  {
    data[i].time_started = -1;               // none have started
    data[i].time_left = data[i].burst_time;  // reminaing time = total exec time required
  }

  struct process* stored_process;  // stores a process that exceeded its quantum
  u32 curr_time_step = 0;          // current time step
  u32 time_slice_elapsed = 0;      // how long curr process has been running
  u32 finished_count = 0;          // # of completed processes
  u32 needs_requeue = 0;           // if it needs to be reinserted
  
  while (finished_count < size){  // loop until all processes are completed

    for (u32 i = 0; i < size; i++){ // loop thru processes --> prioritize appending newly arrived processes
      if (data[i].arrival_time == curr_time_step){
        struct process *arrived_process = &data[i];
        TAILQ_INSERT_TAIL(&list, arrived_process, pointers);
      }
    }

    if (needs_requeue == 1){ // if needs to be reinserted into the queue --> reinsert (after arrived ^)
      needs_requeue = 0;
      TAILQ_INSERT_TAIL(&list, stored_process, pointers);
    }

    struct process* curr_process;
    if (!TAILQ_EMPTY(&list)){ // if queue not empty
      curr_process = TAILQ_FIRST(&list); // first on queue

      time_slice_elapsed += 1;
      curr_process->time_left -= 1;

      if (curr_process->time_started == -1){ // has not started yet --> start it
        curr_process->response_time = curr_time_step - curr_process->arrival_time; // curr_time_step - arrival
        curr_process->time_started = curr_time_step;
      }

      if (curr_process->time_left == 0){ // finished
        curr_process->waiting_time = curr_time_step + 1 - curr_process->arrival_time - curr_process->burst_time; // curr_time_step + 1 - arrival - burst
        time_slice_elapsed = 0;
        finished_count += 1;
        TAILQ_REMOVE(&list, curr_process, pointers);
        // printf("🍅 Finished: current time step = %d\n", curr_time_step);
        // printf("🍅 -----: pid = %d\n", curr_process->pid);
      }
      
      if (curr_process->time_left != 0 && time_slice_elapsed == quantum_length){ // pre-empt
	      stored_process = curr_process;
        time_slice_elapsed = 0;
        needs_requeue = 1;
        TAILQ_REMOVE(&list, curr_process, pointers);
        // printf("🍅 Pre-empt: current time step = %d\n", curr_time_step);
        // printf("🍅 -----: pid = %d\n", curr_process->pid);
      }
    }
    curr_time_step += 1;
  }
  //Iterate through to add up waiting, response time
  for (u32 i = 0; i < size; i++)
  {
    total_waiting_time += data[i].waiting_time;
    total_response_time += data[i].response_time;
  }
  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}