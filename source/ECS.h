#pragma once

#include "gba.h"
#include "fixmath.h"

// TODO: Have both "rarely spawned" and "often spawned" versions of each component, make "rarely spawned" sorted and all that
//       and make "often spawned" not sorted and free lists.
//       "Often spawned" would be used for bullets etc, while "rarely spawned" would be used for stuff that is spawned on loading the scene
// TODO: Make entityId 0 be "this component was removed"
// TODO: Rename ComponentBase -> EntityId and use that everywhere instead

// ECS
// entities are uint32's
// one component max of each kind
// components need to be sorted on entityId
// (or maybe split in sorted and non sorted? sorted are for stuff like enemies etc. While non-sorted are for bullets and shit)
// systems are comprised of two functions, one void system() that is responsible for calling
//  void systemRun(CompType1 comp1, CompType2 comp2, ...) that does the actual updating
//
// Components are stored in one static array per type, also need a "top" uint32 (and if we go sorted/non-sorted a uint32 that say how many are sorted)

enum EntityFlag {
    ON_GROUND,
}; 

struct ComponentBase {
    uint32 entityId;
};

#define NUM_FLAGS 1

struct EntityFlags {
    uint32 flags[NUM_FLAGS];
    operator bool() {
        for (uint32 i = 0; i < NUM_FLAGS; ++i) {
            if (UNLIKELY(flags[i])) {
                return true;
            }
        }
        return false;
    }
};

static inline
bool isSet(EntityFlags *flags, EntityFlag f) {
    uint32 idx = 0;
    while (f >= 32) {
        idx++;
        f = (EntityFlag)(((uint32)f) - 32);
    }
    return flags->flags[idx] & (1 << f);
}

static inline
void set(EntityFlags *flags, EntityFlag f) {
    uint32 idx = 0;
    while (f >= 32) {
        idx++;
        f = (EntityFlag)(((uint32)f) - 32);
    }
    flags->flags[idx] |= (1 << f);
}

static inline
void clear(EntityFlags *flags, EntityFlag f) {
    uint32 idx = 0;
    while (f >= 32) {
        idx++;
        f = (EntityFlag)(((uint32)f) - 32);
    }
    flags->flags[idx] &= ~(1 << f);
}

struct FlagsComponent {
    ComponentBase base;
    EntityFlags flags;
    static const uint32 max = 20;
    static FlagsComponent components[FlagsComponent::max];
    static uint32 top;
};

uint32 FlagsComponent::top = 0;

static inline
EntityFlags emptyFlags() {
    EntityFlags ret;
    for (uint32 idx = 0; idx < NUM_FLAGS; idx++) {
        ret.flags[idx] = 0;
    }
    return ret;
}

static inline
EntityFlags flag(EntityFlag f) {
    EntityFlags ret;
    uint32 idx = 0;
    while (f >= 32) {
        ret.flags[idx] = 0;
        idx++;
        f = (EntityFlag)(((uint32)f) - 32);
    }
    ret.flags[idx] = 1 << f;
    idx++;
    for (; idx < NUM_FLAGS; idx++) {
        ret.flags[idx] = 0;
    }
    return ret;
}

static inline
EntityFlags operator|(EntityFlags a, EntityFlags b) {
    for (uint32 i = 0; i < NUM_FLAGS; i++) {
        a.flags[i] |= b.flags[i];
    }
    return a;
}

static inline
EntityFlags operator&(EntityFlags a, EntityFlags b) {
    for (uint32 i = 0; i < NUM_FLAGS; i++) {
        a.flags[i] &= b.flags[i];
    }
    return a;
}

static inline
EntityFlags operator|(EntityFlags a, EntityFlag f) {
    EntityFlags b = flag(f);
    for (uint32 i = 0; i < NUM_FLAGS; i++) {
        a.flags[i] |= b.flags[i];
    }
    return a;
}

static inline
EntityFlags operator&(EntityFlags a, EntityFlag f) {
    EntityFlags b = flag(f);
    for (uint32 i = 0; i < NUM_FLAGS; i++) {
        a.flags[i] &= b.flags[i];
    }
    return a;
}

