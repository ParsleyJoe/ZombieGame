#pragma once

#include <raylib.h>
#include <raymath.h>

#include <optional>
#include <functional>

class Bullet {
public:
	Bullet() = delete;
	Bullet(float damage, Vector2 velocity, Vector2 pos)
		: m_damage(damage), m_velocity(velocity), m_pos(pos)
	{
	}

	void update(float dt) {
		if (!m_active) { return; }
		m_pos.x += m_velocity.x * dt;
		m_pos.y += m_velocity.y * dt;
	}

	void draw() const {
		if (!m_active) { return; }
		DrawCircle(m_pos.x, m_pos.y, m_radius, m_color);
	}

	Rectangle getHitbox() const {
		return Rectangle{ m_pos.x - m_radius, m_pos.y - m_radius, m_radius * 2.0f, m_radius * 2.0f };
	}

	bool getActive() const {
		return m_active;
	}
	void setActive(bool active) {
		m_active = active;
	}
	int getDamage() {
		return m_damage;
	}

private:
	float m_damage;
	Vector2 m_velocity;
	Vector2 m_pos;
	int m_radius = 10;
	Color m_color = RED;
	bool m_active = true;
};

/// <summary>
///  GUN CODE
/// </summary>
struct GunStats {
	float damage;
	float spread;
	float fireRate;
	float bulletSpeed;
	int bulletCount;
};

class Gun {
public:
	Gun() = default;

	explicit Gun(GunStats stats)
		: m_stats(stats)
	{
	}

	std::optional<Bullet> shoot(float dt, Vector2 dir, Vector2 pos) {
		m_fireTimer -= dt;

		// return NOT a bullet
		if (m_fireTimer > 0.0f)
			return std::nullopt;

		m_fireTimer = 1.0f / m_stats.fireRate;
		Bullet blt{ m_stats.damage, dir * m_stats.bulletSpeed, pos };
		return blt;
	}

private:
	GunStats m_stats{ 4, 2, 3, 700, 20 };
	float m_fireTimer = 0.0f;
};

/// <summary>
/// PLAYER CODE
/// </summary>
class Player
{
public:
	Player(std::function<void(Bullet)> spawnBullet)
		: m_spawnBullets(spawnBullet)
	{
	}
	void update(float dt, Vector2 mouseWorldPos)
	{
		this->move(dt);

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 dir = Vector2Normalize(mouseWorldPos - m_pos);
			auto blt = m_gun.shoot(dt, dir, m_pos);
			if (blt)
				m_spawnBullets(*blt);
		}
	}

	// might want to add stuff here
	void push(float dx, float dy) {
		m_pos.x += dx;
		m_pos.y += dy;
	}

	void draw() const {
		DrawRectangle(m_pos.x, m_pos.y, m_width, m_height, m_color);
	}

	Vector2 getPosition() const {
		return m_pos;
	}
	Rectangle getHitbox() const {
		return m_hitbox;
	}
private:
	void move(float dt)
	{
		if (IsKeyDown(KEY_A)) {
			m_pos.x -= m_speed * dt;
		}
		if (IsKeyDown(KEY_D)) {
			m_pos.x += m_speed * dt;
		}
		if (IsKeyDown(KEY_W)) {
			m_pos.y -= m_speed * dt;
		}
		if (IsKeyDown(KEY_S)) {
			m_pos.y += m_speed * dt;
		}

		m_hitbox.x = m_pos.x;
		m_hitbox.y = m_pos.y;
	}


	Vector2 m_pos{ 400, 300 };
	Rectangle m_hitbox{ 400, 300, 50, 50 };
	float m_speed = 300.0;
	int m_width = 50, m_height = 50;
	Color m_color = RAYWHITE;
	Gun m_gun;
	std::function<void(Bullet)> m_spawnBullets;
};
