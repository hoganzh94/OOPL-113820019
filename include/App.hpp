#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Util/Renderer.hpp"
#include "SceneManager.hpp"

class App {
public:
    enum class State { START, UPDATE, END };
    [[nodiscard]] State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    static void End();

private:
    State m_CurrentState = State::START;
    Util::Renderer m_Root;

    std::unique_ptr<SceneManager> m_SceneManager;
    int m_LevelCounter = 1; // 追蹤目前應該進入哪一關
};

#endif