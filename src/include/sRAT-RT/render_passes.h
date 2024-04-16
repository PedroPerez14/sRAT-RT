#ifndef _RENDER_PASSES_H_
#define _RENDER_PASSES_H_

// elem_count returns the size of this enum as if it was an array, it's not a render queue.
static const enum RenderPasses {DEFERRED_GEOMETRY, DEFERRED_ILLUMINATION, DIRECT, ELEM_COUNT};

#endif