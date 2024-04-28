#ifndef _RENDER_PASSES_H_
#define _RENDER_PASSES_H_

// elem_count returns the size of this enum as if it was an array.
// The passes have to be IN ORDER !!
const enum RenderPasses {DEFERRED_GEOMETRY, DEFERRED_ILLUMINATION, FORWARD_PASS, POSTPROCESS, ELEM_COUNT};

#endif