# VTable Crash Fix - Use After Free Bug

## Problem Analysis

The crash in `vtable for **cxxabiv1::**class_type_info` indicates a **use-after-free** bug where we're calling virtual functions on destroyed objects.

### Crash Timeline:
1. User closes current order
2. `closeCurrentOrder()` calls `posService_->setCurrentOrder(nullptr)`
3. `CURRENT_ORDER_CHANGED` event is published immediately
4. OrderEntryPanel receives event and tries to handle it
5. **Meanwhile**, POSModeContainer starts UI recreation and destroys widgets
6. OrderEntryPanel event handler executes on destroyed object
7. **CRASH** - Virtual function call on corrupted vtable

## Root Cause

**Event Race Condition**: The OrderEntryPanel was destroyed but its event handler was still in the event queue and executed after destruction.

```cpp
// OLD PROBLEMATIC FLOW:
closeCurrentOrder() {
    posService_->setCurrentOrder(nullptr);           // 1. Change state
    eventManager_->publish(CURRENT_ORDER_CHANGED);   // 2. Immediate event
    // 3. Event handler queued for OrderEntryPanel
    updateWorkArea();                                 // 4. Destroy OrderEntryPanel
    // 5. Event handler executes on destroyed object → CRASH
}
```

## Solution: Defensive Object Lifecycle Management

### 1. **Added Destruction Flag**
```cpp
class POSModeContainer {
    bool isDestroying_;  // Track destruction state
    
    ~POSModeContainer() {
        isDestroying_ = true;  // Prevent further operations
        // Clean up event subscriptions
        for (auto handle : eventSubscriptions_) {
            eventManager_->unsubscribe(handle);
        }
    }
};
```

### 2. **Destruction Safety Checks**
All methods now check destruction state:
```cpp
void handleCurrentOrderChanged() {
    if (isDestroying_) {
        return; // Don't process events during destruction
    }
    // ... handle event
}
```

### 3. **Immediate Pointer Clearing**
Clear component pointers before publishing events:
```cpp
void closeCurrentOrder() {
    // Clear service state
    posService_->setCurrentOrder(nullptr);
    
    // CRITICAL: Clear pointers immediately
    orderEntryPanel_ = nullptr;
    menuDisplay_ = nullptr;
    currentOrderDisplay_ = nullptr;
    
    // Defer event publication to avoid race
    WTimer::singleShot(100ms, [this]() {
        if (!isDestroying_) {
            eventManager_->publish(CURRENT_ORDER_CHANGED);
        }
    });
}
```

### 4. **Deferred Event Publication**
Instead of immediate event publication, use timers to ensure UI settles:
```cpp
// OLD: Immediate
eventManager_->publish(CURRENT_ORDER_CHANGED);

// NEW: Deferred
WTimer::singleShot(100ms, [this]() {
    if (!isDestroying_) {
        eventManager_->publish(CURRENT_ORDER_CHANGED);
    }
});
```

### 5. **Lambda Capture Safety**
All timer lambdas check destruction state:
```cpp
WTimer::singleShot(500ms, [this]() {
    if (isDestroying_) {
        return; // Don't execute if destroyed
    }
    updateWorkArea();
});
```

### 6. **Proper Event Cleanup**
Destructor properly unsubscribes from all events:
```cpp
~POSModeContainer() {
    isDestroying_ = true;
    
    // Unsubscribe from all events
    for (auto handle : eventSubscriptions_) {
        eventManager_->unsubscribe(handle);
    }
}
```

## Benefits

✅ **No More VTable Crashes** - Objects don't receive events after destruction  
✅ **Proper Lifecycle Management** - Destruction flag prevents operations during cleanup  
✅ **Event Safety** - Deferred event publication avoids race conditions  
✅ **Memory Safety** - Pointers cleared immediately before destruction  
✅ **Exception Safety** - All operations check validity before proceeding  

## Technical Details

### The VTable Issue
When a C++ object is destroyed, its vtable pointer becomes invalid. If a virtual function is called on the destroyed object, it tries to access the corrupted vtable, causing a crash in the ABI runtime.

### Event System Race Condition
Wt's event system is asynchronous - events published during one operation may not execute until later. This created a window where the OrderEntryPanel was destroyed but its event handler was still queued.

### Prevention Strategy
The multi-layered approach prevents the crash through:
1. **Prevention** - Destruction flag stops new operations
2. **Isolation** - Clear pointers before destruction
3. **Delay** - Defer events until after UI settles  
4. **Cleanup** - Proper event unsubscription
5. **Validation** - Check state in all timer callbacks

This should eliminate the vtable crash while maintaining proper order management functionality.