## How to use the simple class:

```cpp
// In the WndProc function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_HOTKEY:
        {
            HotKeyManager::GetInstance().OnHotKeyPressed(static_cast<int>(wParam)); 
        }
    }
}

// Anywhere else
// Any hwnd will work, no need to make a window specifically for the hotkey class
HotKeyManager::GetInstance().AddHotKey(
    someHwnd, MOD_NOREPEAT, 'B', []() { 
        std::cout << "sheesh\n"; 
    });
    
HotKeyManager::GetInstance().RemoveHotKey(someHwnd, MOD_NOREPEAT, 'B');
```

## How to use the complex class:

```cpp
// Same concept here
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_HOTKEY:
        {
            HotKeyManager::GetInstance().OnHotKeyPressed(static_cast<int>(wParam)); 
        }
    }
}

// More complicated elsewhere
// Resgister the hotkey
// Note that all behavior other than hotkey id can be user defined
typedef void (*CallBack)(int,int);
std::unordered_map<int, CallBack> funcmap{};
void SomeClassAddHotKey(HWND hwnd, int modifiers, int key, CallBack callback) {
    // This could go wrong since it is an optional
    if(const auto id = HotKeyManager::GetInstance().AddHotKey(hwnd, MOD_NOREPEAT, 'B')) {
        // You will need to save the id and associate it with a function or the parameters to a single function
        funcmap.emplace(id.value(), callback);
    }
}

// All that needs to be done is to detect the event in the main loop
struct some_class
{
    void go(int id) {
        std::jthread th(funcmap.at(id), param1, param2);
        th.detach();
    }
}

while (true) {
    if (const auto id = HotKeyManager::GetInstance().ReadQueue()) {
	    some_class.go(id.value());
    }
}
```
