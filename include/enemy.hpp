#pragma once
#include <raylib.h>

#include <functional>

class Enemy {
public:
	Enemy() = delete;
	Enemy(std::function<void(int)> playerDmgCallbck)
		: m_playerDmgCallback(playerDmgCallbck)
	{
	}
	void update(float dt, Vector2 playerPos);

	void draw() const;

	void takeDamage(int damage);

	bool getActive() const { return m_active; }
	Vector2 getPosition() const { return m_pos; }
	Rectangle getHitbox() const { return Rectangle{ m_pos.x, m_pos.y, m_width, m_height}; }
private:
	Vector2 m_pos{ 200, 200 };
	float m_width = 30, m_height = 30;
	float m_speed = 100;
	bool m_active = true;
	int m_health = 10;

	int m_attackRange = 20.0f;
	float m_attackCooldown = 0.7f;
	float m_attackTimer = 0.0f;
	int m_attackDamage = 2;
	std::function<void(int)> m_playerDmgCallback;
};