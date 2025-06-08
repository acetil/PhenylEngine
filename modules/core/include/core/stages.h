#pragma once

namespace phenyl::core {
struct PostInit {};

struct FrameBegin {};

struct GlobalFixedTimestep {};

struct GlobalVariableTimestep {};

struct Update {};

struct PostUpdate {};

struct Render {};

struct FixedUpdate {};

struct PhysicsUpdate {};
} // namespace phenyl::core
