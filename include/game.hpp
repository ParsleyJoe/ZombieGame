#pragma once

#include <raylib.h>

#include <vector>
#include <functional>
#include <optional>
#include <algorithm>

#include "player.hpp"
#include "enemy.hpp"
#include "particlesystem.hpp"

class Map {
public:
	Map()
	{
		m_objects = {
			{ 400,  300,  500,   50 },
			{ 400,  300,   50,  400 },
			{ 1200, 500,  300,   50 },
			{ 1500, 500,   50,  400 }
		};
	}

	void draw() const
	{
		DrawRectangle(0, 0, 3000, 2000, DARKGREEN);

		for (const auto& obj : m_objects) {
			DrawRectangleRec(obj, GRAY);
		}
	}

	const std::vector<Rectangle>& getObjects() const {
		return m_objects;
	}

private:
	std::vector<Rectangle> m_objects;
};


class Game {
public:
	Game() // Need to pass in spawnBullet callback
		: m_player([this](Bullet blt) { spawnBullets(blt); })
		, m_camera(Camera2D{
				.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
				.target = m_player.getPosition(),
				.rotation = 0.0f,
				.zoom = 1.0f
			})
	{
	}

	void update() {
		float dt = GetFrameTime();

		Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_camera);
		m_player.update(dt, mouseWorldPos);
		m_enemy.update(dt);
		updateCamera(dt);
		for (auto& blt : m_bullets) {
			blt.update(dt);
		}
		m_particleSystem.update(dt);

		// Collisions
		const auto& mapObjects = m_map.getObjects(); // just a vec of Rectangles rn
		for (const auto& obj : mapObjects) {
			// Check for player
			Rectangle playerBox = m_player.getHitbox();
			if (CheckCollisionRecs(playerBox, obj)) {
				float overlapX = std::min(playerBox.x + playerBox.width, obj.x + obj.width) - std::max(playerBox.x, obj.x);
				float overlapY = std::min(playerBox.y + playerBox.height, obj.y + obj.height) - std::max(playerBox.y, obj.y);

				if (overlapX < overlapY) {
					// resolve horizontally
					if (playerBox.x < obj.x)
						m_player.push(-overlapX, 0);
					else
						m_player.push(overlapX, 0);
				}
				else {
					// resolve vertically
					if (playerBox.y < obj.y)
						m_player.push(0, -overlapY);
					else
						m_player.push(0, overlapY);
				}
			}

			// Bullets
			for (auto& blt : m_bullets) {
				if (CheckCollisionRecs(blt.getHitbox(), obj)) {
					blt.setActive(false);
				}

				if (CheckCollisionRecs(blt.getHitbox(), m_enemy.getHitbox())) {
					m_enemy.takeDamage(blt.getDamage());
					m_particleSystem.emit(m_enemy.getPosition(), 20);
				}
			}
		}

		// delete blts
		std::erase_if(m_bullets, [](const Bullet& blt) {
			return !blt.getActive();
		});
	}

	void draw() const {
		BeginMode2D(m_camera);

		m_map.draw();
		m_player.draw();
		m_enemy.draw();
		m_particleSystem.draw();

		for (auto& blt : m_bullets) {
			blt.draw();
		}
		EndMode2D();
	}

	void spawnBullets(Bullet& blt) {
		m_bullets.push_back(blt);
	}

	void updateCamera(float dt) {
		Vector2 playerPos = m_player.getPosition();

		Vector2 screenPlayerPos = GetWorldToScreen2D(playerPos, m_camera);

		const float marginX = 200.0f;
		const float marginY = 150.0f;

		float screenWidth = static_cast<float>(GetScreenWidth());
		float screenHeight = static_cast<float>(GetScreenHeight());

		Vector2 desiredTarget = m_camera.target;
		if (screenPlayerPos.x > screenWidth - marginX) {
			desiredTarget.x += screenPlayerPos.x - (screenWidth - marginX);

		}
		else if (screenPlayerPos.x < marginX) {
			desiredTarget.x -= marginX - screenPlayerPos.x;
		}

		if (screenPlayerPos.y < marginY) {
			desiredTarget.y -= marginY - screenPlayerPos.y;
		}
		else if (screenPlayerPos.y > screenHeight - marginY) {
			desiredTarget.y += screenPlayerPos.y - (screenHeight - marginY);
		}

		m_camera.target.x = Lerp(m_camera.target.x, desiredTarget.x, 8.0f * dt);
		m_camera.target.y = Lerp(m_camera.target.y, desiredTarget.y, 8.0f * dt);
	}
private:
	Player m_player;
	std::vector<Bullet> m_bullets{};
	Camera2D m_camera{};
	Map m_map{};
	Enemy m_enemy{};
	ParticleSystem m_particleSystem;
};