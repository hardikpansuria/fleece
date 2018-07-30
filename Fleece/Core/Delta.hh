//
// Delta.hh
//
// Copyright © 2018 Couchbase. All rights reserved.
//

#pragma once
#include "slice.hh"

namespace fleece {
    class Value;
    class Encoder;
    class JSONEncoder;
    class SharedKeys;


    /** Returns JSON that describes the changes to turn the value `old` into `nuu`.
        If the values are equal, returns nullslice. */
    alloc_slice CreateDelta(const Value *old, SharedKeys *oldSK,
                            const Value *nuu, SharedKeys *nuuSK,
                            bool json5 =false);

    /** Writes JSON that describes the changes to turn the value `old` into `nuu`.
        If the values are equal, writes nothing and returns false. */
    bool CreateDelta(const Value *old, SharedKeys *oldSK,
                     const Value *nuu, SharedKeys *nuuSK,
                     JSONEncoder&);


    /** Applies the JSON data created by `CreateDelta` to the value `old`, which must be equal
        to the `old` value originally passed to `CreateDelta`, and returns a Fleece document
        equal to the original `nuu` value.
        If the delta is malformed or can't be applied to `old`, throws a FleeceException. */
    alloc_slice ApplyDelta(const Value *old, SharedKeys*, slice jsonDelta, bool isJSON5 =false);

    /** Applies the (parsed) JSON data created by `CreateDelta` to the value `old`, which must be
        equal to the `old` value originally passed to `CreateDelta`, and writes the corresponding
        `nuu` value to the encoder.
        If the delta is malformed or can't be applied to `old`, throws a FleeceException. */
    void ApplyDelta(const Value *old, SharedKeys*, const Value* NONNULL delta, Encoder&);

}