static inline
EntityFlags operator|(EntityFlag f, EntityFlags a) {
    EntityFlags b = flag(f);
    for (uint32 i = 0; i < NUM_FLAGS; i++) {
        a.flags[i] |= b.flags[i];
    }
    return a;
}

static inline
EntityFlags operator&(EntityFlag f, EntityFlags a) {
    EntityFlags b = flag(f);
    for (uint32 i = 0; i < NUM_FLAGS; i++) {
        a.flags[i] &= b.flags[i];
    }
    return a;
}

static inline
EntityFlags operator~(EntityFlags flags) {
    for (uint32 i = 0; i < NUM_FLAGS; i++) {
        flags.flags[i] = ~flags.flags[i];
    }
    return flags;
}

struct SpriteComponent {
    ComponentBase base;
    volatile ObjectAttributes *sprite; 
    int8 relX, relY;
    uint8 sizeX, sizeY;
    static const uint32 max = 20;
    static SpriteComponent components[SpriteComponent::max];
    static uint32 top;
};

uint32 SpriteComponent::top = 0;

struct PositionComponent {
    ComponentBase base;
    V2fix24b8 p;
    V2fix24b8 dp;
    static const uint32 max = 20;
    static PositionComponent components[PositionComponent::max];
    static uint32 top;
};

uint32 PositionComponent::top = 0;

struct ControllerComponent {
    ComponentBase base;
    static const uint32 max = 1;
    static ControllerComponent components[ControllerComponent::max];
    static uint32 top;
};

uint32 ControllerComponent::top = 0;
ControllerComponent ControllerComponent::components[ControllerComponent::max];
SpriteComponent SpriteComponent::components[SpriteComponent::max];
PositionComponent PositionComponent::components[PositionComponent::max];
FlagsComponent FlagsComponent::components[FlagsComponent::max];

struct GravityComponent {
    ComponentBase base;
    Fix24b8 value;
    static const uint32 max = 50;
    static GravityComponent components[GravityComponent::max];
    static uint32 top;
};

uint32 GravityComponent::top = 0;
GravityComponent GravityComponent::components[GravityComponent::max];

struct FrictionComponent {
    ComponentBase base;
    V2fix24b8 value;
    static const uint32 max = 50;
    static FrictionComponent components[FrictionComponent::max];
    static uint32 top;
};

uint32 FrictionComponent::top = 0;
FrictionComponent FrictionComponent::components[FrictionComponent::max];

template<class T>
T *addComponent(uint32 entityId) {
    uint32 idx = T::top;
    T::top++;
    while (idx > 0 && T::components[idx - 1].base.entityId > entityId) {
        T::components[idx] = T::components[idx - 1];
        idx++;
    }
    T::components[idx].base.entityId = entityId;
    return T::components + idx;
}

template<>
FlagsComponent *addComponent<FlagsComponent>(uint32 entityId) {
    uint32 idx = FlagsComponent::top;
    FlagsComponent::top++;
    while (idx > 0 && FlagsComponent::components[idx - 1].base.entityId > entityId) {
        FlagsComponent::components[idx] = FlagsComponent::components[idx - 1];
        idx++;
    }
    FlagsComponent::components[idx].base.entityId = entityId;
    FlagsComponent::components[idx].flags = emptyFlags();
    return FlagsComponent::components + idx;
}

template<class C, class F>
void systemRun(F fun) {
    for (uint32 i = 0; i < C::top; ++i) {
        fun(C::components + i);
    }
}


template<class A, class B, class F>
void systemRun(F fun) {
    uint32 curr1 = 0;
    uint32 curr2 = 0;
    while (curr1 < A::top && curr2 < B::top) {
        uint32 e1 = A::components[curr1].base.entityId;
        uint32 e2 = B::components[curr2].base.entityId;
        if (UNLIKELY(e1 == e2)) {
            fun(A::components + curr1, B::components + curr2);
            curr1++;
            curr2++;
        } else if (e1 < e2) {
            curr1++;
        } else {
            curr2++;
        }
    }
}

