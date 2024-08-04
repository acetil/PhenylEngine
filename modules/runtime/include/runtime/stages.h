#pragma once

namespace phenyl::runtime {
    struct PostInit {};
    struct FrameBegin {};
    struct GlobalFixedTimestep {};
    struct GlobalVariableTimestep {};

    struct Update {};
    struct Render {};
    struct FixedUpdate {};
    struct PhysicsUpdate {};
}