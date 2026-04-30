
#include "wavtool/Project.h"
#include <stdexcept>
#include <vector>

namespace wavtool {

Project::Project(size_t maxHistorySize_) 
    : maxHistorySize(maxHistorySize_) {}

bool Project::hasClip() const {
    return currentClip.has_value();
}

const AudioClip& Project::getClip() const {
    if (!currentClip.has_value()) {
        throw std::runtime_error("No clip loaded");
    }
    return *currentClip;
}

bool Project::canUndo() const {
    return !undoStack.empty();
}

bool Project::canRedo() const {
    return !redoStack.empty();
}

void Project::loadClip(AudioClip clip) {
    currentClip = std::move(clip);
    undoStack.clear();
    redoStack.clear();
}

void Project::applyEdit(AudioClip editedClip) {
    if (!currentClip.has_value()) {
        currentClip = std::move(editedClip);
        return;
    }
    undoStack.push_back(*currentClip);
    currentClip = std::move(editedClip);
    redoStack.clear();

    if (undoStack.size() > maxHistorySize) {
        undoStack.erase(undoStack.begin());
    }

}

void Project::undo() {
    if (!canUndo())  {
        throw std::runtime_error("Can't undo with empty undo stack");
    }
    if (!currentClip.has_value()) {
        throw std::runtime_error("Invariant violated: undo with no current clip");
    }

    redoStack.push_back(*currentClip);
    AudioClip restored = std::move(undoStack.back());
    undoStack.pop_back();
    currentClip = std::move(restored);
}

void Project::redo() {
    if (!canRedo()) {
        throw std::runtime_error("Can't redo with empty redo stack");
    }
    if (!currentClip.has_value()) {
        throw std::runtime_error("Invariant violated: redo with no current clip");
    }

    undoStack.push_back(*currentClip);

    if (undoStack.size() > maxHistorySize) {
        undoStack.erase(undoStack.begin());
    }

    AudioClip restored = std::move(redoStack.back());
    redoStack.pop_back();
    currentClip = std::move(restored);
}

}