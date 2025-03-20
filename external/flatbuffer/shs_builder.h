#ifndef SHS_BUILDER_H
#define SHS_BUILDER_H

/* Generated by flatcc 0.6.2 FlatBuffers schema compiler for C by dvide.com */

#ifndef SHS_READER_H
#include "shs_reader.h"
#endif
#ifndef FLATBUFFERS_COMMON_BUILDER_H
#include "flatbuffers_common_builder.h"
#endif
#include "flatcc/flatcc_prologue.h"
#ifndef flatbuffers_identifier
#define flatbuffers_identifier 0
#endif
#ifndef flatbuffers_extension
#define flatbuffers_extension "bin"
#endif

typedef flatbuffers_union_ref_t fb_file_table_Any_union_ref_t;
typedef flatbuffers_union_vec_ref_t fb_file_table_Any_union_vec_ref_t;
static fb_file_table_Any_union_ref_t fb_file_table_Any_clone(flatbuffers_builder_t *B, fb_file_table_Any_union_t t);

static const flatbuffers_voffset_t __fb_file_table_fileEntry_required[] = { 0 };
typedef flatbuffers_ref_t fb_file_table_fileEntry_ref_t;
static fb_file_table_fileEntry_ref_t fb_file_table_fileEntry_clone(flatbuffers_builder_t *B, fb_file_table_fileEntry_table_t t);
__flatbuffers_build_table(flatbuffers_, fb_file_table_fileEntry, 3)

static const flatbuffers_voffset_t __fb_file_table_thombStone_required[] = { 0 };
typedef flatbuffers_ref_t fb_file_table_thombStone_ref_t;
static fb_file_table_thombStone_ref_t fb_file_table_thombStone_clone(flatbuffers_builder_t *B, fb_file_table_thombStone_table_t t);
__flatbuffers_build_table(flatbuffers_, fb_file_table_thombStone, 1)

static const flatbuffers_voffset_t __fb_file_table_BinaryFileTable_required[] = { 0 };
typedef flatbuffers_ref_t fb_file_table_BinaryFileTable_ref_t;
static fb_file_table_BinaryFileTable_ref_t fb_file_table_BinaryFileTable_clone(flatbuffers_builder_t *B, fb_file_table_BinaryFileTable_table_t t);
__flatbuffers_build_table(flatbuffers_, fb_file_table_BinaryFileTable, 2)

#define __fb_file_table_fileEntry_formal_args , flatbuffers_string_ref_t v0, uint64_t v1, uint64_t v2
#define __fb_file_table_fileEntry_call_args , v0, v1, v2
static inline fb_file_table_fileEntry_ref_t fb_file_table_fileEntry_create(flatbuffers_builder_t *B __fb_file_table_fileEntry_formal_args);
__flatbuffers_build_table_prolog(flatbuffers_, fb_file_table_fileEntry, fb_file_table_fileEntry_file_identifier, fb_file_table_fileEntry_type_identifier)

#define __fb_file_table_thombStone_formal_args , uint64_t v0
#define __fb_file_table_thombStone_call_args , v0
static inline fb_file_table_thombStone_ref_t fb_file_table_thombStone_create(flatbuffers_builder_t *B __fb_file_table_thombStone_formal_args);
__flatbuffers_build_table_prolog(flatbuffers_, fb_file_table_thombStone, fb_file_table_thombStone_file_identifier, fb_file_table_thombStone_type_identifier)

#define __fb_file_table_BinaryFileTable_formal_args , fb_file_table_Any_union_vec_ref_t v1
#define __fb_file_table_BinaryFileTable_call_args , v1
static inline fb_file_table_BinaryFileTable_ref_t fb_file_table_BinaryFileTable_create(flatbuffers_builder_t *B __fb_file_table_BinaryFileTable_formal_args);
__flatbuffers_build_table_prolog(flatbuffers_, fb_file_table_BinaryFileTable, fb_file_table_BinaryFileTable_file_identifier, fb_file_table_BinaryFileTable_type_identifier)

static inline fb_file_table_Any_union_ref_t fb_file_table_Any_as_NONE(void)
{ fb_file_table_Any_union_ref_t uref; uref.type = fb_file_table_Any_NONE; uref.value = 0; return uref; }
static inline fb_file_table_Any_union_ref_t fb_file_table_Any_as_fileEntry(fb_file_table_fileEntry_ref_t ref)
{ fb_file_table_Any_union_ref_t uref; uref.type = fb_file_table_Any_fileEntry; uref.value = ref; return uref; }
static inline fb_file_table_Any_union_ref_t fb_file_table_Any_as_thombStone(fb_file_table_thombStone_ref_t ref)
{ fb_file_table_Any_union_ref_t uref; uref.type = fb_file_table_Any_thombStone; uref.value = ref; return uref; }
__flatbuffers_build_union_vector(flatbuffers_, fb_file_table_Any)

