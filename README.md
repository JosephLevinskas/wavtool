# WavTool

A production-focused C++ audio processing tool for working with WAV files, built from scratch with clean architecture, manual binary parsing, and performance-aware design.

## Features

### Core Audio Capabilities
- **WAV File Support**:
  - Manual parsing of WAV files (no external libraries)
  - 16-bit PCM WAV files
  - Mono and stereo audio
  - Full RIFF/WAVE chunk handling (RIFF header, fmt chunk, data chunk, unknown chunks)
  - Strict validation of audio structure

- **Metadata Extraction**:
  - Sample rate, channel count, bit depth
  - Duration and frame count
  - File path and clip naming

### Audio Processing
- **Real-time Operations**:
  - Reverse audio
  - Trim audio by time range
  - Normalize audio amplitude
  - Immutable-style processing (operations return new AudioClip objects)

### File I/O
- Load WAV files into AudioClip
- Save AudioClip to valid WAV file
- Correct stereo interleaving (L R L R)
- Full round-trip support (load → save → reload)

### Visualization
- **Waveform Generation**:
  - Min/max amplitude per column
  - Stereo-aware waveform generation
  
- **Rendering**:
  - Converts waveform data to pixel buffers
  - 24-bit BMP output
  - Amplitude-to-coordinate mapping
  - Center line drawing
  - Gradient coloring support

### Project Management
- Project state management with undo/redo support
- Project serialization (save/load project files)
- Clip library management
- Multi-clip editing workflow

## Architecture

### wavtool_core Library
The core library contains all audio processing and file I/O logic:

- **AudioClip**:
  - Validated audio data model
  - Owns left/right sample vectors
  - Stores clip name and original file path

- **WavFile**:
  - Loads WAV files with binary parsing
  - Saves WAV files in proper format
  - Handles RIFF/WAVE structure

- **AudioProcessor**:
  - Reverse, trim, normalize operations
  - Returns new AudioClip objects (immutable style)

- **WaveformGenerator**:
  - Converts AudioClip data into drawable waveform min/max points
  - Configurable column count
  - Mono/stereo support

- **Renderer**:
  - Converts waveform points into 24-bit BMP files
  - Pixel mapping and center line rendering
  - Gradient coloring

- **Project**:
  - Manages edited audio clips
  - Undo/redo history using full AudioClip snapshots
  - State management for current working clip

- **ProjectSerializer**:
  - Project file persistence
  - Save/load project state

- **Library**:
  - Stores and manages saved file paths
  - Clip metadata storage

- **App**:
  - CLI command interface
  - Command routing and execution

### Applications

- **CLI** (`wavtool_cli`):
  - Interactive command-line interface
  - Commands: load, save, reverse, trim, normalize, render, undo, redo, info
  - REPL-style interaction

- **GUI** (`wavtool_gui`):
  - Not yet started (placeholder in place)

### Test Suite
Comprehensive test coverage with dedicated test executables:
- `AudioClipTests` - Validation and data integrity
- `WavFileTests` - File I/O and format compliance
- `AudioProcessorTests` - Audio transformation logic
- `WaveformGeneratorTests` - Waveform data generation
- `RendererTests` - BMP output and rendering
- `ProjectTests` - Project state and undo/redo
- `AppTests` - CLI application logic

Test assets and output stored in:
```
test_assets/    # Input test WAV files
test_output/    # Generated output (BMPs, etc.)
```

## Design Principles

- **Validated Data Model**:
  - AudioClip guarantees correctness at construction
  - Strict validation prevents invalid state

- **Separation of Concerns**:
  - AudioClip stores data
  - WavFile handles file format I/O
  - AudioProcessor transforms audio
  - WaveformGenerator prepares visual data
  - Renderer outputs images
  - Project manages workflow

- **Immutable-Style Editing**:
  - Audio edits return new AudioClip objects instead of mutating
  - Enables undo/redo via full snapshots

- **No External Dependencies**:
  - Manual binary parsing (no WAV libraries)
  - Pure C++20 (no third-party audio frameworks)

