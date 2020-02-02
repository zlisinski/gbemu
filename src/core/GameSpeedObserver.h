#pragma once

#include <algorithm>
#include <vector>

#include "gbemu.h"

class GameSpeedSubject;


class GameSpeedObserver
{
public:
    GameSpeedObserver(GameSpeedSubject *gameSpeedSubject) :
        gameSpeedSubject(gameSpeedSubject) {}
    GameSpeedObserver(const GameSpeedObserver&) = delete;
    void operator=(const GameSpeedObserver&) = delete;

    virtual void UpdateGameSpeed(int value) = 0;

protected:
    ~GameSpeedObserver() {}
    GameSpeedSubject *gameSpeedSubject;
};


class GameSpeedSubject
{
public:
    GameSpeedSubject() {}
    GameSpeedSubject(const GameSpeedSubject&) = delete;
    void operator=(const GameSpeedSubject&) = delete;

    void AttachGameSpeedObserver(GameSpeedObserver *observer)
    {
        gameSpeedObservers.push_back(observer);
    }

    void DetachObserver(GameSpeedObserver *observer)
    {
        auto it = std::find(gameSpeedObservers.cbegin(), gameSpeedObservers.cend(), observer);
        if (it != gameSpeedObservers.cend())
            gameSpeedObservers.erase(it);
    }

    void NotifyGameSpeedObservers(int value)
    {
        for (GameSpeedObserver *observer : gameSpeedObservers)
            observer->UpdateGameSpeed(value);
    }

protected:
    ~GameSpeedSubject() {}

private:
    std::vector<GameSpeedObserver*> gameSpeedObservers;
};