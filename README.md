# CException

An simple implementation of exception handling in the C language.

Released into the public domain, see [LICENSE](LICENSE) for more information.

## Notes

- Multithreading support depends on the build environment.
- Variables that will be modified in TRY-CATCH blocks need to qualify with `volatile`.
- Should not set the uncaught exception handler to a function can return or `NULL`.
- To disable line information in exceptions, define `CEXCEPTION_DISABLE_LINE_INFO` before including `cexception.h`.
- To disable file information in exceptions, define `CEXCEPTION_DISABLE_FILE_INFO` before including `cexception.h` (implies `CEXCEPTION_DISABLE_LINE_INFO`).

## Undefined Behaviors

- Directly jump out of a TRY-CATCH block (use `INTERRUPT_TRY`/`BREAK_TRY` to wrap the corresponding statement instead).
- A TRY-CATCH block does not terminate after executing `INTERRUPT_TRY`/`BREAK_TRY`.
- Throws an exception from a scope that contains any variable-length array, variably-modified type object or C++ automatic object with a non-trivial destructor.

## All Identifiers

### Macros

#### TRY-CATCH Block

| Identifier | Parameters         | Description                                                                  |
| ---------- | ------------------ | ---------------------------------------------------------------------------- |
| `TRY`      | -                  | Represents the beginning of a TRY-CATCH block                                |
| `CATCH`    | `(type, variable)` | Catches an exception of the specific type and its subtypes, matches only one |
| `NTHROWN`  | -                  | Executes when no exception is thrown                                         |
| `FINALLY`  | -                  | Always executes unless the TRY-CATCH block has terminated                    |
| `TRY_END`  | -                  | Represents the end of a TRY-CATCH block                                      |

#### Code Jumping

| Identifier      | Parameters                 | Description                                                                                          |
| --------------- | -------------------------- | ---------------------------------------------------------------------------------------------------- |
| `THROW_NEW`     | `(type, message, deleter)` | Creates a new exception instance and throws it (`deleter` can be `NULL`)                             |
| `THROW`         | `(exception)`              | Throws an exception                                                                                  |
| `INTERRUPT_TRY` | `(statement)`              | Jumps out of the TRY-CATCH block by the given statement after trying to execute the `FINALLY` clause |
| `BREAK_TRY`     | `(statement)`              | Jumps out of the TRY-CATCH block by the given statement immediately                                  |

#### Others

| Identifier          | Parameters                 | Description                                                       |
| ------------------- | -------------------------- | ----------------------------------------------------------------- |
| `NEW_EXCEPTION`     | `(type, message, deleter)` | Used to create a new exception instance (`deleter` can be `NULL`) |
| `DROP_EXCEPTION`    | `(exception)`              | Drops an exception instance                                       |
| `DECLARE_EXCEPTION` | `(name, parent)`           | Declares a new exception type (usually used in a header file)     |
| `DEFINE_EXCEPTION`  | `(name, parent)`           | Defines a declared exception type (usually used in a source file) |
| `NORETURN`          | -                          | Indicates that the function does not return                       |

### Exception Types

- Actually constants of the type `ExceptionType`.

| Identifier   | Parent | Description                                                        |
| ------------ | ------ | ------------------------------------------------------------------ |
| `EXCEPTION`  | -      | Top level exception type, the parent of all other exceptions types |

### Structure Types

- All structure types have a typedef with the same name.

#### ExceptionType

- The type of exception types.

| Member   | Type                         | Description               |
| -------- | ---------------------------- | ------------------------- |
| `name`   | `const char* const`          | The name of exception     |
| `parent` | `const ExceptionType* const` | The parent exception type |

#### ExceptionInstance

- The exception instance type.

