// ***********************************************************************
// <author>Stephan Burguchev</author>
// <copyright company="Stephan Burguchev">
//   Copyright (c) Stephan Burguchev 2012-2015. All rights reserved.
// </copyright>
// <summary>
//   SpinLock.h
// </summary>
// ***********************************************************************
#pragma once

#include <atomic>
#include <thread>

namespace Common
{
    class SpinLock {
        std::atomic_flag locked;
    public:
        void lock() {
            while (locked.test_and_set(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
        }
        void unlock() {
            locked.clear(std::memory_order_release);
        }
    };
}
