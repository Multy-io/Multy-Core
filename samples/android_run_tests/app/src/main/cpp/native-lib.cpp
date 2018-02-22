/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include <jni.h>
#include <android/log.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "multy_test/run_tests.h"
#include "multy_core/common.h"
#include "multy_core/src/utility.h"
#include "multy_core/error.h"


#ifdef __cplusplus
extern "C" {
#endif

static int pfd[2];
static pthread_t thr;
static const char *tag = "io.multy.android_run_tests";

static void *thread_func(void *);


int start_logger(const char *app_name) {
    tag = app_name;

    /* make stdout line-buffered and stderr unbuffered */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    /* create the pipe and redirect stdout and stderr */
    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);

    /* spawn the logging thread */
    if (pthread_create(&thr, 0, thread_func, 0) == -1)
        return -1;
    pthread_detach(thr);
    return 0;
}

static void *thread_func(void *) {
    ssize_t rdsz;
    char buf[128];
    while ((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
        if (buf[rdsz - 1] == '\n') --rdsz;
        buf[rdsz] = 0;  /* add null-terminator */
        __android_log_write(ANDROID_LOG_DEBUG, tag, buf);
    }
    return 0;
}

void throw_java_exception_str(JNIEnv *env, const char *message) {
    jclass c = env->FindClass("io/multy/android_run_tests/JniException");
    env->ThrowNew(c, message);
}

void handle_multy_error(JNIEnv *env, const Error &error) {
    __android_log_print(ANDROID_LOG_INFO, "Multy-core error",
            "Error: %s \n In file ""%s, \n in line: %d ",
            error.message, error.location.file , error.location.line);

    throw_java_exception_str(env, error.message);
}

#define JNI_ERROR(statement)                                                                       \
    do {                                                                                           \
        ErrorPtr error(statement);                                                                 \
        if (error)                                                                                 \
        {                                                                                          \
            handle_multy_error(env, *error);                                                       \
            return 0;                                                                              \
        }                                                                                          \
    } while(false)

JNIEXPORT jint JNICALL
Java_io_multy_android_1run_1tests_MainActivity_runTest(JNIEnv *env, jobject instance) {

    start_logger("io.multy.android_run_tests");

    char *foo = (char *) "foo";
    return run_tests(1, &foo);
}

JNIEXPORT jstring JNICALL
Java_io_multy_android_1run_1tests_MainActivity_version(JNIEnv *env, jobject instance) {

    using namespace multy_core::internal;
    ConstCharPtr version;
    JNI_ERROR(make_version_string(reset_sp(version)));

    return env->NewStringUTF(version.get());
}

#ifdef __cplusplus
}
#endif