- **Testable Pipeline**:
  - Each component can be tested independently
  - Clear boundaries between layers

## Build Instructions

### Prerequisites
- CMake 3.20+
- C++20 compatible compiler
- Visual Studio 2022 or equivalent (for build files)

### Build

Debug build:
```bash
cmake -S . -B build
cmake --build build
```

Release build:
```bash
cmake --build build --config Release
```

### Run CLI

Debug:
```bash
build/Debug/wavtool_cli
```

Release:
```bash
build/Release/wavtool_cli
```

### Run Tests

```bash
cmake --build build --target RUN_TESTS
```

Or run individual test executables:
```bash
build/Debug/audio_clip_tests
build/Debug/wav_file_tests
build/Debug/audio_processor_tests
build/Debug/waveform_generator_tests
build/Debug/renderer_tests
build/Debug/project_tests
build/Debug/app_tests
```

## Current Implementation Status

### Completed ✓
- AudioClip data model and validation
- WavFile binary parsing and I/O
- AudioProcessor (reverse, trim, normalize)
- WaveformGenerator with mono/stereo support
- Renderer with 24-bit BMP output
- Project state management
- ProjectSerializer for persistence
- Library file management
- App class with full CLI command set
- Comprehensive test suite (7 test executables)
- CMake build system

### In Progress 🔄
- CLI command implementation details
- Extended project workflow features

### Not Started 📋
- GUI application interface
- Advanced audio effects
- Additional audio formats (MP3, FLAC, etc.)
- Waveform zooming and scrubbing
- Performance optimizations

## Project Structure

```
├── CMakeLists.txt                 # Build configuration
├── include/wavtool/               # Public headers
│   ├── App.h
│   ├── AudioClip.h
│   ├── AudioProcessor.h
│   ├── Library.h
│   ├── Project.h
│   ├── ProjectSerializer.h
│   ├── Renderer.h
│   ├── WaveformGenerator.h
│   └── WavFile.h
├── src/                           # Implementation
│   ├── App.cpp
│   ├── AudioClip.cpp
│   ├── AudioProcessor.cpp
│   ├── Library.cpp
│   ├── Project.cpp
│   ├── ProjectSerializer.cpp
│   ├── Renderer.cpp
│   ├── WaveformGenerator.cpp
│   └── WavFile.cpp
├── apps/
│   ├── cli/main.cpp              # CLI entry point
│   └── gui/main.cpp              # GUI entry point (empty)
├── tests/                         # Test implementations
│   ├── AppTests.cpp
│   ├── AudioClipTests.cpp
│   ├── AudioProcessorTests.cpp
│   ├── ProjectTests.cpp
│   ├── RendererTests.cpp
│   ├── WaveformGeneratorTests.cpp
│   └── WavFileTests.cpp
├── test_assets/                   # Test input files
├── test_output/                   # Test output directory
└── build/                         # Build output (generated)
```

## Command-Line Interface

The CLI application provides the following commands:

- `load <file>` - Load a WAV file
- `save <file>` - Save current clip to file
- `reverse` - Reverse the current clip
- `trim <start> <end>` - Trim clip to time range
- `normalize` - Normalize amplitude
- `render <output.bmp>` - Generate waveform visualization
- `undo` - Undo last operation
- `redo` - Redo last undone operation
- `info` - Display current clip information
- `help` - Show command help
- `exit` - Exit application

## Roadmap

### Phase 1: CLI Stability (Current)
- Finalize and test all CLI commands
- Improve error messaging and validation
- Add file format error recovery

### Phase 2: GUI Application
- Implement basic GUI waveform editor
- Click-to-load file interface
- Real-time waveform visualization
- Direct waveform click-and-drag trimming

### Phase 3: Advanced Features
- Add waveform zooming and panning
- Timeline scrubbing
- Multiple simultaneous clips
- More audio effects (fade in/out, compression, etc.)

### Phase 4: Format Support
- MP3 audio support
- FLAC audio support
- WAV export options

## License

MIT License
