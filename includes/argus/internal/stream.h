/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ARGUS_INTERNAL_STREAM_H
#define ARGUS_INTERNAL_STREAM_H

#include <stdio.h>

#ifdef _WIN32
    #include <fcntl.h>
    #include <io.h>
    #ifndef O_BINARY
        #define O_BINARY 0x00008000
    #endif
    #define pipe(fds)           _pipe(fds, 4096, O_BINARY)
    #define dup(fd)             _dup(fd)
    #define dup2(fd1, fd2)      _dup2(fd1, fd2)
    #define close(fd)           _close(fd)
    #define read(fd, buf, size) _read(fd, buf, size)
    // Define standard file descriptors for Windows
    #define STDIN_FILENO  0
    #define STDOUT_FILENO 1
    #define STDERR_FILENO 2
#else
    #include <unistd.h>
#endif

typedef enum
{
    STREAM_SUCCESS              = 0,
    STREAM_ERROR_INVALID_FD     = -1,
    STREAM_ERROR_ALREADY_ACTIVE = -2,
    STREAM_ERROR_PIPE_FAILED    = -3,
    STREAM_ERROR_DUP_FAILED     = -4,
    STREAM_ERROR_MEMORY         = -5,
    STREAM_ERROR_NOT_ACTIVE     = -6
} stream_result_t;

int   stream_redirect(int fd);
char *stream_read(int fd);
void  stream_restore(int fd);
int   stream_is_active(int fd);
void  stream_cleanup_all(void);

#define redirect_stdout() stream_redirect(STDOUT_FILENO)
#define redirect_stderr() stream_redirect(STDERR_FILENO)
#define read_stdout()     stream_read(STDOUT_FILENO)
#define read_stderr()     stream_read(STDERR_FILENO)
#define restore_stdout()  stream_restore(STDOUT_FILENO)
#define restore_stderr()  stream_restore(STDERR_FILENO)

#endif  // ARGUS_INTERNAL_STREAM_H
