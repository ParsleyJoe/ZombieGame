#include "enemy.hpp"

void Enemy::update(float dt) {
	if (!m_active) { return; }
}

void Enemy::draw() const {
	if (!m_active) { return; }
	DrawRectangle(m_pos.x, m_pos.y, m_width, m_height, ORANGE);
}

void Enemy::takeDamage(int damage) {
	m_health -= damage;

	if (m_health <= 0) {
		//m_active = false;
	}
}