# WavTool

A production-focused C++ audio processing tool for working with WAV files, built from scratch with clean architecture, manual binary parsing, and performance-aware design.

## Features

Core:
- Manual parsing of WAV files
- Supports 16-bit PCM WAV files
- Supports mono and stereo audio
- Full RIFF/WAVE chunk handling:
  - RIFF header
  - fmt chunk
  - data chunk
  - unknown chunks
- Metadata extraction:
  - Sample rate
  - Channel count
  - Bit depth
  - Duration
  - Frame count
- Strict validation of audio structure

Audio Processing:
- Reverse audio
- Trim audio by time range
- Normalize audio amplitude
- Immutable-style processing:
  - Processing functions return new AudioClip objects

File I/O:
- Load WAV files into AudioClip
- Save AudioClip to valid WAV file
- Correct stereo interleaving:
  - L R L R
- Full round-trip support:
  - load -> save -> reload

Visualization:
- Waveform generation
- Min/max amplitude per column
- Stereo-aware waveform generation
- Renderer:
  - Converts waveform data to a pixel buffer
  - Maps amplitude to image coordinates
  - Draws waveform columns
  - Draws center line
  - Applies gradient coloring
  - Writes valid 24-bit BMP files

Applications:
- CLI interface
- GUI waveform editor planned

## Architecture

wavtool_core:
- AudioClip:
  - Validated audio data model
  - Owns left/right sample vectors
  - Stores clip name and original file path
- WavFile:
  - Loads WAV files
  - Saves WAV files
  - Handles binary WAV format details
- AudioProcessor:
  - Reverse
  - Trim
  - Normalize
- WaveformGenerator:
  - Converts AudioClip data into drawable waveform min/max points
- Renderer:
  - Converts waveform points into BMP output
- Library:
  - Stores and manages saved file paths
- ProjectState:
  - Early project-state layer
- ProjectSerializer:
  - Early project persistence layer

apps:
- cli:
  - Command-line application entry point
- gui:
  - Planned GUI application

tests:
- AudioClipTests.cpp
- AudioProcessorTests.cpp
- WavFileTests.cpp
- WaveformGeneratorTests.cpp
- RendererTests.cpp

## Design Principles

- Validated data model:
  - AudioClip guarantees correctness at construction

- Separation of concerns:
  - AudioClip stores data
  - WavFile handles file format I/O
  - AudioProcessor transforms audio
  - WaveformGenerator prepares visual data
  - Renderer outputs images

- Immutable-style editing:
  - Audio edits return new AudioClip objects instead of mutating the original

- Manual binary parsing:
  - No external WAV parsing libraries

- Testable pipeline:
  - Audio data, processing, waveform generation, and rendering are tested separately

- CMake-based build system

## Build

```bash
cmake -S . -B build
cmake --build build
```

Release build:

```bash
cmake --build build --config Release
```

## Run CLI

```bash
build/Debug/wavtool_cli
```

or:

```bash
build/Release/wavtool_cli
```

## Run Tests

```bash
build/Debug/wavtool_tests
```

## Testing

Current test coverage includes:

- AudioClip validation
- WAV load behavior
- WAV save behavior
- Load -> save -> reload round-trip behavior
- AudioProcessor behavior:
  - reverse
  - trim
  - normalize
- WaveformGenerator behavior:
  - column count validation
  - min/max generation
  - mono/stereo behavior
- Renderer behavior:
  - BMP header validation
  - invalid input handling
  - center line rendering
  - waveform pixel generation

Test folders:

```text
test_assets/
test_output/
```

## Current Status

Complete:
- AudioClip
- WavFile load
- WavFile save
- AudioProcessor reverse
- AudioProcessor trim
- AudioProcessor normalize
- WaveformGenerator
- Renderer BMP output
- CLI entry point
- Core test suite

In progress:
- ProjectState
- ProjectSerializer
- Application-level project management

Not started:
- Undo/redo project history
- Full App class
- GUI

## Roadmap

Short term:
- Design Project class
- Add undo/redo using full AudioClip snapshots
- Integrate Project with AudioProcessor
- Clean up ProjectState and ProjectSerializer roles

Mid term:
- Build App class for CLI command flow
- Add project saving/loading
- Improve renderer output quality
- Add thicker waveform lines
- Add better error handling

Long term:
- Build GUI waveform editor
- Add waveform zooming
- Add scrubbing/timeline interaction
- Add export workflow
- Add more audio effects
- Support more audio formats

## License

MIT License
