#include "argus/internal/stream.h"
#include <stdlib.h>
#include <string.h>

#define MAX_STREAMS 3
#define BUFFER_SIZE 4096

typedef struct {
    int fd;
    int pipe_fds[2];
    int backup_fd;
    int is_active;
} stream_info_t;

static stream_info_t streams[MAX_STREAMS] = {{0}};


static int is_valid_fd(int fd) {
    return (fd == STDOUT_FILENO || fd == STDERR_FILENO || fd == STDIN_FILENO);
}

int stream_redirect(int fd)
{
    if (!is_valid_fd(fd))
        return STREAM_ERROR_INVALID_FD;

    stream_info_t *info = &streams[fd];

    if (info->is_active)
        return STREAM_ERROR_ALREADY_ACTIVE;

    if (pipe(info->pipe_fds) != 0)
        return STREAM_ERROR_PIPE_FAILED;

    info->backup_fd = dup(fd);
    if (info->backup_fd == -1) {
        close(info->pipe_fds[0]);
        close(info->pipe_fds[1]);
        return STREAM_ERROR_DUP_FAILED;
    }
    
    if (dup2(info->pipe_fds[1], fd) == -1) {
        close(info->backup_fd);
        close(info->pipe_fds[0]);
        close(info->pipe_fds[1]);
        return STREAM_ERROR_DUP_FAILED;
    }
    
    close(info->pipe_fds[1]);
    info->pipe_fds[1] = -1;
    
    info->fd = fd;
    info->is_active = 1;
    
    return STREAM_SUCCESS;
}

char *stream_read(int fd)
{
    if (!is_valid_fd(fd))
        return NULL;

    stream_info_t *info = &streams[fd];

    if (!info->is_active)
        return NULL;

    if (fd == STDOUT_FILENO)
        fflush(stdout);
    else if (fd == STDERR_FILENO)
        fflush(stderr);

    char *result = NULL;
    size_t total_size = 0;
    size_t result_capacity = 0;
    char buffer[BUFFER_SIZE];
    
    int bytes_read;
    while ((bytes_read = read(info->pipe_fds[0], buffer, sizeof(buffer))) > 0)
	{
        if (total_size + bytes_read + 1 > result_capacity)
		{
            result_capacity = (result_capacity == 0) ? BUFFER_SIZE : result_capacity * 2;
            while (total_size + bytes_read + 1 > result_capacity)
                result_capacity *= 2;

            char *new_result = realloc(result, result_capacity);
            if (!new_result) {
                free(result);
                return NULL;
            }
            result = new_result;
        }
        
        memcpy(result + total_size, buffer, bytes_read);
        total_size += bytes_read;
    }
    
    if (total_size == 0) {
        free(result);
        return NULL;
    }
    
    result[total_size] = '\0';
    
    char *final_result = realloc(result, total_size + 1);
    if (final_result)
        result = final_result;

    return result;
}

void stream_restore(int fd)
{
    if (!is_valid_fd(fd))
        return;

    stream_info_t *info = &streams[fd];

    if (!info->is_active)
        return;

    if (fd == STDOUT_FILENO)
        fflush(stdout);
    else if (fd == STDERR_FILENO)
        fflush(stderr);

    
    char buffer[BUFFER_SIZE];
    while (read(info->pipe_fds[0], buffer, sizeof(buffer)) > 0) {
        // Just read until the pipe is empty
    }
    
    dup2(info->backup_fd, fd);
    close(info->backup_fd);
    close(info->pipe_fds[0]);
    
    info->fd = -1;
    info->pipe_fds[0] = info->pipe_fds[1] = -1;
    info->backup_fd = -1;
    info->is_active = 0;
}

int stream_is_active(int fd)
{
    if (!is_valid_fd(fd))
        return 0;

    return streams[fd].is_active;
}

void stream_cleanup_all(void)
{
    for (int i = 0; i < MAX_STREAMS; i++)
	{
        stream_info_t *info = &streams[i];
        if (info->is_active) {
            int fd = (i == 0) ? STDOUT_FILENO : STDERR_FILENO;
            stream_restore(fd);
        }
    }
}
