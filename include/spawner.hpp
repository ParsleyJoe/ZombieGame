#pragma once
#include <vector>
#include <functional>

class Enemy;

class Spawner {
public:
	Spawner(std::function<void(int)> playerDmgCallback)
		: m_playerDmgCallback(playerDmgCallback)
	{
	}
	void update(float dt, std::vector<Enemy>& enemies);

private:
	Enemy spawnEnemies();

	int m_wave{};
	int m_enemyPerWave{};
	float m_cooldown = 2.0f;
	float m_timer{};

	std::function<void(int)> m_playerDmgCallback;
};