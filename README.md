# WavTool

A C++ audio processing tool for working with WAV files, built from scratch with a focus on clean architecture and manual binary parsing.

## Features

Core:
- Manual parsing of WAV files (16-bit PCM)
- Extraction of metadata (sample rate, channels, bit depth, duration)

Audio Processing:
- Reverse audio
- Trim audio by time range
- Normalize audio

Visualization:
- Waveform data generation (min/max per column)
- Zoom support (time to sample mapping)

Applications:
- CLI interface (current)
- GUI (planned)

## Architecture

The project is structured with a clear separation of concerns:

wavtool_core:
- AudioClip: validated audio data model
- WavFile: WAV parsing and writing
- AudioProcessor: editing operations
- WaveformGenerator: visualization data
- Renderer: rendering (GUI later)

apps:
- cli: command-line interface
- gui: planned GUI application

tests:
- AudioClipTests.cpp

## Design Principles

- Validated data model: AudioClip guarantees valid data at construction
- Separation of concerns: parsing, processing, and rendering are independent
- Immutable-style processing: operations return new AudioClip instances
- Manual WAV handling: no external libraries for parsing
- CMake-based build system

## Build

cmake -S . -B build
cmake --build build

## Run CLI

build/Debug/wavtool_cli

## Run Tests

build/Debug/wavtool_tests

## Current Status

- Project structure and CMake setup complete
- AudioClip implementation with validation complete
- Unit tests for AudioClip complete
- WavFile parsing in progress
- Audio processing not started
- GUI not started

## Roadmap

- Implement WAV file reader/writer (WavFile)
- Add audio processing (reverse, trim, normalize)
- Generate waveform data
- Build GUI
- Add project save/load system
- Improve testing with a framework

## License

This project is licensed under the MIT License.