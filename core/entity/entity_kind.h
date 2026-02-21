#pragma once

#define for_entity_kinds(_do) \
    _do(Entity_Base)

// Kind enum.
enum Entity_Kind : u64 {
    Entity_Kind_None = 0,
    #define declare_enum_entry(_kind) \
        Entity_Kind_##_kind,

        for_entity_kinds(declare_enum_entry)
    
    #undef declare_enum_entry
    
    Entity_Kind_Count
};

inline fn to_string(Entity_Kind kind) -> const char* {
    switch (kind)
    {
        #define declare_switch_case(_kind) \
            case Entity_Kind_##_kind: return stringify(Entity_Kind_##_kind);

            for_entity_kinds(declare_switch_case)
        #undef declare_switch_case
        
        default: return "";
    }
}

inline fn from_string(std::string_view str, Entity_Kind* pKind) {
    #define declare_if_case(_kind)                   \
        if (str == stringify(Entity_Kind_##_kind)) { \
           *pKind = Entity_Kind_##_kind;             \
            return;                                  \
        }

        for_entity_kinds(declare_if_case)
        #undef declare_if_case
}

// Forward declarations.
#define forward_declare(_kind) \
    struct _kind;
    
    for_entity_kinds(forward_declare)

#undef forward_declare