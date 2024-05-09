#pragma once
#include <Windows.h>
#include <thread>
#include <unordered_map>
#include <functional>

class HotKeyManager
{
public:
    HotKeyManager(const HotKeyManager&) = delete;
    HotKeyManager& operator=(const HotKeyManager&) = delete;
    HotKeyManager(HotKeyManager&&) = delete;
    HotKeyManager& operator=(HotKeyManager&&) = delete;

    inline static HotKeyManager& GetInstance() noexcept
    {
        static HotKeyManager s_instance;
        return s_instance;
    }

    inline void AddHotKey(const HWND hwnd, const int modifiers, const int key, const std::function<void()> function) noexcept
    {
        if (RegisterHotKey(hwnd, m_NextId, modifiers, key)) 
        {
            m_HotKeyMap.emplace(m_HotKey{ m_NextId, modifiers, key }, function);
            ++m_NextId;
        }
    }

    inline void RemoveHotKey(const HWND hwnd, const int modifiers, const int key) noexcept
    {
        auto it = std::find_if(m_HotKeyMap.begin(), m_HotKeyMap.end(), [&](const auto& pair) 
        {
            return pair.first.key == key && pair.first.modifiers == modifiers;
        });

        if (it != m_HotKeyMap.end()) 
        {
            if (UnregisterHotKey(hwnd, it->first.id))
            {
                m_HotKeyMap.erase(it);
            }
        }
    }

    inline void OnHotKeyPressed(const int id) noexcept
    {
        m_HotKey key{ id };
        if (m_HotKeyMap.contains(key))
        {
            std::jthread th(m_HotKeyMap.at(key));
            th.detach();
        }
    }

private:
    HotKeyManager() = default;
    ~HotKeyManager() = default;

    struct m_HotKey
    {
        int id;
        int modifiers;
        int key;

        bool operator==(const m_HotKey& other) const noexcept
        {
            return (id == other.id);
        }
    };

    struct HotKeyHash
    {
        std::size_t operator()(const m_HotKey& k) const noexcept
        {
            return std::hash<int>()(k.id);
        }
    };

    std::unordered_map<m_HotKey, std::function<void()>, HotKeyHash> m_HotKeyMap;
    int m_NextId{ 1 };
};
