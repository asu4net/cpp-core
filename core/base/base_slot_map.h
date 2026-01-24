
#pragma once

struct Slot_Handle {
    u32 index = U32.Max;
    u32 generation = 0;

    fn valid() const -> bool  { return index < U32.Max; }
};

template<typename T>
class Slot_Map {
public:
    inline static constexpr u64 DEFAULT_CAP = 30;

    struct iterator {
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::forward_iterator_tag;
        
        iterator(Slot_Map<T>* map_, size_t index_) 
            : map(map_)
            , index(index_) 
        { 
            skip_to_valid(); 
        }
        
        fn operator*() const -> reference { return *map->m_slots[index].ptr(); }
        fn operator->() const -> pointer { return map->m_slots[index].ptr(); }

        fn operator++() -> iterator& {
            ++index;
            skip_to_valid();
            return *this;
        }
        
        fn operator++(i32) -> iterator& {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        fn operator==(const iterator& other) -> bool const { return index == other.index && map == other.map; }
        fn operator!=(const iterator& other) -> bool const { return !(*this == other); }

        Slot_Map<T>* map;
        size_t index;

    private:
        fn skip_to_valid() -> void {
            while (index < map->m_slots.size() && !map->m_slots[index].occupied) {
                ++index;
            }
        }
    };

    Slot_Map(u64 start_cap = DEFAULT_CAP) {
        m_slots.reserve(start_cap);
        m_free_list.reserve(start_cap);
    }

    ~Slot_Map() {
        for (Slot<T>& slot : m_slots) {
            if (slot.occupied) {
                slot.ptr()->~T();
            }
        }
    }

    Slot_Map(const Slot_Map&) = delete;
    Slot_Map(Slot_Map&&) noexcept = delete;
    
    fn operator=(const Slot_Map&) -> Slot_Map& = delete;
    fn operator=(Slot_Map&&) noexcept -> Slot_Map& = delete;

    fn begin() -> iterator { return iterator(this, 0); }
    fn end() -> iterator { return iterator(this, m_slots.size()); }
    
    template<typename ...TArgs>
    fn insert(TArgs&& ...args) -> std::pair<Slot_Handle, T*> {
        u32 index;
        if (!m_free_list.empty()) {
            index = m_free_list.back();
            m_free_list.pop_back();
        } else {
            index = static_cast<u32>(m_slots.size());
            m_slots.emplace_back();
        }
        Slot<T>& slot = m_slots[index];
        T* data = slot.ptr();
        // @Note: This is called placemenet new. It calls the constructor in raw memory.
        new (data) T(std::forward<TArgs>(args)...);
        slot.occupied = true;
        ++slot.generation;
        return { Slot_Handle{ index, slot.generation }, data };
    }

    fn alive(Slot_Handle handle) const -> bool {
        return handle.valid() &&
               m_slots.size() &&
               m_slots[handle.index].occupied &&
               m_slots[handle.index].generation == handle.generation;
    }

    fn get(Slot_Handle handle) -> T* {
        if (!alive(handle)) return nullptr;
        return m_slots[handle.index].ptr();
    }
    
    fn erase(Slot_Handle handle) -> void {
        if (!alive(handle)) return;

        Slot<T>& slot = m_slots[handle.index];
        
        slot.ptr()->~T();     
        slot.occupied = false;
        m_free_list.push_back(handle.index);
    }

private:
    template<typename T>
    struct Slot  {
        alignas(T) std::byte storage[sizeof(T)] = {};
        u32 generation = 0u;
        bool occupied = false;
        
        fn ptr() -> T* { return std::launder(reinterpret_cast<T*>(storage)); }
    };

    std::vector<Slot<T>> m_slots;
    std::vector<u32> m_free_list;
};
