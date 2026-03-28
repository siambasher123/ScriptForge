# AI Compiler - A Domain-Specific Language Compiler for Unity Game AI

AI Compiler is a tool that lets game developers design AI behaviors using a simple, human-friendly language instead of wrestling with complex Unity C# code. Just describe what your characters should do—like "when see_player -> Chase" or "orbit 3 times and heal"—and the compiler automatically generates clean, ready-to-use Unity scripts. It handles all the heavy lifting: type checking, loops, state machines, and even optimizes your code behind the scenes. Whether you're prototyping quickly or building complex enemy behaviors, this tool lets you focus on making your game AI feel alive instead of getting lost in implementation details.

---

## Project Overview

Building smart game AI can be daunting, especially if you're not deeply familiar with programming in C#. AI Compiler bridges that gap. It gives you a friendly DSL to express your ideas, then turns them into efficient Unity-compatible code behind the scenes. Whether you're prototyping, teaching, or working on production games, this tool aims to make AI development quick, safe, and enjoyable.

---

## Features

**Lexical Analysis**
- Supports complete token definitions for all constructs and robust error handling through Flex.
- Can process both single-line (`//`) and multi-line (`/* ... */`) comments.

**Syntax Analysis**
- Uses Bison for a precise grammar and line-numbered error messages.
- Handles everything from entities and modes, to variable declarations, control flows, and loops.

**Semantic Analysis**
- Checks types strictly and prevents mistakes like variable redeclaration.
- Built-in variables provided for common AI needs (e.g., `see_player`, `low_health`, `tired`).
- Automatically converts between types when needed, with helpful warnings.

**Intermediate Representation and Optimization**
- Generates a three-address code IR (in `output.ir`) for further processing and optimizations.
- Performs constant folding to compute expressions during compile-time and improve runtime performance.

**Unity C# Code Generation**
- Produces readable, organized MonoBehaviour scripts with states, transitions, and Unity lifecycle methods like `Start()` and `Update()`.

**Debugging and Messages**
- Print statements and logging built in for transparent AI debugging.

---

## Language Syntax Examples

**Variable Declarations**
```c
meter health_level = 100;
bool is_angry = 0;
int damage = 25;
float multiplier = 1.5f;
```

**Entity with Modes (States)**
```c
entity Goblin {
    mode Idle {
        when see_player -> Chase;
        orbit 3 {
            unless low_health -> Escape;
            health_level = health_level - 10;
        }
    }
    
    mode Chase {
        print("Chasing player!");
        when low_health -> Escape;
        speed_factor = speed_factor + 5;
    }
}
```

**Control Flow**
```c
if (health < 50) {
    print("Low health!");
} else {
    print("Healthy");
}
```

---

## Installation

**Requirements:**
- Linux/Unix environment (or WSL on Windows)
- Flex (lexical analyzer)
- Bison (parser generator)
- GCC (C compiler)

**Steps:**
```bash
# Clone the repository
git clone <repository-url>
cd compiler

# Build the compiler
make clean
make

# (Optional) Use the interactive terminal interface
chmod +x compiler_terminal.sh
./compiler_terminal.sh
```

---

## Usage Guide

Once installed, you can compile your DSL source files like this:
```bash
./compiler input.txt
```

This will generate:
- `output.cs`: The complete Unity C# script.
- `output.ir`: The intermediate representation of your logic.

To check the output, simply:
```bash
cat output.cs
cat output.ir
```

---

## Compilation Pipeline

1. **DSL Source Code** (`.txt` file)
2. **Lexical Analysis** (Flex): Token stream
3. **Syntax Analysis** (Bison): Abstract Syntax Tree (AST)
4. **Semantic Analysis**: Type checking and symbol tracking
5. **Optimization**: Constant folding
6. **Intermediate Code Generation**: Three-address code in `output.ir`
7. **Target Code Generation**: Unity C# in `output.cs`

---

## Advanced Features

- Automatic type conversion between numbers and booleans, with helpful warnings
- Intermediate representation for flexibility and future extensibility
- Compile-time expression evaluation to boost performance
- Interactive terminal interface for compiling and previewing files

---

## Error Handling

AI Compiler provides detailed error messages, with:
- Line numbers
- Clear explanations for lexical, syntax, and semantic issues
- Warnings about type mismatches and implicit conversions

---

## Project Structure

```
compiler/
├── lexer.l              # Flex lexer 
├── parser.y             # Bison parser 
├── ast.h / ast.c        # Abstract Syntax Tree
├── symbol_table.h / .c  # Symbol table
├── semantic.h / .c      # Semantic analyzer
├── ir.c                 # IR generation
├── main.c               # Compiler driver
├── token.h / .c         # Tokens
├── Makefile             # Build automation
├── compiler_terminal.sh # Interactive interface
├── input.txt            # Example input
└── README.md
```

---

## Example Walkthrough

**Input DSL (`input.txt`):**
```c
meter health = 100;
bool is_angry = 0;

entity Goblin {
    mode Idle {
        when see_player -> Chase;
        orbit 3 {
            health = health - 10;
            when health < 50 -> Flee;
        }
        print(health);
    }
    
    mode Chase {
        print("Chasing!");
        speed = speed + is_angry * 5;
        when low_health -> Escape;
    }
}
```

**Generated C# (`output.cs`):**
```csharp
using UnityEngine;
using System;

public class Goblin : MonoBehaviour {
    public float health = 100;
    public bool is_angry = 0;

    private string currentState = "Idle";

    void Update() {
        switch (currentState) {
            case "Idle": Idle(); break;
            case "Chase": Chase(); break;
        }
    }
    void Idle() {
        if (see_player) ChangeState("Chase");
        for (int i = 0; i < 3; i++) {
            health = health - 10;
            if (health < 50) ChangeState("Flee");
        }
        Debug.Log(health);
    }
    void Chase() {
        Debug.Log("Chasing!");
        speed = speed + is_angry * 5;
        if (low_health) ChangeState("Escape");
    }
}
```

---

## Future Enhancements

- Dead code elimination and further optimizations
- Support for user functions, arrays, and additional data structures
- More Unity-specific exports (coroutines, physics, etc.)
- Graphical user interface for greater usability





## License

This project is licensed under the MIT License. See [LICENSE](./LICENSE) for details.


