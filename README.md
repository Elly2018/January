# January

My goal is createa a Touch designer alternative software.

## Feature

- [ ] Window management
- [ ] Custom project file
- [ ] 3D renderering
- [ ] 2D Renderering
- [ ] Node editor
- [ ] Shader compile in real-time
- [ ] Decklink SDK support
- [ ] Raw Websocket, HTTP, UDP, TCP Server/Client support
- [ ] Custom python-like script support (Multithread)

## CLI

You can use --help to check all the command usages.

### -v --verbose

Print detail information.

### -p --path

Project path target in the local machine.

## Structure

```mermaid
---
title: Software Structure
---
graph LR
    subgraph System
        SystemH["system.h
        Main system control flow"];
        WindowH["window.h
        Handle SDL window and vulkan"];
    end
    subgraph GUI
        ViewManagerH["manager.h
        Handle all the GUI widgets"];
    end
    subgraph Engine
        EngineH["engine.h
        The application core
        All important things are in here"];
    end
```

Software Structure shows the different region of the code

```mermaid
---
title: Init Flow
---
sequenceDiagram
    participant main.h;
    participant system.h;
    participant window.h;
    participant engine.h;
    participant config;
    participant view_manager.h;
    

    main.h->>system.h: Init Call
    system.h->>window.h: Init Call
    system.h->>engine.h: Init Call
    config->>engine.h: Load config.json
    engine.h->>view_manager.h: Apply window setup
```

This is the init process flow\
It's trying to create all instance such as imgui window classes and config object\
And load the config setting from the local machine

```mermaid
---
title: Update Flow (Main Thread)
---
sequenceDiagram
    participant main.h;
    participant system.h;
    participant window.h;
    participant engine.h;
    

    main.h->>system.h: Update Call (SRun)
    system.h-->>window.h: (Thread Loop) Draw Call
    system.h-->>engine.h: (Thread Loop) Update Call
    system.h-->>system.h: (While Loop) Process Commands
```

This is the update process flow\
The graph only shows the main thread part.\
To prevent confusion.