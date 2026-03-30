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

## Target Platforms & Tools

| Component | Description |
|-----------|-------------|
| **Source Language** | Custom DSL for game character behavior |
| **Target Language** | Unity C# (MonoBehaviour-based scripts) |
| **Tools Used** | Flex (Lexer), Bison (Parser), C (Semantic analysis, IR generation, optimization, code generation) |

---

## Language Overview

### Keywords

| DSL Keyword | C#/Unity Equivalent | Description |
|------------|-------------------|-------------|
| `entity` | `class` | Defines a game character |
| `state` / `mode` | `enum` / `case` | Defines behavior states |
| `when` | `if` | Conditional state transition |
| `unless` | `if not` | Transition if condition is false |
| `ifnot` | `if not` | Alternative conditional |
| `else` | `else` | Provides alternative behavior |
| `print` | `Debug.Log()` | Output for debugging |
| `return` | `return` | Function return |
| `orbit` | loop | Circular orbit movement pattern |
| `pulse` | loop | Rhythmic pulse/heartbeat pattern |
| `bounce` | loop | Bouncing/oscillating pattern |
| `repeat` | loop | Simple repeat count |
| `while` | loop | Conditional loop |
| `for` | loop | Iterator-based loop |

### Identifiers

| Feature | Rule | Example |
|---------|------|---------|
| **Character Name** | PascalCase | `Goblin`, `Boss` |
| **State Name** | PascalCase | `Idle`, `Chase`, `Attack` |
| **Enum Name** | PascalCase + State suffix | `GoblinState` |
| **Condition / Flag** | snake_case | `see_player`, `low_health` |

### Symbols and Operators

| DSL Symbol | C#/Unity Equivalent | Description |
|-----------|-------------------|-------------|
| `{ }` | `{ }` | Block start/end |
| `->` | assignment | State transition / variable assignment |
| `=>` | lambda / assignment | Alternative transition operator |
| `==`, `!=`, `>=`, `<=`, `>`, `<` | comparison | Conditional checks |
| `+`, `-`, `*`, `/`, `%` | arithmetic operators | Math operations |
| `&&`, `\|\|`, `!` | logical operators | Boolean operations |
| `=` | assignment | Variable assignment |
| `;` | `;` | Statement terminator |
| `,` | `,` | Separator |
| `:` | `:` | Declaration / mapping |
| `( )` | `( )` | Expression grouping |
| `[ ]` | `[ ]` | Array / indexing |

### Built-in Boolean Variables

| Variable | Type | Description |
|----------|------|-------------|
| `see_player` | bool | Whether character sees the player |
| `low_health` | bool | Low health flag |
| `tired` | bool | Tiredness flag |

### Control Structures

| DSL Construct | C#/Unity Equivalent | Description |
|--------------|-------------------|-------------|
| `state` / `mode` | `enum` + `switch` | Represents behavior states |
| `when condition -> state` | `if` statement | Transition to another state |
| `unless condition -> state` | `if not` | Transition if condition false |
| `repeat`, `while`, `for` | Standard loops | Iterative control |
| `print` | `Debug.Log()` | Debug output |
| `return` | `return` | Exit function |

---

## Formal Grammar

```
program → entity_list
entity_list → entity | entity_list entity
entity → ENTITY ID { state_list }
state_list → state | state_list state
state → MODE ID { statement_list }
statement_list → statement | statement_list statement
statement → declaration | assignment | transition | if_statement | loop | print_statement | return
declaration → type ID EQUALS expression SEMICOLON
transition → WHEN ID ARROW ID SEMICOLON | UNLESS ID ARROW ID SEMICOLON
if_statement → IF LPAREN condition RPAREN LBRACE statement_list RBRACE
loop → LOOP_ORBIT INT LBRACE statement_list RBRACE | ...
condition → expression | expression comp_op expression
expression → int_val | ID | expression op expression | LPAREN expression RPAREN
type → INT | FLOAT | BOOL | METER | MOOD | ENERGY | HEALTH | SPEED | POWER
```

---

## Compiler Architecture

### 1. Lexical Analysis (Flex)
- Scans the DSL source and converts it into tokens
- Recognizes keywords, identifiers, literals, operators, and symbols
- Handles single-line (`//`) and multi-line (`/* ... */`) comments
- Produces comprehensive error messages with line numbers

### 2. Syntax Analysis (Bison)
- Ensures token sequences follow the formal grammar rules
- Builds an Abstract Syntax Tree (AST) representing the program structure
- Provides detailed parse errors with line number information
- Validates syntax before semantic analysis

### 3. Abstract Syntax Tree (AST)
- Each node represents program constructs: entities, states, transitions, expressions, loops, conditions
- Supports tree traversal for semantic checks, IR generation, and code generation
- 16 different node types for complete language representation

### 4. Semantic Analysis
- Ensures logical correctness of the program
- Checks that all referenced states are properly defined
- Prevents duplicate variable or state declarations
- Performs type checking for assignments and operations
- Detects undefined variables before code generation
- Uses built-in variable pre-initialization for common AI conditions

### 5. Symbol Table Management
- Stores variable names, types (int, float, bool), and values
- Supports symbol lookup, addition, and type compatibility checking
- Tracks scope and manages built-in variables
- Maximum capacity: 100 symbols per compilation

### 6. Intermediate Representation (Three-Address Code)
- Generates TAC (Three-Address Code) for each statement
- Facilitates optimizations and simplifies final code generation
- Supports temporary variables, labels, and jump statements
- Creates `output.ir` file for debugging and analysis

### 7. Code Optimization
- **Constant Folding**: Computes expressions with literal operands at compile-time
  - Example: `3 + 4` → `7` (no runtime computation needed)
- Eliminates unnecessary operations and redundant code
- Improves runtime performance by pre-computing static values

### 8. Code Generation
- Translates AST/IR into Unity C# code
- Generates `MonoBehaviour` class for each entity
- Creates `enum` representing all behavioral states
- Implements `Update()` function with switch-case logic for state transitions
- Produces readable, well-formatted, production-ready code

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


