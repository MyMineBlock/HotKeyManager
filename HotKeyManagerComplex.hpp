#pragma once
#include <Windows.h>
#include <vector>
#include <queue>
#include <utility>

class HotKeyManager
{
public:
    HotKeyManager(const HotKeyManager&) = delete;
    HotKeyManager& operator=(const HotKeyManager&) = delete;
    HotKeyManager(HotKeyManager&&) = delete;
    HotKeyManager& operator=(HotKeyManager&&) = delete;

    static inline HotKeyManager& GetInstance() noexcept
    {
        static HotKeyManager s_instance;
        return s_instance;
    }

    [[nodiscard]] inline std::optional<int> AddHotKey(const HWND hwnd, const int modifiers, const int key) noexcept
    {
        if (RegisterHotKey(hwnd, m_NextId, modifiers, key)) 
        {
            m_HotKeys.emplace_back(m_HotKey{ m_NextId, modifiers, key });
            return std::optional<int> {m_NextId++};
        }
        return std::optional<int>{};
    }

    inline void RemoveHotKey(const HWND hwnd, const int modifiers, const int key) noexcept
    {
        auto it = std::find_if(m_HotKeys.begin(), m_HotKeys.end(), [&](const auto& mem)
        {
            return mem.key == key && mem.modifiers == modifiers;
        });

        if (it != m_HotKeys.end())
        {
            if (UnregisterHotKey(hwnd, it->id))
            {
                m_HotKeys.erase(it);
            }
        }
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
private:
    HotKeyManager() = default;
    ~HotKeyManager() = default;

    struct m_HotKey
    {
        int id;
        int modifiers;
        int key;
    };

    std::vector<m_HotKey> m_HotKeys{};
    std::queue<int> m_EventQueue{};
    int m_NextId{ 1 };
};
