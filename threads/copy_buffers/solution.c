/* Given the following code:
 *
 * #define BUF_SIZE 1024
 * #define BUFFERS 100
 *
 * void *bufs[BUFFERS]; // Each buffer is BUF_SIZE long
 *
 * void copy_buf(int dest_idx, int src_idx) {
 *     // Implement this
 * }
 *
 * Implement `copy_buf()`, a method that copies one buffer in `bufs` to another buffer.
 * Make it work safely in a multithreaded program.
 * Each buffer is BUF_SIZE long.
 *
 * Source: past interview experience
 */
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#define BUF_SIZE 1024
#define BUFFERS 100
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

void *bufs[BUFFERS]; // Each buffer is BUF_SIZE long

void copy_buf(int dest_idx, int src_idx) {
	// Note: this makes use of a GCC extension
	static pthread_mutex_t buf_locks[BUFFERS] = {[0 ... BUFFERS-1] = PTHREAD_MUTEX_INITIALIZER};

	if (dest_idx == src_idx) {
		return;
	}

	int first = min(dest_idx, src_idx);
	int second = max(dest_idx, src_idx);

	pthread_mutex_lock(&buf_locks[first]);
	pthread_mutex_lock(&buf_locks[second]);

	memcpy(bufs[dest_idx], bufs[src_idx], BUF_SIZE);

	pthread_mutex_unlock(&buf_locks[second]);
	pthread_mutex_unlock(&buf_locks[first]);
}

static char buff_sample1[BUF_SIZE] = "Just a test";
static char buff_sample2[BUF_SIZE] = "Hello, world!";
static char buff_sample3[BUF_SIZE] = "Goodbye, world!";
static char buff_temp[BUF_SIZE];

int main(void) {

	bufs[0] = buff_sample1;
	bufs[1] = buff_sample2;
	bufs[2] = buff_sample3;
	bufs[3] = buff_temp;

	copy_buf(3, 0);
	copy_buf(0, 1);
	copy_buf(1, 2);
	copy_buf(2, 3);

	size_t i;
	for (i = 0; i < 3; i++) {
		printf("bufs[%zu] = %s\n", i, (char *) bufs[i]);
	}

	return 0;
}
