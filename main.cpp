#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>

// ================ MERMI ================

class Bullet
{
public:

    sf::Sprite sprite;
    sf::Vector2f velocity;

    Bullet(
        const sf::Texture& texture,
        sf::Vector2f position,
        sf::Vector2f direction
    )
        : sprite(texture)
    {
        sprite.setScale({0.15f, 0.15f});

        sprite.setOrigin({
            texture.getSize().x / 2.f,
            texture.getSize().y / 2.f
        });

        sprite.setPosition(position);

        float angle =
            std::atan2(direction.y, direction.x)
            * 180.f / 3.14159f;

        sprite.setRotation(sf::degrees(angle));

        velocity = direction * 10.f;
    }

    void update()
    {
        sprite.move(velocity);
    }
};

// ================ DUSMAN ================

class Enemy
{
public:

    sf::Sprite sprite;
    float speed;

    Enemy(
        const sf::Texture& texture,
        sf::Vector2f position
    )
        : sprite(texture)
    {
        sprite.setScale({0.15f, 0.15f});

        sprite.setOrigin({
            texture.getSize().x / 2.f,
            texture.getSize().y / 2.f
        });

        sprite.setPosition(position);

        speed = 2.f;
    }

    void update(sf::Vector2f playerPosition)
    {
        sf::Vector2f direction =
            playerPosition - sprite.getPosition();

        float length =
            std::sqrt(
                direction.x * direction.x +
                direction.y * direction.y
            );

        if (length != 0.f)
        {
            direction /= length;

            sprite.move(direction * speed);

            float angle =
                std::atan2(direction.y, direction.x)
                * 180.f / 3.14159f;

            sprite.setRotation(sf::degrees(angle));
        }
    }
};

