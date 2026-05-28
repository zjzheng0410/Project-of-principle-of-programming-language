#include "source.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *source_strdup(const char *text) {
    size_t len = strlen(text);
    char *copy = (char *)malloc(len + 1);
    if (!copy) {
        return NULL;
    }
    memcpy(copy, text, len + 1);
    return copy;
}

static char *copy_message(const char *prefix, const char *path) {
    const char *err = strerror(errno);
    size_t len = strlen(prefix) + strlen(path) + strlen(err) + 8;
    char *msg = (char *)malloc(len);
    if (!msg) {
        return NULL;
    }
    snprintf(msg, len, "%s '%s': %s", prefix, path, err);
    return msg;
}

char *source_read_file(const char *path, char **error_message) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        if (error_message) {
            *error_message = copy_message("无法读取输入文件", path);
        }
        return NULL;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        if (error_message) {
            *error_message = copy_message("无法定位输入文件", path);
        }
        fclose(fp);
        return NULL;
    }
    long size = ftell(fp);
    if (size < 0) {
        if (error_message) {
            *error_message = copy_message("无法获取输入文件大小", path);
        }
        fclose(fp);
        return NULL;
    }
    rewind(fp);
    char *buf = (char *)malloc((size_t)size + 1);
    if (!buf) {
        if (error_message) {
            *error_message = source_strdup("源码读取：内存不足");
        }
        fclose(fp);
        return NULL;
    }
    size_t got = fread(buf, 1, (size_t)size, fp);
    if (got != (size_t)size) {
        if (error_message) {
            *error_message = copy_message("无法完整读取输入文件", path);
        }
        free(buf);
        fclose(fp);
        return NULL;
    }
    buf[size] = '\0';
    fclose(fp);
    return buf;
}
