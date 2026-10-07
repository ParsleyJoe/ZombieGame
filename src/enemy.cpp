#include "enemy.hpp"

#include <raymath.h>
#include <print>

void Enemy::update(float dt, Vector2 playerPos) {
	if (!m_active) { return; }

	Vector2 direction = playerPos - m_pos;
	float dist = Vector2Length(direction);
	if (dist > m_attackRange) {
		direction = Vector2Normalize(direction);

		m_pos.x += direction.x * m_speed * dt;
		m_pos.y += direction.y * m_speed * dt;
	} else {
		m_attackTimer -= dt;

		if (m_attackTimer <= 0.0f) {
			// attack player
			std::println("ATTACK!!!!");
			m_playerDmgCallback(m_attackDamage);
			m_attackTimer = m_attackCooldown;
		}
	}
}

void Enemy::draw() const {
	if (!m_active) { return; }
	DrawRectangle(m_pos.x, m_pos.y, m_width, m_height, ORANGE);
}

void Enemy::takeDamage(int damage) {
	m_health -= damage;

	if (m_health <= 0) {
		m_active = false;
	}
}