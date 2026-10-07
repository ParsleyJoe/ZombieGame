#include "spawner.hpp"
#include "enemy.hpp"

void Spawner::update(float dt, std::vector<Enemy>& enemies) {
	if (m_timer < 0.0f) {
		m_timer = m_cooldown;

		Enemy enemy = spawnEnemies();
		enemies.emplace_back(enemy);
	}
	m_timer -= dt;
}

Enemy Spawner::spawnEnemies() {
	Enemy enemy{ m_playerDmgCallback };
	return enemy;
}