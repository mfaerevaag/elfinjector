#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef DEBUG
/* log debug */
#define log_debug(msg) fprintf(stdout, "[\x1B[34m~\x1B[0m] " msg "\n");
/* log formatted debug */
#define log_debugf(msg, ...) fprintf(stdout, "[\x1B[34m~\x1B[0m] " msg "\n", ##__VA_ARGS__);
#else
#define log_debug(msg)
#define log_debugf(msg, ...)
#endif // DEBUG

/* log info */
#define log_info(msg) fprintf(stdout, "[\x1B[32m*\x1B[0m] " msg "\n");
/* log formatted info */
#define log_infof(msg, ...) fprintf(stdout, "[\x1B[32m*\x1B[0m] " msg "\n", ##__VA_ARGS__);
/* log warning */
#define log_warn(msg) fprintf(stderr, "[\x1B[33m?\x1B[0m] " msg "\n");
/* log formatted warning */
#define log_warnf(msg, ...) fprintf(stderr, "[\x1B[33m?\x1B[0m] " msg "\n", ##__VA_ARGS__);
/* log error */
#define log_err(msg) fprintf(stderr, "[\x1B[31m!\x1B[0m] " msg "\n");
/* log formatted error */
#define log_errf(msg, ...) fprintf(stderr, "[\x1B[31m!\x1B[0m] " msg "\n", ##__VA_ARGS__);
/* log custom perror from errno */
#define log_perr(msg) fprintf(stderr, "[\x1B[31m!\x1B[0m] %s: %s\n", msg, strerror(errno));

#endif /* LOGGER_H */
