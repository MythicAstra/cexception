/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org/>
 */

#ifndef CEXCEPTION_H
#define CEXCEPTION_H

#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #if __STDC_VERSION__ < 202311L
        #include <stdbool.h>
    #endif
#elif !defined(__cplusplus)
    #define bool unsigned char
    #define true 1
    #define false 0
#endif

#define TRY \
    { \
        TryCatchContext _try_catch_context; \
        volatile bool _try_catch_finally_executed = false; \
        _try_catch_context.status = TryCatchStatusTrying; \
        _try_catch_context.exception.deleter = NULL; \
        try_catch_context_stack_push(&_try_catch_context); \
        (void) setjmp(_try_catch_context.env); \
        if (_try_catch_context.status == TryCatchStatusTrying) { \

#define CATCH(type, variable) \
        } else if (_try_catch_context.status == TryCatchStatusExceptionOccurred0 && \
            exception_instance_of(&_try_catch_context.exception, &(type))) { \
            _try_catch_context.status = TryCatchStatusCaughtException; \
            const volatile ExceptionInstance* const (variable) = &_try_catch_context.exception;

#define NOEXCEP \
        } \
        if (_try_catch_context.status == TryCatchStatusTrying) { \
            _try_catch_context.status = TryCatchStatusNoException; \
        } \
        if (_try_catch_context.status == TryCatchStatusNoException) {

#define FINALLY \
        } \
        if (_try_catch_context.status == TryCatchStatusTrying) { \
            _try_catch_context.status = TryCatchStatusNoException; \
        } \
        if (!_try_catch_finally_executed) { \
            _try_catch_finally_executed = true;

#define TRY_END \
        } \
        if (_try_catch_context.status == TryCatchStatusInterrupted) { \
            longjmp(*get_try_catch_interrupted_env(), 1); \
        } \
        try_catch_context_stack_pop(); \
        if (_try_catch_context.status == TryCatchStatusExceptionOccurred0 || \
            _try_catch_context.status == TryCatchStatusExceptionOccurred1) { \
            THROW(&_try_catch_context.exception); \
        } \
        DROP_EXCEPTION(_try_catch_context.exception); \
    } \
    (void) 0

#define THROW_NEW(type, message, deleter) \
    { \
        const ExceptionInstance _exception = NEW_EXCEPTION((type), (message), (deleter)); \
        THROW(&_exception); \
    } \
    (void) 0

#define THROW(exception) throw_exception(exception)

#define INT_TRY(statement) \
    { \
        if (_try_catch_finally_executed) { \
            BREAK_TRY(statement); \
        } \
        if (setjmp(*get_try_catch_interrupted_env()) == 0) { \
            _try_catch_context.status = TryCatchStatusInterrupted; \
            longjmp(_try_catch_context.env, 0); \
        } \
        BREAK_TRY(statement); \
    } \
    (void) 0

#define BREAK_TRY(statement) \
    { \
        try_catch_context_stack_pop(); \
        DROP_EXCEPTION(_try_catch_context.exception); \
        statement; \
    } \
    (void) 0

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #ifdef CEXCEPTION_DISABLE_FILE_INFO
        #define NEW_EXCEPTION(type, message, deleter) \
            ((const ExceptionInstance) {&(type), (message), (deleter), NULL, 0})
    #else
        #ifdef CEXCEPTION_DISABLE_LINE_INFO
            #define NEW_EXCEPTION(type, message, deleter) \
                ((const ExceptionInstance) {&(type), (message), (deleter), __FILE__, 0})
        #else
            #define NEW_EXCEPTION(type, message, deleter) \
                ((const ExceptionInstance) {&(type), (message), (deleter), __FILE__, __LINE__})
        #endif
    #endif
#else
    #ifdef CEXCEPTION_DISABLE_FILE_INFO
        #define NEW_EXCEPTION(type, message, deleter) \
            {&(type), (message), (deleter), NULL, 0}
    #else
        #ifdef CEXCEPTION_DISABLE_LINE_INFO
            #define NEW_EXCEPTION(type, message, deleter) \
                {&(type), (message), (deleter), __FILE__, 0}
        #else
            #define NEW_EXCEPTION(type, message, deleter) \
                {&(type), (message), (deleter), __FILE__, __LINE__}
        #endif
    #endif
#endif

#define DROP_EXCEPTION(exception) \
    { \
        if ((exception).deleter != NULL) { \
            (exception).deleter((void*) (exception).message); \
        } \
    } \
    (void) 0

#define DECLARE_EXCEPTION(name, parent) \
    extern const ExceptionType (name)

#define DEFINE_EXCEPTION(name, parent) \
    const ExceptionType (name) = {#name, &(parent)}

typedef struct ExceptionType {
    const char* const name;
    const struct ExceptionType* const parent;
} ExceptionType;

typedef struct ExceptionInstance {
    const ExceptionType* type;
    const char* message;
    void (*deleter)(void*);
    const char* file;
    unsigned int line;
} ExceptionInstance;

typedef enum TryCatchStatus {
    TryCatchStatusTrying,
    TryCatchStatusNoException,
    TryCatchStatusExceptionOccurred0,
    TryCatchStatusExceptionOccurred1,
    TryCatchStatusCaughtException,
    TryCatchStatusInterrupted
} TryCatchStatus;

typedef struct TryCatchContext {
    volatile TryCatchStatus status;
    volatile ExceptionInstance exception;
    jmp_buf env;
    struct TryCatchContext* volatile link;
} TryCatchContext;

typedef void UncaughtExceptionHandler(const volatile ExceptionInstance*);

#ifdef CEXCEPTION_SHARED_LIB
    #if defined(_WIN32) || defined(__CYGWIN__)
        #ifdef CEXCEPTION_BUILDING
            #define CEXCEPTION_API __declspec(dllexport)
        #else
            #define CEXCEPTION_API __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) && __GNUC__ >= 4
        #define CEXCEPTION_API __attribute__((visibility("default")))
    #else
        #define CEXCEPTION_API
    #endif
#else
    #define CEXCEPTION_API
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L || \
    (defined(__cplusplus) && (defined(_MSVC_LANG) && _MSVC_LANG >= 201103L || __cplusplus >= 201103L))
    #define NORETURN [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define NORETURN _Noreturn
#elif defined(__GNUC__)
    #define NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define NORETURN __declspec(noreturn)
#else
    #define NORETURN
#endif

#ifdef __cplusplus
extern "C" {
#endif

CEXCEPTION_API
extern const ExceptionType EXCEPTION;

NORETURN CEXCEPTION_API
void throw_exception(const volatile ExceptionInstance* exception);

CEXCEPTION_API
void print_exception_info(const volatile ExceptionInstance* exception, FILE* stream);

CEXCEPTION_API
bool exception_instance_of(const volatile ExceptionInstance* exception, const ExceptionType* type);

CEXCEPTION_API
void try_catch_context_stack_push(TryCatchContext* context);

CEXCEPTION_API
void try_catch_context_stack_pop(void);

CEXCEPTION_API
TryCatchContext* get_try_catch_context(void);

CEXCEPTION_API
jmp_buf* get_try_catch_interrupted_env(void);

CEXCEPTION_API
UncaughtExceptionHandler* get_uncaught_exception_handler(void);

CEXCEPTION_API
void set_uncaught_exception_handler(UncaughtExceptionHandler* handler);

NORETURN CEXCEPTION_API
void default_uncaught_exception_handler(const volatile ExceptionInstance* exception);

#ifdef __cplusplus
}
#endif

#endif
