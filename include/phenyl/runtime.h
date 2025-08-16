#pragma once

#include "core/runtime.h"
#include "core/stages.h"

namespace phenyl {
using PhenylRuntime = core::PhenylRuntime;

template <typename... Args>
using Resources = core::Resources<Args...>;

using PostInit = core::PostInit;
using FrameBegin = core::FrameBegin;
using Update = core::Update;
using Render = core::Render;
using FixedUpdate = core::FixedUpdate;
} // namespace phenyl