static fb_file_table_Any_union_ref_t fb_file_table_Any_clone(flatbuffers_builder_t *B, fb_file_table_Any_union_t u)
{
    switch (u.type) {
    case 1: return fb_file_table_Any_as_fileEntry(fb_file_table_fileEntry_clone(B, (fb_file_table_fileEntry_table_t)u.value));
    case 2: return fb_file_table_Any_as_thombStone(fb_file_table_thombStone_clone(B, (fb_file_table_thombStone_table_t)u.value));
    default: return fb_file_table_Any_as_NONE();
    }
}

__flatbuffers_build_string_field(0, flatbuffers_, fb_file_table_fileEntry_name, fb_file_table_fileEntry)
__flatbuffers_build_scalar_field(1, flatbuffers_, fb_file_table_fileEntry_offset, flatbuffers_uint64, uint64_t, 8, 8, UINT64_C(0), fb_file_table_fileEntry)
__flatbuffers_build_scalar_field(2, flatbuffers_, fb_file_table_fileEntry_size, flatbuffers_uint64, uint64_t, 8, 8, UINT64_C(0), fb_file_table_fileEntry)

static inline fb_file_table_fileEntry_ref_t fb_file_table_fileEntry_create(flatbuffers_builder_t *B __fb_file_table_fileEntry_formal_args)
{
    if (fb_file_table_fileEntry_start(B)
        || fb_file_table_fileEntry_offset_add(B, v1)
        || fb_file_table_fileEntry_size_add(B, v2)
        || fb_file_table_fileEntry_name_add(B, v0)) {
        return 0;
    }
    return fb_file_table_fileEntry_end(B);
}

static fb_file_table_fileEntry_ref_t fb_file_table_fileEntry_clone(flatbuffers_builder_t *B, fb_file_table_fileEntry_table_t t)
{
    __flatbuffers_memoize_begin(B, t);
    if (fb_file_table_fileEntry_start(B)
        || fb_file_table_fileEntry_offset_pick(B, t)
        || fb_file_table_fileEntry_size_pick(B, t)
        || fb_file_table_fileEntry_name_pick(B, t)) {
        return 0;
    }
    __flatbuffers_memoize_end(B, t, fb_file_table_fileEntry_end(B));
}

__flatbuffers_build_scalar_field(0, flatbuffers_, fb_file_table_thombStone_value, flatbuffers_uint64, uint64_t, 8, 8, UINT64_C(0), fb_file_table_thombStone)

static inline fb_file_table_thombStone_ref_t fb_file_table_thombStone_create(flatbuffers_builder_t *B __fb_file_table_thombStone_formal_args)
{
    if (fb_file_table_thombStone_start(B)
        || fb_file_table_thombStone_value_add(B, v0)) {
        return 0;
    }
    return fb_file_table_thombStone_end(B);
}

static fb_file_table_thombStone_ref_t fb_file_table_thombStone_clone(flatbuffers_builder_t *B, fb_file_table_thombStone_table_t t)
{
    __flatbuffers_memoize_begin(B, t);
    if (fb_file_table_thombStone_start(B)
        || fb_file_table_thombStone_value_pick(B, t)) {
        return 0;
    }
    __flatbuffers_memoize_end(B, t, fb_file_table_thombStone_end(B));
}

__flatbuffers_build_union_vector_field(1, flatbuffers_, fb_file_table_BinaryFileTable_fileTable, fb_file_table_Any, fb_file_table_BinaryFileTable)
__flatbuffers_build_union_table_vector_value_field(flatbuffers_, fb_file_table_BinaryFileTable_fileTable, fb_file_table_Any, fileEntry, fb_file_table_fileEntry)
__flatbuffers_build_union_table_vector_value_field(flatbuffers_, fb_file_table_BinaryFileTable_fileTable, fb_file_table_Any, thombStone, fb_file_table_thombStone)

static inline fb_file_table_BinaryFileTable_ref_t fb_file_table_BinaryFileTable_create(flatbuffers_builder_t *B __fb_file_table_BinaryFileTable_formal_args)
{
    if (fb_file_table_BinaryFileTable_start(B)
        || fb_file_table_BinaryFileTable_fileTable_add(B, v1)) {
        return 0;
    }
    return fb_file_table_BinaryFileTable_end(B);
}

static fb_file_table_BinaryFileTable_ref_t fb_file_table_BinaryFileTable_clone(flatbuffers_builder_t *B, fb_file_table_BinaryFileTable_table_t t)
{
    __flatbuffers_memoize_begin(B, t);
    if (fb_file_table_BinaryFileTable_start(B)
        || fb_file_table_BinaryFileTable_fileTable_pick(B, t)) {
        return 0;
    }
    __flatbuffers_memoize_end(B, t, fb_file_table_BinaryFileTable_end(B));
}

#include "flatcc/flatcc_epilogue.h"
#endif /* SHS_BUILDER_H */
