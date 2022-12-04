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

- NESASM(MagicKit) v2.51

    http://www.magicengine.com/mkit/

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

1. (For Windows only) Install "WSL(ms-vscode-remote.remote-containers)" extension to Visual Studio Code.

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

Create "sample.nes" and run the following command in the Visual Studio Code terminal.(For NESASM)
```
# nesasm sample.nes
```

Have fun!