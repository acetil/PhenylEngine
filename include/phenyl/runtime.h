#pragma once

#include "runtime/delta_time.h"
#include "runtime/runtime.h"
#include "runtime/stages.h"

namespace phenyl {
    using PhenylRuntime = runtime::PhenylRuntime;

    template <typename ...Args>
    using Resources = runtime::Resources<Args...>;

    using PostInit = runtime::PostInit;
    using FrameBegin = runtime::FrameBegin;
    using Update = runtime::Update;
    using Render = runtime::Render;
    using FixedUpdate = runtime::FixedUpdate;

    using DeltaTime = runtime::DeltaTime;
    using FixedDelta = runtime::FixedDelta;
}