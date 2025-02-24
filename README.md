# Telemetry Logger


## Project Overview

The **Telemetry Logger** is a C++ library designed to record telemetry data in a structured and efficient way. This tool is inspired by the **rerun.io** framework and is intended for use in embedded systems and robotics applications.

## Usage

### Build

To build the lib execute these commands :
```bash
# Create a build directory
mkdir build && cd build

# Run CMake
cmake ..

# Compile the project
make
```

This will generate:
    build/lib/libtelemetry.a → The static telemetry library

## Use the lib

Here is an example on how to use the lib

```cpp
#include "TelemetryLogger.h"
#include <thread>

int main() {
    // Create logger with a name
    TelemetryLogger logger("session_1");

    // Declare telemetry series (name, unit, type)
    logger.declareSeries("battery_level", "V", TelemetryType::DOUBLE);
    logger.declareSeries("status", "", TelemetryType::STRING);

    // Simulate logging at different times
    // to log a value use .log(serie_name, value)
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    logger.log("battery_level", 29.5);
    logger.log("status", "OK");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    logger.log("battery_level", 28.9);
    logger.log("status", "WARNING");

    // Save to file
    logger.saveToFile("telemetry.rrd");

    // Print data
    logger.saveToStdout();

    return 0;
}
```

You can compile this with :

``g++ main.cpp  -I./include -L./build/lib -ltelemetry``

and run it : ```./a.out```

At test file (``main.cpp``) that generates random noise and a sine curve (with random times) is provided.

#### details :
While ``saveToFile`` or ``saveToStdout`` are not called, everything is stored in a buffer. if one of these function is used, the buffer content is written is the file (or stdout) and the buffer is clear. Then the data are written directly in the selected outputs (can be booth at once).


## Implementation
### Data Storage Format

Telemetry data is stored in a binary, bit-packed format for efficiency.
A file is made of three elements :
- The header (unique) which contains the log name and its starting timestamp.
- Serie declarations
- Values (multiple)

A serie is written in the file as soon as it is declared. A serie declaration contains :
- magic number to identify serie declaration (2 bytes)
- lenght of serie name (1 byte)
- serie name (variable)
- serie ID (2 bytes)
- serie start time (8 bytes) Time relative to the logger start
- serie type (1 byte)
- serie unit (16 bytes)

Each recorded value consists of:
- Series ID (2 bytes):  Unique identifier assigned at series declaration.
- Timestamp (8 bytes) Time relative to the logger start.
- Value : either 8 bytes for a double, or if a string 1 byte for the size and a variable number of byte for the content


### Serialization & File Handling

Each element (header, serie, value), has two functions :
- Readable Representation – for debugging and logging in a human-friendly format.
- Binary Representation – Converts the element into a byte array.

ex:
```cpp
    std::vector<uint8_t> getValueAsBytes(const TelemetryData &value);
```

This function returns the full binary representation of the value in a single operation.

Advantages:
- Efficient I/O – Instead of multiple ``write()`` calls (one per field), the entire object is written at once, minimizing the number f expensive disk I/O operations.
- Better Performance – Reducing system calls improves overall logging speed, making the telemetry system more efficient.


File Persistence:

The file is opened once and stay open until ``stopSaveToFile()`` is called, ensuring continuous logging without unnecessary open/close operations.


### Resilience to Corruption & Fault Tolerance

Writing each elements as soon as they are logged ensure that there is no data loss.
If the program stop suddenly, everything written before will still be in the file. It also limit the memory usage: everything is logged directly, we do not need to keep the values in memory, which is a big advantage because in the context of embedded programming there are a lot of memory limits.

It could have been interesting to put everything in a buffer and write in bigger chunck (groups of a lot of elements). In addition to limiting the number of write, it would have made it possible to compress the data. But there is two major issues :
1. Saving values in memory can rapidly take a lot of place
2. If there is a crash, all the value after the last crash are lost.


### Performance Considerations

#### Efficiency

The biggest advantage of declaring the series is that it avoids redundant data.
Indeed, as all the informations of the series are linked to its id, when we log a value we can only use its serie id (only 2 bytes) instead of the full name, type, timestamp ... which saves us a lot of space.

As said before the informations are grouped to reduce the number of call to ``write``, that takes a lot of performance and time.

#### Compression

There is no compression. As the elements are written one-by-one, (the average write size is under 30 bytes), it would be pretty hard to compress. There are two problemes :
1. small packets can't reach a high level of compression (15-20% at most)
2. Compression can cost a lot in performance and time

Therefore, because of this problem it is really hard to find a good compromise between efficiency in storage and in performance, and it would be for a relly small gain, so I choosed to not implement it.

#### Real-Time & Multithreading

There is no multithreading for the moment.
A thread that handle all the writing operation could be interesting. As the writes can take a lot of time (and be blocking in some cases). It would ensure that the main program still run if there is a probleme with the logging.

### Code Organization & Design Choices

Instead of a struct, ``TelemetrySeries`` should have been a class.
In this way, it would have handled it's own logic (display, convert to bytes ...). Making a cleaner and more independant code.

The function ``appendToVector`` is templated and can add any integral type to a vector of bytes. This mean that we can easily implement other types for the value, such as int, float, short...

## Possible Improvements

1. Testing

    Currently, there are no test.
    A unit test suite using a framework like **Google Test** could help ensure correctness, detect regressions, and validate edge cases.


2. Extending Data Type Support

    Right now, the system supports double and string telemetry types.
    The architecture could be adapted to support additional types. It would be easy for C-style types (that can easily be converted to bytes), but way harder for more complex types (vector or map). A posssible way to do that would be to use more **templates** with constraints, hence, we could use any types that respect the conditions


3. Codebase Separation & Modularity
    Right now, all the logic is in the ``telemetryLogger`` class, making it monolithic and hard to maintain. It would be great to split responsibilities into dedicated classes. For example, a ``FileWriter`` class, dedicated to file I/O operations would be a great improvement.
