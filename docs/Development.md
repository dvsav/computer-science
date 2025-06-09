# How to Setup Development Environment

This document will guide you through the process of setting up typical development environment on Linux and Windows.

## On Windows

Install WSL:
```PowerShell
wsl --install
```

## On Linux (including WSL)

Install development tools:
```bash
sudo apt update
sudo apt install build-essential gdb make cmake
```

## Visual Studio Code

1. [Install VS Code](https://code.visualstudio.com/Download)

2. Launch VS Code and install the following extensions:
   - [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
   - [Remote Development](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.vscode-remote-extensionpack)

### Build Command in VS Code

#### WSL

In VS Code you can setup build commands and run them directly from VS Code GUI via menu `Terminal > Run Build Task...` or `Terminal > Run Task...`.
In order to do that you need to create `.vscode/tasks.json` file inside your workspace.
To create a single build task that runs our build script `make.sh` from WSL on Windows you can create the following `.vscode/tasks.json` file:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "(WSL) build debug version",
            "type": "process",
            "command": "wsl",
            "args": [
                "bash", "-c",
                "cd \"$(wslpath '${workspaceFolder}')\" && ./make.sh -B DEBUG=yes"
            ],
            "group": {
                "kind": "build",
                "isDefault": false
            },
            "problemMatcher": []
        }
    ]
}
```
Note that this task works under Windows only. Here's a detailed explanation of each line in the above file:  

- `"label": "(WSL) build debug version"`
  - A user-friendly name for the task.
  - This label is shown in the Run Task menu and can be used in launch.json (as a preLaunchTask).
  - You can choose any string, but it must be unique within the file.

- `"type": "process"`
  - Indicates the task runs a process directly, without going through the integrated shell.
  - This means that command refers to a system executable (like wsl, make, python, etc.).

- `"command": "wsl"`
  - Specifies the executable to run.
  - In this case, it's the `wsl` command, which launches a process inside Windows Subsystem for Linux.
  - So, everything after this is interpreted inside the WSL environment.

- `"args": [...]`
- An array of arguments to pass to the command.
- In this case:
  - `"bash"`: runs the Bash shell inside WSL
  - `"-c"`: tells Bash to execute the following command string
  - `"cd \"$(wslpath '${workspaceFolder}')\" && ./make.sh -B DEBUG=yes"`: this is the actual shell command being run inside WSL:
    - `$(wslpath ...)` converts the Windows path of `${workspaceFolder}` to a Linux-style path (`/mnt/...`)
    - `cd` into that path
    - then run the script `make.sh` with flags `-B DEBUG=yes`

- `"group": { "kind": "build", "isDefault": false }`
  - Organizes the task for VS Code’s UI:
    - `"kind": "build"` marks this task as a build task (shown in the Build Task menu).
    - `"isDefault": false` means this is not the default build task (i.e., not run automatically when you press `Ctrl+Shift+B`).

- `"problemMatcher": []`
  - Defines how VS Code should parse output from the task to detect errors/warnings.
  - An empty array [] means no error parsing - output will just go to the terminal, but VS Code won't try to detect errors or navigate to files.
  - You could attach a matcher like `$gcc` if you want VS Code to recognize compiler errors.

### Debugging Command in VS Code

#### WSL

1. Install [WSL workspaceFolder](https://marketplace.visualstudio.com/items?itemName=lfurzewaddock.vscode-wsl-workspacefolder) extension in VS Code to be able to convert `"${workspaceFolder}"` to Linux style path.

2. Create `.vscode/launch.json` file inside your workspace with the following content:
  ```json
  {
    "version": "0.2.0",
    "configurations": [
      {
        "name": "WSL Debug",
        "type": "cppdbg",
        "request": "launch",
        "program": "${command:extension.vscode-wsl-workspaceFolder}/build/GNUMake/bin/test.exe",
        "args": [],
        "stopAtEntry": false,
        "cwd": "${command:extension.vscode-wsl-workspaceFolder}",
        "environment": [],
        "externalConsole": false,
        "MIMode": "gdb",
        "miDebuggerPath": "/usr/bin/gdb",
        "pipeTransport": {
          "pipeProgram": "C:\\Windows\\Sysnative\\bash.exe",
          "pipeArgs": ["-c"],
          "debuggerPath": "/usr/bin/gdb"
        },
        "setupCommands": [
          {
            "description": "Enable pretty-printing for gdb",
            "text": "-enable-pretty-printing",
            "ignoreFailures": true
          }
        ],
        "sourceFileMap": {
          "/mnt/d": "d:\\"
        }
      }
    ]
  }
  ```
Note that this task works under Windows only. Here's a detailed explanation of each line in the above file:  

- `"name": "WSL Debug"`
  Name shown in the Run and Debug panel in VS Code when selecting which debug configuration to run.

- `"type": "cppdbg"`
  Debugger type (debugger backend to use), where `cppdbg` is for C++ using the Microsoft C/C++ debugger extension (ms-vscode.cpptools) which supports **gdb**, **lldb** and **Microsoft's C++ debugger**. 

- `"request": "launch"`
  This is a "launch" request (i.e., start the program), as opposed to an "attach" request (attach to a running process).

- `"program": "${command:extension.vscode-wsl-workspaceFolder}/build/GNUMake/bin/test.exe"`
  Path to the executable you want to debug, here using: `${command:extension.vscode-wsl-workspaceFolder}` which resolves to the WSL-style path of your workspace folder (e.g., `/mnt/d/Projects/...`), even if VS Code is running in Windows mode. This avoids issues with backslashes in `${workspaceFolder}` when used across Windows/WSL boundaries.

- `"args": []`
  An array of command-line arguments passed to the program being debugged.

- `"stopAtEntry": false`
  If `true`, the debugger pauses at the program entry point (`main()`); if `false`, it runs until the first breakpoint or program termination.

- `"cwd": "${command:extension.vscode-wsl-workspaceFolder}"`
  The current working directory of the launched program. Again, using the WSL path of the workspace.

- `"environment": []`
  Set any environment variables to use during the debugging session. Empty means use the default environment.

- `"externalConsole": false`
  If `true`, launches a new terminal window for input/output; false uses VS Code's built-in terminal.

- `"MIMode": "gdb"`
  - Specifies the debugger engine, `gdb` in this case.
  - Enables the machine interface protocol (MI) that VS Code uses to communicate with GDB (i.e., structured messages, not plain CLI).
  - Used with GDB in Linux/WSL environments.

- `"miDebuggerPath": "/usr/bin/gdb"`
  Absolute path to GDB inside WSL.

- ```json
  "pipeTransport": {
    "pipeProgram": "C:\\Windows\\Sysnative\\bash.exe",
    "pipeArgs": ["-c"],
    "debuggerPath": "/usr/bin/gdb"
  }
  ```
  - This section tells VS Code to run GDB inside WSL, even if VS Code is running on Windows:
  - `"pipeProgram": "C:\\Windows\\Sysnative\\bash.exe"`: This launches the WSL environment from Windows in a low-level way that works even from 32-bit VS Code.
  - `"pipeArgs": ["-c"]`: Tells bash to treat the next string as a shell command.
  - `"debuggerPath": "/usr/bin/gdb"`: The command `bash -c "/usr/bin/gdb"` gets executed in WSL to start the debugger.

- ```json
  "setupCommands": [
    {
      "description": "Enable pretty-printing for gdb",
      "text": "-enable-pretty-printing",
      "ignoreFailures": true
    }
  ]
  ```
  This sends a GDB command at startup to enable pretty-printing of STL containers, useful for C++ debugging.

- ```json
  "sourceFileMap": {
    "/mnt/d": "d:\\"
  }
  ```
  Tells VS Code how to map paths between WSL (Linux) and Windows: When GDB says `"breakpoint at /mnt/d/Project/file.cpp"`, VS Code can find and open `d:\Project\file.cpp`.
