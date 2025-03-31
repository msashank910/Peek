Peek - A Lightweight Linux Debugger (WIP)

Peek is a lightweight Linux debugger for 64-bit systems, built from scratch in C++. This project is currently a work in progress and aims to provide essential debugging features with a minimalistic approach. It uses Linenoise for command line editing and Libelfin for parsing ELF and DWARF debugging information.

Features (Implemented)

- Attaching to processes
- Setting breakpoints
- Inspecting registers
- Memory manipulation
- Single-Step

Technologies Used:

    Language: C++
    Platform: Linux-x86-64 (WSL2 compatible)
    Build System: g++ / CMake

📦 Installation

Clone the repository:

    git clone https://github.com/msashank910/Peek.git
    cd Peek

Build the project:

    mkdir -p build && cmake -B build . && cmake --build build

🖥 Usage

To run the debugger:
    
    ./build/pld ./<file_path_to_debuggee_program>

Note: Debugee Program must be compiled with the following flags

    -g -gdwarf04

While in the [__p|d__] command-line interface, you can interact with the child process via breakpoints, memory manipulation, and register manipulation.
For a full list of commands, use the help command:

    help

Certain commands can be executed via their prefixes as well
For example for continue: cont, con, c all work.

📋 Current Limitations

    No GUI (command-line only).
    Limited command support (in progress).
    Binaries must be compiled in C or C++ on x86-64 Linux

🛤 Roadmap

Source-Level Breakpoints

    Enable breakpoints to be set directly at specific lines of code rather than just memory addresses.

Stack Unwinding

    Implement stack trace generation to unwind the stack and show function call history.

Handling Variables

    Inspect and modify local variables and function parameters during execution.

📄 License

This project is licensed under the MIT License.

Inspiration drawn from Sy Brand’s mini-debugger tutorial series (2017).  
No code was reused; this implementation was built from scratch.
