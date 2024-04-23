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

    static HotKeyManager& GetInstance()
    {
        static HotKeyManager s_instance;
        return s_instance;
    }

    void AddHotKey(HWND hwnd, int modifiers, int key, std::function<void()> function) noexcept
    {
        if (RegisterHotKey(hwnd, m_NextId, modifiers, key)) 
        {
            HotKeyMap.emplace(m_HotKey{ m_NextId, modifiers, key }, function);
            ++m_NextId;
        }
    }

    void RemoveHotKey(HWND hwnd, int modifiers, int key) noexcept
    {
        auto it = std::find_if(HotKeyMap.begin(), HotKeyMap.end(), [&](const auto& pair) 
        {
            return pair.first.key == key && pair.first.modifiers == modifiers;
        });

        if (it != HotKeyMap.end()) 
        {
            if (UnregisterHotKey(hwnd, it->first.id))
            {
                HotKeyMap.erase(it);
            }
        }
    }

    void OnHotKeyPressed(int id) noexcept
    {
        m_HotKey key = { id };
        auto it = HotKeyMap.find(key);
        if (it != HotKeyMap.end())
        {
            std::jthread th(it->second);
            th.detach();
        }
    }

private:
    HotKeyManager() = default;

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

    std::unordered_map<m_HotKey, std::function<void()>, HotKeyHash> HotKeyMap;
    int m_NextId{ 1 };
};