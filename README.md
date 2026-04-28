# WavTool

A C++ audio processing tool for working with WAV files, built from scratch with a focus on clean architecture, manual binary parsing, and performance awareness.

## Features

Core:
- Manual parsing of WAV files (16-bit PCM only)
- Full RIFF/WAVE chunk handling (fmt, data, unknown chunks)
- Metadata extraction:
  - Sample rate
  - Channel count (mono/stereo)
  - Bit depth
  - Duration
- Validation of WAV structure

Audio Processing:
- Reverse audio (implemented)
- Trim audio by time range (planned)
- Normalize audio (planned)

File I/O:
- Load WAV files into AudioClip
- Save AudioClip to valid WAV file
- Correct stereo interleaving (L R L R)
- Full round-trip support (load -> save -> reload)

Visualization (planned):
- Waveform generation (min/max per column)
- Zoom support (time to sample mapping)

Applications:
- CLI interface (working)
- GUI waveform editor (planned)

## Architecture

wavtool_core:
- AudioClip: validated audio data model
- WavFile: WAV parsing and writing
- AudioProcessor: editing operations
- WaveformGenerator: visualization data (planned)
- Renderer: rendering (planned)

apps:
- cli: command-line interface
- gui: planned GUI application

tests:
- AudioClipTests.cpp
- WavFileTests.cpp

## Design Principles

- Validated data model:
  AudioClip guarantees correctness at construction (format, sizes, alignment)

- Separation of concerns:
  Parsing, processing, and rendering are independent

- Immutable-style processing:
  Operations return new AudioClip instances

- Manual binary parsing:
  No external libraries for WAV handling

- Performance awareness:
  Uses std::chrono for profiling and compares Debug vs Release builds

- CMake-based build system

## Build

cmake -S . -B build
cmake --build build

Release build (recommended):

cmake --build build --config Release

## Run CLI

build/Debug/wavtool_cli

or

build/Release/wavtool_cli

## Run Tests

build/Debug/wavtool_tests
build/Debug/wav_file_tests

## Testing

- Assertion-based test system
- Round-trip validation:
  load -> save -> reload
- Error handling tests for invalid files

Test folders:
test_assets/
test_output/ (ignored)

## Performance

Tested on approximately 46 MB WAV file:

Load:
- Debug: about 3.6 seconds
- Release: about 0.7 seconds

Load + Save + Reload:
- Debug: about 10 seconds
- Release: about 2.8 seconds

Primary costs:
- Disk I/O
- Sample processing loops
- Memory copies

## Current Status

- AudioClip implementation complete
- WavFile load complete
- WavFile save complete
- Reverse processing complete
- CLI testing working
- Trim not implemented
- Normalize not implemented
- Waveform generation not implemented
- GUI not started

## Roadmap

Short term:
- Implement trim (time to sample conversion)
- Implement normalize (amplitude scaling)
- Add waveform generation
- Improve test coverage

Long term:
- Build GUI waveform editor
- Add zoom and timeline interaction
- Add additional audio effects
- Support more file formats

## License

MIT License