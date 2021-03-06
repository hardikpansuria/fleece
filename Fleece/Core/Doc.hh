//
// Doc.hh
//
// Copyright © 2018 Couchbase. All rights reserved.
//

#pragma once
#include "RefCounted.hh"
#include "Value.hh"
#include "fleece/slice.hh"
#include <map>

namespace fleece { namespace impl {
    class SharedKeys;
    class Value;
    namespace internal {
        class Pointer;
    }


    class Scope {
    public:
        Scope(slice fleeceData,
              SharedKeys*,
              slice externDestination =nullslice) noexcept;
        ~Scope();

        static Scope* containing(const Value* NONNULL) noexcept;

        slice data() const                      {return _data;}
        SharedKeys* sharedKeys() const          {return _sk;}
        slice externDestination() const         {return _externDestination;}

        // For internal use:

        static SharedKeys* sharedKeys(const Value* NONNULL v) noexcept;
        const Value* resolveExternPointerTo(const void* NONNULL) const noexcept;
        static const Value* resolvePointerFrom(const internal::Pointer* NONNULL src,
                                               const void* NONNULL dst) noexcept;
        static std::pair<const Value*,slice> resolvePointerFromWithRange(
                                                                         const internal::Pointer* NONNULL src,
                                                                         const void* NONNULL dst) noexcept;
    protected:
        static const Scope* _containing(const Value* NONNULL) noexcept;
        void unregister() noexcept;

    private:
        Scope(const Scope&) =delete;

        using memoryMap = std::multimap<size_t, Scope*>;
        static memoryMap *sMemoryMap;
        
        Retained<SharedKeys> _sk;
        slice const         _externDestination;
        slice const         _data;
        bool                _registered {false};
        memoryMap::iterator _iter;
    protected:
        bool                _isDoc {false};
    };


    /** A container for Fleece data in memory. Every Value belongs to the Doc whose memory range
        contains it. The Doc keeps track of the SharedKeys used by its Dicts, and where to resolve
        external pointers to. */
    class Doc : public RefCounted, public Scope {
    public:
        enum Trust {
            kUntrusted, kTrusted
        };

        Doc(slice fleeceData,
            Trust =kUntrusted,
            SharedKeys* =nullptr,
            slice externDestination =nullslice) noexcept;
        Doc(alloc_slice fleeceData,
            Trust =kUntrusted,
            SharedKeys* =nullptr,
            slice externDest =nullslice) noexcept;

        static Retained<Doc> fromFleece(slice fleece, Trust =kUntrusted);
        static Retained<Doc> fromJSON(slice json);

        static RetainedConst<Doc> containing(const Value* NONNULL) noexcept;

        alloc_slice allocedData() const         {return _alloced;}

        const Value* root() const               {return _root;}
        const Dict* asDict() const              {return _root ? _root->asDict() : nullptr;}
        const Array* asArray() const            {return _root ? _root->asArray() : nullptr;}

    protected:
        virtual ~Doc() =default;

    private:
        const Value*        _root {nullptr};
        alloc_slice         _alloced;
    };

} }
