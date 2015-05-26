#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define OPEN_MAX 20
#define DEF_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#define BUFF_SIZE 1024
#define EOF (-1)

typedef struct stdio_buff {
	int buff_cnt;
	char *next;
	char *buff;
	int flags;
	int fd;
	int last_op;
} FILE;

enum flags {
	FL_READ = 1,
	FL_WRITE = 2,
	FL_EOF = 4,
	FL_ERR = 8,
	FL_UNBUF = 16
};

static struct stdio_buff iob[OPEN_MAX] = {
	{ 0, NULL, NULL, FL_READ, STDIN_FILENO },
	{ 0, NULL, NULL, FL_WRITE, STDOUT_FILENO },
	{ 0, NULL, NULL, FL_WRITE | FL_UNBUF, STDERR_FILENO }
};

#define stdin (&iob[0])
#define stdout (&iob[1])
#define stderr (&iob[2])

#define feof(f) ((f)->flags & FL_EOF)
#define ferror(f) ((f)->flags & FL_ERR)
#define fileno(f) ((f)->fd)

#define getchar() getc(stdin)
#define putchar(c) putc((c), stdout)

int fillbuff(FILE *f);
int flushbuf(int c, FILE *f);
int fflush(FILE *f);

int getc(FILE *f) {
	f->last_op = FL_READ;

	if (f->buff_cnt > 0) {
		f->buff_cnt--;
		return (unsigned char) *f->next++;
	} else {
		return fillbuff(f);
	}
}

int putc(int c, FILE *f) {
	f->last_op = FL_WRITE;

	if (f->buff_cnt > 0) {
		f->buff_cnt--;
		return *f->next++ = c;
	} else {
		return flushbuf(c, f);
	}
}

FILE *fopen(const char *name, const char *mode) {
	int oflags;
	int iolib_flags;

	if (!strcmp(mode, "r") || !strcmp(mode, "rb")) {
		oflags = O_RDONLY;
		iolib_flags = FL_READ;
	} else if (!strcmp(mode, "r+") || !strcmp(mode, "r+b") || !strcmp(mode, "rb+")) {
		oflags = O_RDWR;
		iolib_flags = FL_READ;
	} else if (!strcmp(mode, "w") || !strcmp(mode, "wb")) {
		oflags = O_WRONLY | O_CREAT | O_TRUNC;
		iolib_flags = FL_WRITE;
	} else if (!strcmp(mode, "w+") || !strcmp(mode, "w+b") || !strcmp(mode, "wb+")) {
		oflags = O_RDWR | O_CREAT | O_TRUNC;
		iolib_flags = FL_WRITE;
	} else if (!strcmp(mode, "a") || !strcmp(mode, "ab")) {
		oflags = O_WRONLY | O_CREAT | O_APPEND;
		iolib_flags = FL_WRITE;
	} else if (!strcmp(mode, "a+") || !strcmp(mode, "a+b") || !strcmp(mode, "ab+")) {
		oflags = O_RDWR | O_CREAT | O_APPEND;
		iolib_flags = FL_WRITE;
	} else {
		return NULL;
	}

	int slot;
	for (slot = 0; slot < OPEN_MAX && iob[slot].flags != 0; slot++)
		; /* Intentionally left blank */

	if (slot == OPEN_MAX) {
		return NULL;
	}

	int fd;
	fd = open(name, oflags, DEF_PERMS);
	if (fd < 0) {
		return NULL;
	}

	iob[slot].buff_cnt = 0;
	iob[slot].next = iob[slot].buff = NULL;
	iob[slot].flags = iolib_flags;
	iob[slot].fd = fd;

	return &iob[slot];
}

int fclose(FILE *f) {

	if (f == NULL) {
		return EOF;
	}

	int flush_res = fflush(f);
	int close_res = close(f->fd);

	free(f->buff);

	return flush_res == 0 && close_res == 0 ? 0 : EOF;
}

int fflush(FILE *f) {
	if ((f->flags & FL_WRITE) && !(f->flags & FL_UNBUF) &&
	    f->buff != NULL && f->next > f->buff && f->last_op == FL_WRITE) {

		int to_write = f->next-f->buff;
		int write_res = write(f->fd, f->buff, to_write);

		f->next = f->buff;
		f->buff_cnt = BUFF_SIZE;

		return write_res == 0 ? 0 : EOF;

	}
	return EOF;
}

int fillbuff(FILE *f) {
	int buffsize = BUFF_SIZE;

	if ((f->flags & (FL_READ | FL_EOF | FL_ERR)) != FL_READ) {
		return EOF;
	}

	if (f->flags & FL_UNBUF) {
		buffsize = 1;
	}

	if (f->buff == NULL) {
		f->buff = malloc(buffsize);
		if (f->buff == NULL) {
			return EOF;
		}
	}

	f->next = f->buff;
	f->buff_cnt = read(f->fd, f->buff, buffsize);

	if (f->buff_cnt == 0) {
		f->flags |= FL_EOF;
		return EOF;
	}

	if (f->buff_cnt < 0) {
		f->flags |= FL_ERR;
		return EOF;
	}

	f->buff_cnt--;
	return (unsigned char) (*f->next++);
}

int flushbuf(int c, FILE *f) {

	if ((f->flags & (FL_WRITE | FL_EOF | FL_ERR)) != FL_WRITE) {
		return EOF;
	}

	if (f->flags & FL_UNBUF) {
		char to_write = c;
		if (write(f->fd, &to_write, 1) != 1) {
			f->flags |= FL_ERR;
			return EOF;
		}
		return c;
	}

	if (f->buff == NULL) {
		f->buff = malloc(BUFF_SIZE);
		f->buff_cnt = BUFF_SIZE;
		f->next = f->buff;
		if (f->buff == NULL) {
			return EOF;
		}
	}

	if (f->next > f->buff) {
		if (write(f->fd, f->buff, f->next-f->buff) != f->next-f->buff) {
			f->flags |= FL_ERR;
			return EOF;
		}
		f->next = f->buff;
		f->buff_cnt = BUFF_SIZE;
	}

	f->buff_cnt--;
	return (unsigned char) (*f->next++ = c);
}

int main(void) {

	/* Copy stdin to stdout */
	int c;

	while ((c = getchar()) != EOF) {
		putchar(c);
	}

	fflush(stdout);

	/* Test fopen() - let's copy this program */
	FILE *src, *dst;
	src = fopen("stdio.c", "r");
	if (src == NULL) {
		return 0;
	}

	dst = fopen("stdio_copy.c", "w");
	if (dst == NULL) {
		return 0;
	}

	while ((c = getc(src)) != EOF) {
		putc(c, dst);
	}

	fclose(src);
	fclose(dst);

	return 0;
}
