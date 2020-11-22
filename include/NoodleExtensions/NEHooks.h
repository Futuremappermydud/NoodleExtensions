#pragma once

namespace NoodleExtensions {

void InstallBeatmapObjectSpawnMovementDataHooks();
void InstallBasicBeatmapObjectManagerHooks();
void InstallSpawnRotationProcessorHooks();
void InstallObstacleControllerHooks();

// Fake notes
void InstallGameNoteControllerHooks();
void InstallBombNoteControllerHooks();

void InstallHooks();

}