// ================ MAIN ================

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // ================ PENCERE ================

    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Top Down Shooter"
    );

    window.setFramerateLimit(60);

    // ================ TEXTURE ================

    sf::Texture playerTexture;
    sf::Texture enemyTexture;
    sf::Texture bulletTexture;
    sf::Texture backgroundTexture;

    if (!playerTexture.loadFromFile("assets/oyuncu.png"))
        return -1;

    if (!enemyTexture.loadFromFile("assets/dusman.png"))
        return -1;

    if (!bulletTexture.loadFromFile("assets/mermi.png"))
        return -1;

    if (!backgroundTexture.loadFromFile("assets/arkaplan.png"))
        return -1;

    // ================= FONT =================

    sf::Font font("assets/font.ttf");

    // ================= ARKAPLAN =================

    sf::Sprite background(backgroundTexture);

    background.setScale({
        800.f / backgroundTexture.getSize().x,
        600.f / backgroundTexture.getSize().y
    });

    // ================= OYUNCU =================

    sf::Sprite player(playerTexture);

    player.setScale({0.18f, 0.18f});

    player.setOrigin({
        playerTexture.getSize().x / 2.f,
        playerTexture.getSize().y / 2.f
    });

    player.setPosition({400.f, 300.f});

    float playerSpeed = 4.f;

    // ================= CAN =================

    int health = 100;

    sf::RectangleShape healthBarBg({200.f, 20.f});
    healthBarBg.setFillColor(sf::Color(60, 60, 60));
    healthBarBg.setPosition({20.f, 20.f});

    sf::RectangleShape healthBar({200.f, 20.f});
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setPosition({20.f, 20.f});

    // ================= SKOR =================

    int score = 0;

    sf::Text scoreText(font);

    scoreText.setCharacterSize(24);

    scoreText.setFillColor(sf::Color::White);

    scoreText.setPosition({20.f, 50.f});

    // ================= GAME OVER =================

    bool gameOver = false;

    sf::Text gameOverText(font);

    gameOverText.setString("GAME OVER");

    gameOverText.setCharacterSize(50);

    gameOverText.setFillColor(sf::Color::Red);

    gameOverText.setPosition({220.f, 250.f});

    // ================= VECTORLER =================

    std::vector<Bullet> bullets;

    std::vector<Enemy> enemies;

    // ================= CLOCK =================

    sf::Clock shootClock;

    sf::Clock spawnClock;

    float shootCooldown = 0.2f;

    // ================= OYUN =================

    while (window.isOpen())
    {
        // ================= EVENT =================

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        if (!gameOver)
        {
            // ================= HAREKET =================

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
                player.move({0.f, -playerSpeed});

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                player.move({0.f, playerSpeed});

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
                player.move({-playerSpeed, 0.f});

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
                player.move({playerSpeed, 0.f});

            // ================= MOUSE =================

            sf::Vector2i mousePos =
                sf::Mouse::getPosition(window);

            sf::Vector2f direction =
                sf::Vector2f(
                    static_cast<float>(mousePos.x),
                    static_cast<float>(mousePos.y)
                ) - player.getPosition();

            float angle =
                std::atan2(direction.y, direction.x)
                * 180.f / 3.14159f;

            player.setRotation(sf::degrees(angle));

            // ================= ATES =================

            if (
                sf::Mouse::isButtonPressed(
                    sf::Mouse::Button::Left
                )
                &&
                shootClock.getElapsedTime()
                .asSeconds() >= shootCooldown
            )
            {
                float length =
                    std::sqrt(
                        direction.x * direction.x +
                        direction.y * direction.y
                    );

                if (length != 0.f)
                {
                    direction /= length;

                    bullets.push_back(
                        Bullet(
                            bulletTexture,
                            player.getPosition(),
                            direction
                        )
                    );

                    shootClock.restart();
                }
            }

            // ================= DUSMAN DOGUR =================

            if (
                spawnClock.getElapsedTime()
                .asSeconds() >= 2.f
            )
            {
                sf::Vector2f enemyPos;

                int side = std::rand() % 4;

                if (side == 0)
                {
                    enemyPos = {
                        static_cast<float>(std::rand() % 800),
                        -50.f
                    };
                }

                else if (side == 1)
                {
                    enemyPos = {
                        static_cast<float>(std::rand() % 800),
                        650.f
                    };
                }

                else if (side == 2)
                {
                    enemyPos = {
                        -50.f,
                        static_cast<float>(std::rand() % 600)
                    };
                }

                else
                {
                    enemyPos = {
                        850.f,
                        static_cast<float>(std::rand() % 600)
                    };
                }

                enemies.push_back(
                    Enemy(enemyTexture, enemyPos)
                );

                spawnClock.restart();
            }

            // ================= MERMI UPDATE =================

            for (size_t i = 0; i < bullets.size(); i++)
            {
                bullets[i].update();

                sf::Vector2f pos =
                    bullets[i].sprite.getPosition();

                if (
                    pos.x < 0 ||
                    pos.x > 800 ||
                    pos.y < 0 ||
                    pos.y > 600
                )
                {
                    bullets.erase(
                        bullets.begin() + i
                    );

                    i--;
                }
            }

            // ================= DUSMAN UPDATE =================

            for (auto& enemy : enemies)
            {
                enemy.update(player.getPosition());
            }

            // ================= MERMI CARPISMA =================

            for (size_t e = 0; e < enemies.size(); e++)
            {
                for (size_t b = 0; b < bullets.size(); b++)
                {
                    if (
                        enemies[e]
                        .sprite
                        .getGlobalBounds()
                        .findIntersection(
                            bullets[b]
                            .sprite
                            .getGlobalBounds()
                        )
                    )
                    {
                        enemies.erase(
                            enemies.begin() + e
                        );

                        bullets.erase(
                            bullets.begin() + b
                        );

                        score += 10;

                        e--;

                        break;
                    }
                }
            }

            // ================= OYUNCU HASAR =================

            for (size_t i = 0; i < enemies.size(); i++)
            {
                if (
                    enemies[i]
                    .sprite
                    .getGlobalBounds()
                    .findIntersection(
                        player.getGlobalBounds()
                    )
                )
                {
                    enemies.erase(
                        enemies.begin() + i
                    );

                    health -= 10;

                    healthBar.setSize({
                        health * 2.f,
                        20.f
                    });

                    i--;

                    if (health <= 0)
                    {
                        gameOver = true;
                    }
                }
            }

            // ================= SKOR TEXT =================

            scoreText.setString(
                "Skor: " + std::to_string(score)
            );
        }

        // ================= CIZIM =================

        window.clear();

        window.draw(background);

        for (auto& bullet : bullets)
            window.draw(bullet.sprite);

        for (auto& enemy : enemies)
            window.draw(enemy.sprite);

        window.draw(player);

        window.draw(healthBarBg);

        window.draw(healthBar);

        window.draw(scoreText);

        if (gameOver)
            window.draw(gameOverText);

        window.display();
    }

    return 0;
}