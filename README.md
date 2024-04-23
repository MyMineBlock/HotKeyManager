How to use this class:

```cpp
// In the WndProc function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_HOTKEY:
        {
            HotKeyManager::GetInstance().OnHotKeyPressed(static_cast<int>(wParam)); 
        }
    }
}

// Anywhere else
// Any hwnd will work, no need to make a window specifically for the hotkey class
HotKeyManager::GetInstance().AddHotKey(
    someHwnd, MOD_NOREPEAT, 'B', []() 
    { 
        std::cout << "sheesh\n"; 
    });
    
HotKeyManager::GetInstance().RemoveHotKey(someHwnd, MOD_NOREPEAT, 'B');
```
