#include "wavtool/Project.h"
#include "wavtool/AudioClip.h"

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

wavtool::AudioClip makeClip(
    const std::string& name,
    std::vector<int16_t> left
) {
    return wavtool::AudioClip(
        1,
        1,
        48000,
        96000,
        2,
        16,
        left,
        {},
        name,
        "test_assets/" + name
    );
}

void testNewProjectHasNoClip() {
    wavtool::Project project;

    require(!project.hasClip(), "New project should not have a clip");
    require(!project.canUndo(), "New project should not be able to undo");
    require(!project.canRedo(), "New project should not be able to redo");
}

void testGetClipThrowsWhenEmpty() {
    wavtool::Project project;

    bool threw = false;

    try {
        project.getClip();
    } catch (const std::runtime_error&) {
        threw = true;
    }

    require(threw, "getClip should throw when no clip is loaded");
}

void testLoadClipSetsCurrentClip() {
    wavtool::Project project;

    project.loadClip(makeClip("a.wav", {1, 2, 3}));

    require(project.hasClip(), "Project should have clip after loadClip");
    require(project.getClip().getName() == "a.wav", "Loaded clip name incorrect");
    require(project.getClip().getLeft()[0] == 1, "Loaded clip data incorrect");
}

void testLoadClipClearsHistory() {
    wavtool::Project project;

    project.loadClip(makeClip("a.wav", {1, 2, 3}));
    project.applyEdit(makeClip("b.wav", {4, 5, 6}));

    require(project.canUndo(), "Project should be able to undo after edit");

    project.loadClip(makeClip("c.wav", {7, 8, 9}));

    require(!project.canUndo(), "loadClip should clear undo history");
    require(!project.canRedo(), "loadClip should clear redo history");
    require(project.getClip().getName() == "c.wav", "loadClip should replace current clip");
}

void testApplyEditCreatesUndoHistory() {
    wavtool::Project project;

    project.loadClip(makeClip("a.wav", {1, 2, 3}));
    project.applyEdit(makeClip("b.wav", {4, 5, 6}));

    require(project.getClip().getName() == "b.wav", "applyEdit should replace current clip");
    require(project.canUndo(), "applyEdit should create undo history");
    require(!project.canRedo(), "applyEdit should not create redo history");
}

void testApplyEditWithoutClipBehavesLikeLoad() {
    wavtool::Project project;

    project.applyEdit(makeClip("a.wav", {1, 2, 3}));

    require(project.hasClip(), "applyEdit without clip should load clip");
    require(project.getClip().getName() == "a.wav", "applyEdit without clip should set current clip");
    require(!project.canUndo(), "applyEdit without clip should not create undo history");
    require(!project.canRedo(), "applyEdit without clip should not create redo history");
}

void testUndoRestoresPreviousClip() {
    wavtool::Project project;

    project.loadClip(makeClip("a.wav", {1, 2, 3}));
    project.applyEdit(makeClip("b.wav", {4, 5, 6}));

    project.undo();

    require(project.getClip().getName() == "a.wav", "undo should restore previous clip");
    require(!project.canUndo(), "undo should consume undo history");
    require(project.canRedo(), "undo should create redo history");
}

void testRedoRestoresUndoneClip() {
    wavtool::Project project;

    project.loadClip(makeClip("a.wav", {1, 2, 3}));
    project.applyEdit(makeClip("b.wav", {4, 5, 6}));

    project.undo();
    project.redo();

    require(project.getClip().getName() == "b.wav", "redo should restore undone clip");
    require(project.canUndo(), "redo should recreate undo history");
    require(!project.canRedo(), "redo should consume redo history");
}

void testApplyEditClearsRedoHistory() {
    wavtool::Project project;

    project.loadClip(makeClip("a.wav", {1, 2, 3}));
    project.applyEdit(makeClip("b.wav", {4, 5, 6}));
    project.applyEdit(makeClip("c.wav", {7, 8, 9}));

    project.undo();

    require(project.canRedo(), "undo should create redo history");

    project.applyEdit(makeClip("d.wav", {10, 11, 12}));

    require(!project.canRedo(), "new edit should clear redo history");
    require(project.getClip().getName() == "d.wav", "new edit should become current clip");
}

void testUndoThrowsWhenEmpty() {
    wavtool::Project project;

    bool threw = false;

    try {
        project.undo();
    } catch (const std::runtime_error&) {
        threw = true;
    }

    require(threw, "undo should throw when undo stack is empty");
}

void testRedoThrowsWhenEmpty() {
    wavtool::Project project;

    bool threw = false;

    try {
        project.redo();
    } catch (const std::runtime_error&) {
        threw = true;
    }

    require(threw, "redo should throw when redo stack is empty");
}

void testHistoryLimitRemovesOldestState() {
    wavtool::Project project(2);

    project.loadClip(makeClip("a.wav", {1}));
    project.applyEdit(makeClip("b.wav", {2}));
    project.applyEdit(makeClip("c.wav", {3}));
    project.applyEdit(makeClip("d.wav", {4}));

    project.undo();

    require(project.getClip().getName() == "c.wav", "First undo should restore c");

    project.undo();

    require(project.getClip().getName() == "b.wav", "Second undo should restore b");

    require(!project.canUndo(), "Oldest state should have been removed by history limit");
}

} // namespace

int main() {
    try {
        testNewProjectHasNoClip();
        testGetClipThrowsWhenEmpty();
        testLoadClipSetsCurrentClip();
        testLoadClipClearsHistory();
        testApplyEditCreatesUndoHistory();
        testApplyEditWithoutClipBehavesLikeLoad();
        testUndoRestoresPreviousClip();
        testRedoRestoresUndoneClip();
        testApplyEditClearsRedoHistory();
        testUndoThrowsWhenEmpty();
        testRedoThrowsWhenEmpty();
        testHistoryLimitRemovesOldestState();

        std::cout << "All Project tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Project test failed: " << e.what() << '\n';
        return 1;
    }
}