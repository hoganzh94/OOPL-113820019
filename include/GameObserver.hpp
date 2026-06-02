#ifndef GAMEOBSERVER_HPP
#define GAMEOBSERVER_HPP

// 定義會發生的遊戲事件
enum class GameEvent {
    LEVEL_WIN,
    LEVEL_FAIL
};

// 觀察者介面
class IGameObserver {
public:
    virtual ~IGameObserver() = default;
    // 收到通知時會觸發這個函數
    virtual void OnNotify(GameEvent event) = 0;
};

#endif