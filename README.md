# JOCKY Forensic Compiler

<p align="center">
  <img src="https://img.shields.io/badge/JOCKY-Forensic%20Compiler-111827?style=for-the-badge" alt="JOCKY Forensic Compiler"/>
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17"/>
  <img src="https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake"/>
  <img src="https://img.shields.io/badge/IR-JIR%20%2B%20LLVM-7A1FA2?style=for-the-badge" alt="JIR + LLVM"/>
  <img src="https://img.shields.io/badge/Domain-Digital%20Forensics-1F6FEB?style=for-the-badge" alt="Digital Forensics"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Windows-PE%20Direction-0078D4?style=flat-square&logo=windows&logoColor=white" alt="Windows"/>
  <img src="https://img.shields.io/badge/Ubuntu-ELF%20Direction-E95420?style=flat-square&logo=ubuntu&logoColor=white" alt="Ubuntu"/>
  <img src="https://img.shields.io/badge/Status-SIH%202026%20Prototype-2EA44F?style=flat-square" alt="SIH 2026 Prototype"/>
</p>

<p align="center">
  <strong>A compiler-first foundation for the JOCKY forensic framework</strong><br/>
  <sub>Source Language → Frontend → JIR → LLVM IR → Runtime</sub>
</p>

---

## 📖 Overview

**JOCKY Forensic Compiler** is the compiler subsystem of the wider JOCKY forensic framework.

JOCKY is being developed for **Smart India Hackathon 2026 Problem Statement 26148**, which asks for a new programming language and framework for computer and network forensic analysis in environments where conventional forensic tooling may be heavily monitored by endpoint security controls.

The central design decision is to treat **JOCKY as a programming language and compiler platform**, not as a normal forensic application.

Instead of building a collection of unrelated scripts, JOCKY expresses forensic operations as language-level constructs and processes them through a dedicated compilation pipeline.

```text
                    JOCKY SOURCE
                         │
                         ▼
                ┌─────────────────┐
                │      LEXER      │
                │   Tokenization  │
                └────────┬────────┘
                         ▼
                ┌─────────────────┐
                │     PARSER      │
                │   + AST Build   │
                └────────┬────────┘
                         ▼
                ┌─────────────────┐
                │    SEMANTIC     │
                │     ANALYSIS    │
                └────────┬────────┘
                         ▼
                ┌─────────────────┐
                │       JIR       │
                │  JOCKY IR Layer │
                └────────┬────────┘
                         ▼
                ┌─────────────────┐
                │   LLVM CODEGEN  │
                │   LLVM IR       │
                └────────┬────────┘
                         ▼
                ┌─────────────────┐
                │  RUNTIME LAYER  │
                │ Forensic APIs   │
                └─────────────────┘
```

The repository currently demonstrates this compiler flow end to end, including lexical analysis, parsing, AST generation, semantic validation, JIR generation, LLVM IR generation, and native forensic runtime execution.

---

## 🎯 SIH 2026 Problem Statement

**Problem Statement:** 26148  
**Title:** Creation of scripts/functions with new programming language to commence Computer & Network forensic analysis without triggering security solutions  
**Organization:** National Technical Research Organisation (NTRO)

The problem statement describes a next-generation forensic framework centered on a proprietary language named **JOCKY**.

The requested direction combines several areas:

- A new programming language or custom language-independent intermediate representation
- A compiler pipeline suitable for Windows and Ubuntu targets
- Compiler-driven code generation and transformation
- Polymorphic script/function generation
- Custom encryption and transformation concepts
- Fileless and native execution research
- Kernel-level and BYOVD-related research
- Centralized execution and management across multiple systems
- Forensic analysis and evidence collection

The difficult part is not simply collecting data from a computer.

The difficult part is creating a **language and compiler architecture that can express forensic operations, transform them through an intermediate representation, and target different execution environments**.

That is the role of this repository.

---

## 💡 Why a Compiler?

Traditional forensic automation often starts from an existing scripting environment:

