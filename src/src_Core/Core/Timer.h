#pragma once
#include <GLFW/glfw3.h>
#include <cstdint>

class Timer {
    public:
        Timer() {}
        ~Timer() {}

        // various clock actions
        void start() {
            m_started = true;
            m_paused = false;
            m_startTicks = glfwGetTime();
            m_pausedTicks = 0;
        }
        void stop() {
            m_started = false;
            m_paused = true;
            m_startTicks = 0;
            m_pausedTicks = 0;
        }
        void pause() {
            if (m_started && !m_paused) {
                m_paused = true;

                m_pausedTicks = glfwGetTime() - m_startTicks;
                m_startTicks = 0;
            }
        }
        void unpause() {
            if (m_started && m_paused) {
                m_paused = false;
                m_startTicks = glfwGetTime() - m_pausedTicks;
                m_pausedTicks = 0;
            }
        }

        // Getter
        double getTicks() {
            if (m_started) {
                if (m_paused) {
                    return m_pausedTicks;
                }
                return (glfwGetTime() - m_startTicks); 
            }
            return 0;
        }
        
        // Status checkers
        bool isStarted() {
            return m_started;
        }
        bool isPaused() {
            return m_paused && m_started;
        }

    private:
        // Time start
        double m_startTicks = 0;

        // Time paused
        double m_pausedTicks = 0;

        // Status flags
        bool m_paused = false;
        bool m_started = false; 
};