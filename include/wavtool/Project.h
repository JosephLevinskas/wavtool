#pragma once

#include "wavtool/AudioClip.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace wavtool {

class Project {
public:
    explicit Project(size_t maxHistorySize = 50);

    bool hasClip() const;
    const AudioClip& getClip() const;

    void loadClip(AudioClip clip);
    void applyEdit(AudioClip editedClip);

    bool canUndo() const;
    bool canRedo() const;

    void undo();
    void redo();

private:
    std::optional<AudioClip> currentClip;
    std::vector<AudioClip> undoStack;
    std::vector<AudioClip> redoStack;
    size_t maxHistorySize;
};

}