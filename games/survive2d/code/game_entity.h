#pragma once

#include "entity.h"

struct Entity: Entity_Base {
};

fn _Serialize_Entity(const Entity_Base*, std::string*) -> void;
fn _Deserialize_Entity(std::string_view, Entity_Base*) -> void;