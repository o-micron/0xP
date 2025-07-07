#include "processor.slang"

[numthreads(256, 1, 1)]
void
computeMain(uint3   threadId: SV_DispatchThreadID,
            uniform RWStructuredBuffer<XPXPUProcessor> processor,
            uniform uint                               vectorSize)
{
    uint idx = threadId.x;
    if (idx >= vectorSize) return;
    xp_xpu_processor_run(&processor[idx]);
}
