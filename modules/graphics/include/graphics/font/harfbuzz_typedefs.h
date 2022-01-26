#ifndef HARFBUZZ_TYPEDEFS_H
#define HARFBUZZ_TYPEDEFS_H
#ifndef HARFBUZZ_HEADERS_H
extern "C" {
struct FT_LibraryRec_;
struct FT_FaceRec_;
typedef struct FT_LibraryRec_ *FT_Library;
typedef struct FT_FaceRec_ *FT_Face;
struct hb_face_t;
}
#endif
#endif //HARFBUZZ_TYPEDEFS_H