```text
Script
  ↓
Interpreter / Shell
  ↓
Operating-system commands
  ↓
Forensic output
```

That model provides limited control over the representation of the final analysis program.

JOCKY introduces a dedicated compiler boundary:

```text
JOCKY Program
      ↓
Language Frontend
      ↓
AST
      ↓
Semantic Validation
      ↓
JOCKY Intermediate Representation
      ↓
Compiler Transformation Stages
      ↓
LLVM-Oriented Backend
      ↓
Platform Runtime
      ↓
Forensic Analysis
```

This architecture is important because compiler stages provide explicit places to introduce:

- validation
- normalization
- transformation
- target-specific lowering
- optimization
- instrumentation
- future polymorphic transformation research

The result is a **programmable forensic execution model**, rather than a collection of hard-coded scripts.

---

## 🏗️ System Architecture

The wider JOCKY architecture is documented in the repository architecture diagram:

<p align="center">
  <img src="./arch.png" alt="JOCKY Framework Architecture" width="100%"/>
</p>

### Compiler-focused view

```text
                 ┌──────────────────────┐
                 │    JOCKY Source      │
                 │       (.jky)         │
                 └──────────┬───────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │       Frontend       │
                 │                      │
                 │ Lexer                │
                 │ Parser               │
                 │ AST                  │
                 │ Semantic Analysis    │
                 └──────────┬───────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │         JIR          │
                 │ JOCKY Intermediate   │
                 │ Representation       │
                 └──────────┬───────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │      LLVM Layer      │
                 │                      │
                 │ LLVM Context        │
                 │ Module Construction │
                 │ Runtime Bindings    │
                 │ LLVM IR Generation  │
                 └──────────┬───────────┘
                            │
                 ┌──────────┴──────────┐
                 ▼                     ▼
          ┌──────────────┐      ┌──────────────┐
          │   Windows    │      │    Ubuntu    │
          │ PE Direction │      │ ELF Direction│
          └──────┬───────┘      └──────┬───────┘
                 │                     │
                 └──────────┬──────────┘
                            ▼
                 ┌──────────────────────┐
                 │   JOCKY Runtime      │
                 │ Forensic Operations  │
                 └──────────────────────┘
```

---

## 🔬 Compiler Pipeline

### 1. Lexer

The lexer converts JOCKY source text into a stream of typed tokens.

Current token categories include:

| Token | Purpose |
|---|---|
| `Identifier` | Names such as modules, functions, and calls |
| `String` | String literals |
| `module` | Module declaration keyword |
| `fn` | Function declaration keyword |
| `(` `)` | Call syntax |
| `{` `}` | Function body delimiters |
| `;` | Statement terminator |
| `EOF` | End of source |

Each token records source location information through line and column values.

---

### 2. Parser and AST

The parser consumes the token stream and constructs an Abstract Syntax Tree.

The current AST structure is intentionally small and compiler-oriented:

```text
Program
├── Module
└── Function
    └── CallExpression
        └── StringLiteral
```

For example:

```text
Program
  Module: forensic_demo
  Function: main
    CallExpression: print
      StringLiteral: === JOCKY DIGITAL FORENSICS ===
    CallExpression: system_info
    CallExpression: process_list
    CallExpression: network_info
    CallExpression: file_scan
      StringLiteral: .
    CallExpression: event_log
```

Keeping the AST explicit gives the semantic layer and IR generator a stable input rather than requiring either phase to inspect source text directly.

---

### 3. Semantic Analysis

The semantic analyzer verifies that a parsed program satisfies the current language rules.

Implemented validation includes:

- A module declaration must exist
- Multiple module declarations are rejected
- A `main` function must exist
- Duplicate `main` functions are rejected
- Function bodies may contain only valid call expressions
- `print()` requires exactly one string argument
- `file_scan()` requires exactly one string path
- `system_info()`, `process_list()`, `network_info()`, and `event_log()` accept no arguments
- Unknown JOCKY operations are rejected

This stage prevents structurally valid but semantically invalid programs from progressing into code generation.