| Member    | Type                   | Description                                                 |
| --------- | ---------------------- | ----------------------------------------------------------- |
| `type`    | `const ExceptionType*` | The exception type                                          |
| `message` | `const char*`          | The message of the exception                                |
| `alloc`   | `bool`                 | Whether the object referenced by the `message` is allocated |
| `file`    | `const char*`          | The file where the exception created                        |
| `line`    | `int`                  | The line where the exception created                        |

#### TryCatchContext

- The TRY-CATCH context type.

| Member      | Type                | Description                                                   |
| ----------- | ------------------- | ------------------------------------------------------------- |
| `status`    | `TryCatchStatus`    | The status in the TRY-CATCH block                             |
| `env`       | `jmp_buf`           | Stores the execution context                                  |
| `exception` | `ExceptionInstance` | The exception instance if there is an exception occurred      |
| `link`      | `TryCatchContext*`  | The previous TRY-CATCH context in the TRY-CATCH context stack |

### Enumeration Types

- All enumeration types have a typedef with the same name.

#### TryCatchStatus

- Represents the statuses in TRY-CATCH blocks.

| Constant                          | Value | Description                                                              |
| --------------------------------- | ----- | ------------------------------------------------------------------------ |
| `TryCatchStatusStart`             | `0`   | Started executing a TRY-CATCH block                                      |
| `TryCatchStatusTrying`            | `1`   | Executing the `TRY` clause                                               |
| `TryCatchStatusNoException`       | `2`   | No exception occurred in the `TRY` clause                                |
| `TryCatchStatusExceptionOccurred` | `3`   | An exception occurred in the `TRY` clause                                |
| `TryCatchStatusCaughtException`   | `4`   | The exception has been caught                                            |
| `TryCatchStatusExceptionRaised`   | `5`   | An exception occurred in the TRY-CATCH block but not in the `TRY` clause |
| `TryCatchStatusInterrupted`       | `6`   | The TRY-CATCH block is interrupted                                       |

### Typedefs

| Identifier                 | Type                              | Description                              |
| -------------------------- | --------------------------------- | ---------------------------------------- |
| `UncaughtExceptionHandler` | `void (const ExceptionInstance*)` | Represents an uncaught exception handler |

### Functions

| Identifier                           | Signature                                                               | Description                                                                     |
| ------------------------------------ | ----------------------------------------------------------------------  | ------------------------------------------------------------------------------- |
| `throw_exception`                    | `void (const ExceptionInstance* exception)`                             | Throws an exception                                                             |
| `print_exception_info`               | `void (const ExceptionInstance* exception, FILE* stream)`               | Prints the exception information to the specified stream                        |
| `exception_instance_of`              | `bool (const ExceptionInstance* exception, const ExceptionType* type)`  | Determines the exception instance is an instance of the specific exception type |
| `try_catch_context_stack_push`       | `void (TryCatchContext* context)`                                       | Adds the TRY-CATCH context to the TRY-CATCH context stack                       |
| `try_catch_context_stack_pop`        | `void (void)`                                                           | Removes the last TRY-CATCH context from the TRY-CATCH context stack             |
| `try_catch_context`                  | `TryCatchContext* (void)`                                               | Gets the current TRY-CATCH context                                              |
| `get_uncaught_exception_handler`     | `UncaughtExceptionHandler* (void)`                                      | Gets the uncaught exception handler (of the current thread)                     |
| `set_uncaught_exception_handler`     | `void (UncaughtExceptionHandler* handler)`                              | Sets the uncaught exception handler (of the current thread)                     |
| `default_uncaught_exception_handler` | `void (const ExceptionInstance*)`                                       | Prints the exception information to `stderr` and aborts the program             |

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
        THROW_NEW(EXCEPTION, "exeption message", NULL);
        number = 3;
    } CATCH (EXCEPTION, exception) {
        printf("CATCH\n");
        print_exception_info(exception, stderr);
    } NTHROWN {
        printf("NTHROWN\n");
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
Exception (file "example.c", line 11): exeption message
FINALLY
Number: 2
```
