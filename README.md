# CException

A simple implementation of exception handling in the C language.

Released into the public domain, see [LICENSE](LICENSE) for more information.

## Comments

- Multithreading support depends on the build environment.
- Objects of automatic storage duration that will be modified in TRY-CATCH blocks must be `volatile`-qualifed.
- Shall not set the uncaught exception handler to a function that can return or to a null pointer.
- Unless an exception instance is not thrown, you do not need to drop it by using `DROP_EXCEPTION`.
- To disable line information in exceptions, define `CEXCEPTION_DISABLE_LINE_INFO` before including `cexception.h`.
- To disable file information in exceptions, define `CEXCEPTION_DISABLE_FILE_INFO` before including `cexception.h` (implies `CEXCEPTION_DISABLE_LINE_INFO`).

### Undefined Behaviors

- Unless throwing an exception, jumping to outside of a TRY-CATCH block without using `INT_TRY`/`BREAK_TRY`.
- A TRY-CATCH block does not terminate after executing `INT_TRY`/`BREAK_TRY`.
- Throwing an exception that leaves the scope that contains any identifier with variably modified type or C++ object of automatic storage duration with a non-trivial destructor.

## All Identifiers

### Macros

#### TRY-CATCH Block

| Identifier | Parameters         | Description                                                                  |
|------------|--------------------|------------------------------------------------------------------------------|
| `TRY`      | -                  | Represents the beginning of a TRY-CATCH block                                |
| `CATCH`    | `(type, variable)` | Catches an exception of the specific type and its subtypes, matches only one |
| `NOEXCEP`  | -                  | Executes when no exception is thrown                                         |
| `FINALLY`  | -                  | Always executes unless `BREAK_TRY` was executed                              |
| `TRY_END`  | -                  | Represents the end of a TRY-CATCH block                                      |

#### Jumping

| Identifier  | Parameters                 | Description                                                                                       |
|-------------|----------------------------|---------------------------------------------------------------------------------------------------|
| `THROW_NEW` | `(type, message, deleter)` | Creates a new exception instance and throws it                                                    |
| `THROW`     | `(exception)`              | Throws an exception                                                                               |
| `INT_TRY`   | `(statement)`              | Leaves the TRY-CATCH block by the given statement(s) after trying to execute the `FINALLY` clause |
| `BREAK_TRY` | `(statement)`              | Leaves the TRY-CATCH block by the given statement(s) immediately                                  |

#### Others

| Identifier          | Parameters                 | Description                                                             |
|---------------------|----------------------------|-------------------------------------------------------------------------|
| `NEW_EXCEPTION`     | `(type, message, deleter)` | Used to create a new exception instance                                 |
| `DROP_EXCEPTION`    | `(exception)`              | Drops an exception instance                                             |
| `DECLARE_EXCEPTION` | `(name, parent)`           | Declares a new exception type (usually used in a header file)           |
| `DEFINE_EXCEPTION`  | `(name, parent)`           | Defines a declared exception type (usually used in a source file)       |
| `NORETURN`          | -                          | Indicates that the function does not return                             |

### Exception Types

- Actually constants of the type `ExceptionType`.

| Identifier  | Parent | Description                                                       |
|-------------|--------|-------------------------------------------------------------------|
| `EXCEPTION` | -      | Top level exception type, the parent of all other exception types |

### Structure Types

- All structure types have a typedef with the same name.

#### ExceptionType

- The type of exception types.

| Member   | Type                         | Description               |
|----------|------------------------------|---------------------------|
| `name`   | `const char* const`          | The name of exception     |
| `parent` | `const ExceptionType* const` | The parent exception type |

#### ExceptionInstance

- The exception instance type.

| Member    | Type                   | Description                          |
|-----------|------------------------|--------------------------------------|
| `type`    | `const ExceptionType*` | The exception type                   |
| `message` | `const char*`          | The message of the exception         |
| `deleter` | `void (*)(void*)`      | The deleter for `message` (nullable) |
| `file`    | `const char*`          | The file where the exception created |
| `line`    | `int`                  | The line where the exception created |

#### TryCatchContext

- The TRY-CATCH context type.