---

## 🧩 JOCKY Intermediate Representation

JOCKY introduces an explicit intermediate layer named **JIR**.

JIR is the bridge between the language frontend and the LLVM-oriented backend.

Current JIR operations are:

```text
MODULE
FUNCTION
STRING
PRINT
FORENSIC_SYSTEM_INFO
FORENSIC_PROCESS_LIST
FORENSIC_NETWORK_INFO
FORENSIC_FILE_SCAN
FORENSIC_EVENT_LOG
RETURN
```

Example:

```text
MODULE forensic_demo

FUNCTION main
    PRINT "=== JOCKY DIGITAL FORENSICS ==="
    FORENSIC_SYSTEM_INFO
    FORENSIC_PROCESS_LIST
    FORENSIC_NETWORK_INFO
    FORENSIC_FILE_SCAN "."
    FORENSIC_EVENT_LOG
    PRINT "=== FORENSIC COLLECTION COMPLETE ==="
    RETURN
END_FUNCTION
```

### Why JIR exists

JIR is more than an export format.

It is an architectural boundary where future compiler passes can operate without needing to understand JOCKY source syntax.

Potential future passes include:

```text
JIR
 │
 ├── Normalization
 ├── Analysis
 ├── Transformation
 ├── Target Lowering
 └── Backend Preparation
```

That makes JIR a natural foundation for the next stages of the JOCKY compiler.

---

## ⚙️ LLVM Backend

The LLVM backend consumes JIR and constructs LLVM IR using LLVM's C++ APIs.

The current backend creates:

- LLVM modules
- LLVM functions
- String constants
- Runtime function declarations
- Runtime calls
- Return instructions

Representative output:

```llvm
define i32 @main() {
entry:
  call void @jocky_print(ptr @jocky_user_string)
  call void @jocky_system_info()
  call void @jocky_process_list()
  call void @jocky_network_info()
  call void @jocky_file_scan(ptr @jocky_scan_path)
  call void @jocky_event_log()
  ret i32 0
}
```

The repository can export the generated LLVM representation as a `.ll` file using the compiler's `--export-ir` option.

### Important implementation detail

The current compiler **generates LLVM IR but directly executes JIR through the native JOCKY runtime for the demonstration path**.

In other words:

```text
Compilation path:
JOCKY → Lexer → Parser → Semantic → JIR → LLVM IR

Current demo execution path:
JIR → Native JOCKY Runtime
```

This distinction is intentional and documented so the prototype does not claim a full machine-code backend that is not yet present.

---

## 🧪 Forensic Runtime

The runtime is implemented in native C++ and exposes forensic operations to the language.

### Current operations

| JOCKY operation | Current role |
|---|---|
| `print()` | Output text |
| `system_info()` | Collect basic host information |
| `process_list()` | Collect process information |
| `network_info()` | Collect network configuration information |
| `file_scan(path)` | Recursively inspect a filesystem path |
| `event_log()` | Prototype event-log analysis path |

The runtime keeps platform-specific behavior outside the parser and semantic layers.

This separation is important for future cross-platform development:

```text
                 Common JOCKY Language
                          │
                          ▼
                       JIR / LLVM
                          │
                ┌─────────┴─────────┐
                ▼                   ▼
        Windows Runtime      Ubuntu Runtime
```

The current runtime has working Windows-specific collection paths, while non-Windows support remains an expansion area.

---

## 📝 JOCKY Language Example

A complete forensic demonstration program is included in `examples/forensic_demo.jky`.

```jocky
module forensic_demo;

fn main() {
    print("=== JOCKY DIGITAL FORENSICS ===");

    system_info();

    process_list();

    network_info();

    file_scan(".");

    event_log();

    print("=== FORENSIC COLLECTION COMPLETE ===");
}
```

This single source file demonstrates how a forensic workflow can be expressed as a language program rather than a collection of operating-system commands.

---

## 📦 Compiler Artifacts

The repository already includes representative compiler artifacts:

```text
examples/forensic_demo.jky   # JOCKY source
examples/forensic_demo.jir   # JOCKY intermediate representation
examples/forensic_demo.ll    # Generated LLVM IR
```

This makes the compiler pipeline inspectable stage by stage.

---

## 🚀 Build

### Requirements

The current CMake project is configured around:

- **C++17**
- **CMake 3.20+**
- **LLVM**
- **ZLIB**
- **zstd**

The checked-in CMake configuration currently uses a Windows development environment and contains local dependency paths for LLVM, ZLIB, and zstd. These paths should be adapted to the installation layout of your machine.

### Configure and build

```bash
git clone https://github.com/vinu4545/JOCKY-forensic-framework.git
cd JOCKY-forensic-framework

mkdir build
cd build

cmake ..
cmake --build . --config Release
```

---

## ▶️ Usage

Run a JOCKY source file:

```bash
jocky ../examples/forensic_demo.jky
```

Export the intermediate artifacts:

```bash
jocky ../examples/forensic_demo.jky --export-ir
```

The compiler processes the program through:

```text
Source
  ↓
Lexer
  ↓
Parser
  ↓
AST
  ↓
Semantic Analysis
  ↓
JIR
  ↓
LLVM Code Generation
  ↓
Direct Runtime Execution
```

The `--export-ir` option additionally writes:

```text
forensic_demo.jir
forensic_demo.ll
```

next to the source file.

---

## 📁 Repository Structure

```text
JOCKY-forensic-framework/
│
├── compiler/
│   ├── driver/
│   │   └── main.cpp
│   │
│   ├── lexer/
│   │   ├── lexer.h
│   │   └── lexer.cpp
│   │
│   ├── parser/
│   │   ├── ast.h
│   │   ├── parser.h
│   │   └── parser.cpp
│   │
│   ├── semantic/
│   │   ├── semantic.h
│   │   └── semantic.cpp
│   │
│   ├── jir/
│   │   ├── jir.h
│   │   └── jir.cpp
│   │
│   └── llvm/
│       ├── llvm_codegen.h
│       └── llvm_codegen.cpp
│
├── runtime/
│   ├── jocky_runtime.h
│   └── jocky_runtime.cpp
│
├── examples/
│   ├── forensic_demo.jky
│   ├── forensic_demo.jir
│   ├── forensic_demo.ll
│   ├── hello.jky
│   └── demonstration artifacts
│
├── demo/
│   ├── hello.ll
│   └── compiled demonstration binary
│
├── vscode/
│   └── jocky-language/
│
├── CMakeLists.txt
├── Architecture.png
└── README.md
```

---

## 🧱 Design Principles

### Modular compiler design

The implementation is split into independent compiler stages:

```text
compiler/
├── lexer
├── parser
├── semantic
├── jir
└── llvm
```

This makes each stage independently understandable and gives future compiler passes a clear insertion point.

### Explicit IR boundary

JIR prevents the frontend from becoming tightly coupled to the backend.

```text
JOCKY AST
    ↓
  JIR
    ↓
LLVM IR
```

That makes backend experimentation and compiler transformation easier.

### Runtime separation

Forensic collection logic is isolated in `runtime/`.

The frontend therefore does not need to know how a particular forensic operation is implemented on a target operating system.

### Cross-platform strategy

The long-term design is a common language frontend with target-specific backend/runtime components.

```text
                JOCKY Source
                     │
               Common Frontend
                     │
                    JIR
                     │
               LLVM-oriented
                 backend
                  /     \
                 /       \
          Windows         Ubuntu
             │               │
            PE              ELF
             │               │
        Native Runtime   Native Runtime
```

The current repository establishes the common compiler foundation. Full production PE and ELF generation remains future work.

---

## 🔐 Alignment With the SIH Architecture

The complete SIH problem statement includes advanced concepts beyond the compiler core.

JOCKY therefore distinguishes clearly between what is implemented and what belongs to the broader framework roadmap.

### ✅ Implemented

