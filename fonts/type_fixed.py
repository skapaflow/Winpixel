#getline{Blender -b -P type_fixed.py}
# Blender 2.79 - generates a binary .vtf (vector, ASCII-indexed table)
# for the winpixel engine. Runs in background, no window operators.
#
# Usage (everything after "--" goes to the script):
#
#   # default font (arial) -> arial.vtf
#   blender --background --python type_fixed.py
#
#   # another font -> automatic output "<name>.vtf" next to the script
#   blender --background --python type_fixed.py -- "C:/Windows/Fonts/times.ttf"
#
#   # another font + explicit output path
#   blender --background --python type_fixed.py -- "C:/.../comic.ttf" "comic.vtf"

import bpy
import bmesh
import os
import sys

# Arguments passed after "--"
argv = sys.argv
argv = argv[argv.index("--") + 1:] if "--" in argv else []

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

# 1st arg: font path (default: arial)
FONT_PATH = argv[0] if len(argv) >= 1 else "C:/Windows/Font/arial.ttf"

# 2nd arg: output path. Without it: "<font_name>.vtf" next to the script.
#   e.g.: arial.ttf -> arial.vtf  (vector-type-font, binary)
if len(argv) >= 2:
    OUT_PATH = argv[1]
else:
    base = os.path.splitext(os.path.basename(FONT_PATH))[0]
    OUT_PATH = os.path.join(SCRIPT_DIR, base + ".vtf")

# Printable ASCII range
FIRST = 32
LAST = 126

scene = bpy.context.scene

# Load the font ONCE
font = bpy.data.fonts.load(FONT_PATH)


def ink_width(body):
    """Width (max-min on X) of a string's geometry. 0 if empty."""
    cu = bpy.data.curves.new("tmp", type='FONT')
    cu.body = body
    cu.font = font
    ob = bpy.data.objects.new("tmp", cu)
    me = ob.to_mesh(scene, True, 'PREVIEW')
    if me is None or len(me.vertices) == 0:
        w = 0.0
    else:
        xs = [v.co.x for v in me.vertices]
        w = max(xs) - min(xs)
    if me is not None:
        bpy.data.meshes.remove(me)
    bpy.data.objects.remove(ob)
    bpy.data.curves.remove(cu)
    return w


# Global accumulators
all_verts = []     # floats: x, y, x, y, ...
all_indices = []   # int base 0, global into all_verts
glyphs = []        # (code, ch, voff, vcount, ioff, icount, advance)

for code in range(FIRST, LAST + 1):
    ch = chr(code)
    name = "char_%d" % code

    curve = bpy.data.curves.new(name=name, type='FONT')
    curve.body = ch
    curve.font = font
    obj = bpy.data.objects.new(name, curve)

    me = obj.to_mesh(scene, True, 'PREVIEW')

    voff = len(all_verts) // 2
    ioff = len(all_indices)
    vcount = 0
    icount = 0

    if me is not None and len(me.polygons) > 0:
        bm = bmesh.new()
        bm.from_mesh(me)
        bmesh.ops.triangulate(bm, faces=bm.faces[:])
        bm.verts.index_update()

        for v in bm.verts:
            # Y flipped: winpixel engine uses Y down (screen space)
            all_verts.append(v.co.x)
            all_verts.append(-v.co.y)
        vcount = len(bm.verts)

        for face in bm.faces:
            for vert in face.verts:
                all_indices.append(voff + vert.index)
        icount = len(bm.faces) * 3

        bm.free()

    # Horizontal advance to the next character
    if ch == ' ':
        advance = ink_width("i i") - ink_width("ii")
    elif me is not None and len(me.vertices) > 0:
        xs = [v.co.x for v in me.vertices]
        w_single = max(xs) - min(xs)
        advance = ink_width(ch + ch) - w_single
    else:
        advance = 0.0

    glyphs.append((code, ch, voff, vcount, ioff, icount, advance))
    print("ok %d %r  verts=%d tris=%d adv=%.4f" % (code, ch, vcount, icount // 3, advance))

    if me is not None:
        bpy.data.meshes.remove(me)
    bpy.data.objects.remove(obj)
    bpy.data.curves.remove(curve)


# ---- Writes the binary .vtf file (vector-type-font) ----
# Layout (little-endian), read by src/font.c -> wpx_font_load():
#   char  magic[4] = "VTF1"
#   int32 first, last, vert_floats, index_count, glyph_count
#   float verts[vert_floats]            (x,y interleaved; Y is down)
#   int32 indices[index_count]          (base 0, index vertices)
#   glyph[glyph_count]:
#       int32 voff, vcount, ioff, icount
#       float advance
import struct

with open(OUT_PATH, "wb") as f:
    f.write(b"VTF1")
    f.write(struct.pack("<5i", FIRST, LAST, len(all_verts), len(all_indices), len(glyphs)))
    f.write(struct.pack("<%df" % len(all_verts), *all_verts))
    f.write(struct.pack("<%di" % len(all_indices), *all_indices))
    for (code, ch, voff, vcount, ioff, icount, advance) in glyphs:
        f.write(struct.pack("<4if", voff, vcount, ioff, icount, advance))

print("Gerado: %s" % OUT_PATH)
print("Total: %d glyphs, %d vertices, %d indices" %
      (len(glyphs), len(all_verts) // 2, len(all_indices)))