| Member      | Type                         | Description                                                   |
|-------------|------------------------------|---------------------------------------------------------------|
| `status`    | `volatile TryCatchStatus`    | The status in the TRY-CATCH block                             |
| `exception` | `volatile ExceptionInstance` | The exception instance if there is an exception occurred      |
| `env`       | `jmp_buf`                    | The execution context                                         |
| `link`      | `TryCatchContext* volatile`  | The previous TRY-CATCH context in the TRY-CATCH context stack |

### Enumeration Types

- All enumeration types have a typedef with the same name.

#### TryCatchStatus

- Represents the statuses in TRY-CATCH blocks.

| Constant                           | Value | Description                                                              |
|------------------------------------|-------|--------------------------------------------------------------------------|
| `TryCatchStatusTrying`             | `0`   | Executing the `TRY` clause                                               |
| `TryCatchStatusNoException`        | `1`   | No exception occurred in the `TRY` clause                                |
| `TryCatchStatusExceptionOccurred0` | `2`   | An exception occurred in the `TRY` clause                                |
| `TryCatchStatusExceptionOccurred1` | `3`   | An exception occurred in the TRY-CATCH block but not in the `TRY` clause |
| `TryCatchStatusCaughtException`    | `4`   | The exception has been caught                                            |
| `TryCatchStatusInterrupted`        | `5`   | The TRY-CATCH block is interrupted                                       |

### Typedefs

| Identifier                 | Type                                       | Description                              |
|----------------------------|--------------------------------------------|------------------------------------------|
| `UncaughtExceptionHandler` | `void (const volatile ExceptionInstance*)` | Represents an uncaught exception handler |

### Functions

| Identifier                           | Signature                                                                       | Description                                                                     |
|--------------------------------------|---------------------------------------------------------------------------------|---------------------------------------------------------------------------------|
| `throw_exception`                    | `void (const volatile ExceptionInstance* exception)` `NORETURN`                 | The implementation of throwing an exception                                     |
| `print_exception_info`               | `void (const volatile ExceptionInstance* exception, FILE* stream)`              | Prints the exception information to the specified stream                        |
| `exception_instance_of`              | `bool (const volatile ExceptionInstance* exception, const ExceptionType* type)` | Determines the exception instance is an instance of the specific exception type |
| `try_catch_context_stack_push`       | `void (TryCatchContext* context)`                                               | Adds the TRY-CATCH context to the TRY-CATCH context stack                       |
| `try_catch_context_stack_pop`        | `void (void)`                                                                   | Removes the last TRY-CATCH context from the TRY-CATCH context stack             |
| `get_try_catch_context`              | `TryCatchContext* (void)`                                                       | Gets the current TRY-CATCH context                                              |
| `get_try_catch_interrupted_env`      | `jmp_buf* (void)`                                                               | Gets the execution context where the TRY-CATCH block is interrupted             |
| `get_uncaught_exception_handler`     | `UncaughtExceptionHandler* (void)`                                              | Gets the uncaught exception handler (of the current thread)                     |
| `set_uncaught_exception_handler`     | `void (UncaughtExceptionHandler* handler)`                                      | Sets the uncaught exception handler (of the current thread)                     |
| `default_uncaught_exception_handler` | `void (const volatile ExceptionInstance*)` `NORETURN`                           | Prints the exception information to `stderr` and aborts the program             |

## Example

Code:

```c
#include <stdio.h>
#include <cexception.h>

int main(void) {
    volatile int number = 1;
    printf("Number: %d\n", number);

    TRY {
        printf("TRY\n");
        number = 2;
        THROW_NEW(EXCEPTION, "the exception message", NULL);
        number = 3;
    } CATCH (EXCEPTION, exception) {
        printf("CATCH\n");
        print_exception_info(exception, stderr);
    } NOEXCEP {
        printf("NOEXCEP\n");
    } FINALLY {
        printf("FINALLY\n");
    } TRY_END;

    printf("Number: %d\n", number);

    return 0;
}
```

Output:

```
Number: 1
TRY
CATCH
EXCEPTION (file 'example.c', line 11): the exception message
FINALLY
Number: 2
```
