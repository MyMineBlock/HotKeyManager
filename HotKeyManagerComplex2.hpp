#pragma once
#include <Windows.h>
#include <vector>
#include <queue>
#include <utility>

class HotKeyManager
{
public:
    static inline HotKeyManager& GetInstance() noexcept
    {
        static HotKeyManager s_instance;
        return s_instance;
    }

    [[nodiscard]] inline std::optional<int> AddHotKey(const HWND hwnd, const int modifiers, const int key) noexcept
    {
        if (RegisterHotKey(hwnd, m_NextId, modifiers, key)) 
        {
            m_HotKeys.emplace(m_NextId, m_HotKey{modifiers, key});
            return std::optional<int>{ m_NextId++ };
        }
        return std::optional<int>{};
    }

    [[maybe_unused]] inline bool RemoveHotKey(const HWND hwnd, const int id) noexcept
    {
        if (m_HotKeys.contains(id))
        {
            if (UnregisterHotKey(hwnd, id))
            {
                m_HotKeys.erase(id);
                return true;
            }
            return false;
        }
        return false;
    }

    inline void OnHotKeyPressed(const int id) noexcept
    {
        m_EventQueue.push(id);
    }

    [[nodiscard]] inline std::optional<int> ReadQueue() noexcept
    {
        if (!m_EventQueue.empty())
        {
            const auto& tmp{ m_EventQueue.front() };
            m_EventQueue.pop();
            return std::optional<int>{ tmp };
        }
        return std::optional<int>{};
    }

    [[maybe_unused]] inline bool RemoveHotKeyWithoutId(const HWND hwnd, const int modifiers, const int key) noexcept
    {
        auto it{ std::find_if(m_HotKeys.begin(), m_HotKeys.end(),
            [&](const auto& hotkey) -> bool
            {
                return hotkey.second.modifiers == modifiers && hotkey.second.key == key;
            })
        };
        if (it != m_HotKeys.end())
        {
            if (UnregisterHotKey(hwnd, it->first))
            {
                m_HotKeys.erase(it->first);
                return true;
            }
            return false;
        }
        return false;
    }

    [[nodiscard]] inline int GetHotKeyId(const int modifiers, const int key) noexcept
    {
        auto it{ std::find_if(m_HotKeys.begin(), m_HotKeys.end(),
            [&](const auto& hotkey) -> bool
            {
                return hotkey.second.modifiers == modifiers && hotkey.second.key == key;
            })
        };
        if (it != m_HotKeys.end())
        {
            return it->first;
        }
    }

    [[nodiscard]] inline std::optional<std::pair<int, int>> GetHotKeyById(const int id)
    {
        if (m_HotKeys.contains(id))
        {
            return std::optional<std::pair<int, int>>{{m_HotKeys.at(id).modifiers, m_HotKeys.at(id).key}};
        }
        return std::optional<std::pair<int, int>>{};
    }
private:
    struct m_HotKey
    {
        int modifiers;
        int key;
    };

    std::unordered_map<int, m_HotKey> m_HotKeys{};
    std::queue<int> m_EventQueue{};
    int m_NextId{ 1 };
private:
    HotKeyManager() = default;
    ~HotKeyManager() = default;

    HotKeyManager(const HotKeyManager&) = delete;
    HotKeyManager& operator=(const HotKeyManager&) = delete;
    HotKeyManager(HotKeyManager&&) = delete;
    HotKeyManager& operator=(HotKeyManager&&) = delete;
};
