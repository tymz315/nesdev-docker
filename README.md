# NES / Famicom Development Environment

- Maintained by:

    https://github.com/tymz315/nesdev-docker

## Bundled software

- CC65 v2.19

    https://cc65.github.io/

- asm6 v1.6

    https://3dscapture.com/NES/

- asm6f v1.6_freem02

    https://github.com/freem/asm6f

- ~~NESASM(MagicKit) v2.51~~

    ~~http://www.magicengine.com/mkit/~~

- NESASM(Unofficial MagicKit) v2.51

    http://zzo38computer.org/nes_program/ppmck.zip

## How to use this image

### Build with Makefile

For mac(Intel) or Linux

```
$ docker run --rm -v "$PWD:/workspace" -w /workspace tymz315/nesdev make
```

For Windows

```
> docker run --rm -v "%CD%:/workspace" -w /workspace tymz315/nesdev make
```

### Build with NESASM

For mac(Intel) or Linux

```
$ docker run --rm -v "$PWD:/workspace" -w /workspace tymz315/nesdev nesasm sample.nes
```

For Windows

```
> docker run --rm -v "%CD%:/workspace" -w /workspace tymz315/nesdev nesasm sample.nes
```

## To coding with Visual Studio Code

### First step

1. (For Windows only) Install WSL2 on your system.

1. Install "Docker Desktop" on your system.

1. (For Windows only) Install "WSL(ms-vscode-remote.remote-wsl)" extension to Visual Studio Code.

1. Install "Dev Containers(ms-vscode-remote.remote-containers)" extension to Visual Studio Code.

### Create Project

1. Create a project directory and open it in Visual Studio Code.

1. Create a file with the following contents as .devcontainer/devcontainer.json
    ```
    {
        "name": "NESDEV",
        "image": "tymz315/nesdev"
    }
    ```

1. Run "Dev Containers: Reopen in Container" in the command palette.

1. Visual Studio Code opens in the NESDEV container.
In Visual Studio Code, Created source code will be synced with your project directory.

### How to compile

Create "sample.nes" and run the following command in the Visual Studio Code terminal.(For NESASM example)
```
# nesasm sample.nes
```

### Example

![Development image](https://raw.githubusercontent.com/tymz315/nesdev-docker/5a20062e86df1907762f9936e25be0044865c7a6/assets/images/vscode.png)

- .devcontainer/devcontainer.json

    Uses Microsoft Hex Editor extension. And associate it *.nes file.

    ```
    {
        "name": "NESDEV",
        "image": "tymz315/nesdev",
        "extensions": [
            "ms-vscode.hexeditor"
        ],
        "settings": {
            "workbench.editorAssociations": {
                "*.nes": "hexEditor.hexedit"
            }
        }
    }
    ```

- .vscode/tasks.json

    Press *Ctrl+Shift+B* to run make.

    ```
    {
        "version": "2.0.0",
        "tasks": [
            {
                "label": "Make",
                "type": "shell",
                "command": "make all",
                "problemMatcher": [],
                "group": {
                    "kind": "build",
                    "isDefault": true
                }
            }
        ]
    }
    ```

Have fun!

# Changelog

## [1.0.2] - 2025-03-31

Add support for ARM64 architecture.

## [1.0.1] - 2022-12-06

Changed NESASM to unofficial version.

## [1.0.0] - 2022-12-04

First release.