- JOCKY source language
- Custom lexer
- Custom parser
- AST construction
- Semantic validation
- JOCKY Intermediate Representation
- LLVM IR generation
- Native C++ runtime
- Basic forensic collection primitives
- Direct JIR runtime execution
- JIR and LLVM export
- Example forensic programs

### 🧪 Prototype / research direction

- Compiler transformation stages
- Automated polymorphic transformation concepts
- Custom transformation and encryption concepts
- Cross-platform target strategy
- Fileless/native execution research
- Security-agent visibility research
- Kernel-level / BYOVD-related research
- Central multi-device orchestration

### 🗺️ Future framework work

- Full Windows PE backend
- Full Ubuntu ELF backend
- Expanded forensic instruction set
- Evidence correlation
- Incident reconstruction
- Multi-endpoint orchestration
- Central management interface
- Result aggregation and reporting
- Hardened authentication and transport

Advanced execution and kernel-related components are treated as controlled research areas. This repository does not claim to provide an unrestricted offensive toolkit.

---

## 📈 Development Roadmap

```text
Compiler Core
├── ✅ Lexer
├── ✅ Parser
├── ✅ AST
├── ✅ Semantic Analysis
├── ✅ JIR
├── ✅ LLVM IR Generation
└── ✅ Native Runtime

Language
├── ⬜ Variables
├── ⬜ Expressions
├── ⬜ Conditional control flow
├── ⬜ Loops
├── ⬜ User-defined functions
└── ⬜ Richer type system

Backend
├── ✅ LLVM-oriented foundation
├── ⬜ Windows PE generation
├── ⬜ Ubuntu ELF generation
└── ⬜ Target-specific optimization

Forensics
├── ✅ Initial system collection
├── ⬜ Deeper process analysis
├── ⬜ Artifact acquisition
├── ⬜ IOC hunting
├── ⬜ Event correlation
└── ⬜ Incident reconstruction

Framework
├── ⬜ Transformation pipeline
├── ⬜ Evidence packaging
├── ⬜ Multi-device execution
├── ⬜ Central management
└── ⬜ Reporting and aggregation
```

---

## 🧭 Project Philosophy

JOCKY is being built **compiler-first**.

The prototype focuses on proving the architectural core:

> **A purpose-built forensic language can be tokenized, parsed, semantically validated, lowered into a custom IR, translated into LLVM IR, and connected to a native forensic runtime.**

That foundation matters because higher-level framework capabilities can later be implemented as:

```text
Language Features
       +
Compiler Passes
       +
IR Transformations
       +
Target Backends
       +
Native Runtime Modules
       +
Management Services
```

rather than as unrelated scripts and services.

---

## 🧰 Technology Stack

| Layer | Technology |
|---|---|
| Language implementation | C++17 |
| Build system | CMake |
| Frontend | Custom Lexer + Parser |
| Intermediate representation | JIR |
| Low-level backend | LLVM |
| Runtime | Native C++ |
| Source extension | `.jky` |
| JIR extension | `.jir` |
| LLVM IR extension | `.ll` |
| Primary research domain | Digital Forensics + Cybersecurity |

---

## 🧪 Current Prototype Boundaries

This repository should be read as a **working compiler prototype**, not as a finished commercial compiler toolchain.

The strongest implemented portion is:

```text
JOCKY
  ↓
Lexer
  ↓
Parser
  ↓
AST
  ↓
Semantic Analysis
  ↓
JIR
  ↓
LLVM IR
  ↓
Native Runtime
```

The next engineering step is to turn the current LLVM-oriented representation into a more complete target backend and to expand JIR so that future transformation and forensic capabilities can be added cleanly.

---

## 📜 License

A project license should be added before external redistribution or reuse is enabled.

---

## 🏁 Project

Developed for **Smart India Hackathon 2026 — Problem Statement 26148**.

<p align="center">
  <strong>JOCKY</strong><br/>
  <sub>Language → Compiler → JIR → LLVM → Runtime → Forensic Analysis</sub>
</p>