// TODO: Rewrite if elseif chain so that we can put unlikely at match
template<class A, class B, class C, class F>
void systemRun(F fun) {
    uint32 curr1 = 0;
    uint32 curr2 = 0;
    uint32 curr3 = 0;
    while (curr1 < A::top && curr2 < B::top && curr3 < C::top) {
        uint32 e1 = A::components[curr1].base.entityId;
        uint32 e2 = B::components[curr2].base.entityId;
        uint32 e3 = C::components[curr3].base.entityId;
        if (e1 < e2 && e1 <= e3) {
            curr1++;
        } else if (e2 < e1 && e2 <= e3) {
            curr2++;
        } else if (e3 < e1 && e3 <= e2) {
            curr3++;
        } else {
            fun(A::components + curr1, B::components + curr2, C::components + curr3);
            curr1++;
            curr2++;
            curr3++;
        }                                                           
    }
}

template<class A, class F>
void systemRun(F fun, EntityFlags flags) {
    uint32 curr1 = 0;
    uint32 curr2 = 0;
    while (curr1 < A::top && curr2 < FlagsComponent::top) {
        if (LIKELY(!(FlagsComponent::components[curr2].flags & flags))) {
            curr2++;
            continue;
        }
        uint32 e1 = A::components[curr1].base.entityId;
        uint32 e2 = FlagsComponent::components[curr2].base.entityId;
        if (UNLIKELY(e1 == e2)) {
            fun(A::components + curr1, FlagsComponent::components + curr2);
            curr1++;
            curr2++;
        } else if (e1 < e2) {
            curr1++;
        } else {
            curr2++;
        }
    }
}

// TODO: Rewrite if elseif chain so that we can put unlikely at match
template<class A, class B, class F>
void systemRun(F fun, EntityFlags flags) {
    uint32 curr1 = 0;
    uint32 curr2 = 0;
    uint32 curr3 = 0;
    while (curr1 < A::top && curr2 < B::top && curr3 < FlagsComponent::top) {
        if (LIKELY(!(FlagsComponent::components[curr3].flags & flags))) {
            curr3++;
            continue;
        }
        uint32 e1 = A::components[curr1].base.entityId;
        uint32 e2 = B::components[curr2].base.entityId;
        uint32 e3 = FlagsComponent::components[curr3].base.entityId;
        if (e1 < e2 && e1 <= e3) {
            curr1++;
        } else if (e2 < e1 && e2 <= e3) {
            curr2++;
        } else if (e3 < e1 && e3 <= e2) {
            curr3++;
        } else {
            fun(A::components + curr1, B::components + curr2, FlagsComponent::components + curr3);
            curr1++;
            curr2++;
            curr3++;
        }
    }
}

uint32 entitiesTop = 1;

uint32 freeEntities[256];
uint16 freeEntitiesHead = 0;
uint16 freeEntitiesTail = 0;
uint32 killedEntities[256];
uint16 killedEntitiesHead = 0;

static inline
void killEntity(uint32 e) {
    killedEntities[killedEntitiesHead] = e;
    killedEntitiesHead = (killedEntitiesHead + 1) & 0xFF;
}

static inline
void pushDeadEntity(uint32 e) {
    freeEntities[freeEntitiesHead] = e;
    freeEntitiesHead = (freeEntitiesHead + 1) & 0xFF;
}

static inline
uint32 newEntity() {
    if (freeEntitiesHead == freeEntitiesTail) {
        return entitiesTop++;
    } else {
        uint32 ret = freeEntities[freeEntitiesTail];
        freeEntitiesTail = (freeEntitiesTail + 1) & 0xFF;
        return ret;
    }
}

template<class C>
void clearDeadComponent(uint32 i) {
    if (i + 1 < C::top) {
        C::components[i] = C::components[i+1];
    }
    C::top--;
}

template<class C>
void clearDeadComponents(uint32 e) {
    uint32 i = 0;
    for (; i < C::top; ++i) {
        if (C::components[i].base.entityId == e) {
            clearDeadComponent<C>(i);
            break;
        }
    }
    i++;
    for (; i < C::top+1; ++i) {
        C::components[i] = C::components[i+1];
    }
}
                       