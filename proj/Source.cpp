#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include <iostream>

// #define isn't working for some reason

constexpr float WINDOW_WIDTH=1920.0f;
constexpr float WINDOW_HEIGHT = 1080.0f;
constexpr float MOVEMENT_SPEED = 250.0f;
constexpr float GRAVITY = 800.0f;
constexpr float JUMP_FORCE = 600.0f;
constexpr float ENTITY_SIZE = 50.0f;
constexpr int FPS = 144;
class MovableEntity {
public:
    MovableEntity(float startX, float startY) :
        position(startX, startY),
        velocity(0.0f, 0.0f),
        isJumping(false),
        jumpForce(JUMP_FORCE),
        gravity(GRAVITY),
        movementSpeed(MOVEMENT_SPEED),
        isAlive(true),
        currentDirection(Direction::Right)
    {
        // Set up any other properties or resources for the movable entity here
    }

    virtual void update(float deltaTime, const std::vector<std::shared_ptr<MovableEntity>>& entities) {
        if (!isAlive) {
            // ...
            return; // Skip the rest of the update logic for inactive entities
        }

        // Apply gravity
        velocity.y += gravity * deltaTime;

        // Move the entity horizontally
        position.x += velocity.x * deltaTime;

        // Move the entity vertically
        position.y += velocity.y * deltaTime;

        // Check for collisions with other entities
        for (const auto& entity : entities) {
            if (entity.get() != this) {
                if (isIntersecting(entity)) {
                    handleCollision(entity);
                }
            }
        }

        // Update any other entity-specific logic here
        // Temporary BOUNDS
        if (position.x < 0.0f) {
            // Adjust the position and velocity to prevent going beyond the left boundary
            position.x = 0.0f;
            velocity.x = 0.0f;
        }
        else if (position.x > WINDOW_WIDTH - ENTITY_SIZE) {
            // Adjust the position and velocity to prevent going beyond the right boundary
            position.x = WINDOW_WIDTH - ENTITY_SIZE;
            velocity.x = 0.0f;
        }

        if (position.y > WINDOW_HEIGHT - ENTITY_SIZE) {
            // Adjust the position and velocity to prevent falling off the screen
            position.y = WINDOW_HEIGHT - ENTITY_SIZE;
            velocity.y = 0.0f;
            isJumping = false;
        }
    }

    void moveLeft() {
        velocity.x = -movementSpeed;
        currentDirection = Direction::Left;
    }

    void moveRight() {
        velocity.x = movementSpeed;
        currentDirection = Direction::Right;
    }

    void jump() {
        if (!isJumping) {
            isJumping = true;
            velocity.y = -jumpForce; // Instantly apply jump force
        }
    }

    enum class Direction {
        Left,
        Right
    };

    sf::Vector2f getPosition() const {
        return position;
    }

protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    bool isJumping;
    bool isAlive;
    float jumpForce;
    float gravity;
    float movementSpeed;

    Direction currentDirection;
    bool isIntersecting(const std::shared_ptr<MovableEntity>& otherEntity) const {
        sf::FloatRect thisBounds(position.x, position.y, 50.0f, 50.0f);
        sf::FloatRect otherBounds(otherEntity->position.x, otherEntity->position.y, 50.0f, 50.0f);
        return thisBounds.intersects(otherBounds);
    }

    void handleCollision(const std::shared_ptr<MovableEntity>& otherEntity) {
        sf::FloatRect thisBounds(position.x, position.y, 50.0f, 50.0f);
        sf::FloatRect otherBounds(otherEntity->position.x, otherEntity->position.y, 50.0f, 50.0f);
        sf::FloatRect intersection;
        thisBounds.intersects(otherBounds, intersection);

        // Determine the side of collision
        if (intersection.width < intersection.height) {
            // Horizontal collision
            if (position.x < otherEntity->position.x) {
                // Collided from the left
                position.x = otherEntity->position.x - 50.0f;
            }
            else {
                // Collided from the right
                position.x = otherEntity->position.x + 50.0f;
            }
            velocity.x = 0.0f;
        }
        else {
            // Vertical collision
            if (position.y < otherEntity->position.y) {
                // Collided from above
                position.y = otherEntity->position.y - 50.0f;
                velocity.y = 0.0f;
                isJumping = false; // Reset jumping state
            }
            else {
                // Collided from below
                position.y = otherEntity->position.y + 50.0f;
                velocity.y = 0.0f;
            }
        }
    }
};

class Player : public MovableEntity {
public:
    Player(float startX, float startY) : MovableEntity(startX, startY) {
        // Set up any other properties or resources specific to the player here
    }

    void update(float deltaTime, const std::vector<std::shared_ptr<MovableEntity>>& entities) {
        // Update the player's movement based on the input
        handleInput();

        // Call the base class update function to handle movement and physics
        MovableEntity::update(deltaTime, entities);

        // Add any other player-specific logic here
    }

private:
    void handleInput() {
        // Check the input and move the player accordingly

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            moveLeft();
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            moveRight();
        }
        else {
            // Stop moving if no input is detected
            velocity.x = 0.0f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            jump();
        }
    }
};

class Enemy : public MovableEntity {
public:
    Enemy(float startX, float startY) : MovableEntity(startX, startY) {
        // Set up any other properties or resources specific to the enemy here
    }

    void update(float deltaTime, const std::vector<std::shared_ptr<MovableEntity>>& entities) {
        // Update the enemy's movement and behavior here
        // For example, you could make it move in a specific pattern or track the player

        // Call the base class update function to handle movement and physics
        MovableEntity::update(deltaTime, entities);

        // Add any other enemy-specific logic here
    }
};

class Game {

public:
    Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game"), view(sf::FloatRect(0, 0, WINDOW_WIDTH/1.5, WINDOW_HEIGHT/1.5)) {
        // Set up any initial game variables or resources here
        window.setFramerateLimit(FPS);
        window.setView(view);

        // Create and add player entity
        player = std::make_shared<Player>(100.0f, WINDOW_HEIGHT - 50.0f); // -50.0f because of current height of the player
        // Create and add enemy entities
        spawnEnemies(5);
        entities.push_back(player);

        // Calculate the initial camera center based on the bounding box of all entities
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();

        initializeBorders();

        for (const auto& entity : entities) {
            const sf::Vector2f& position = entity->getPosition();
            if (position.x < minX) {
                minX = position.x;
            }
            if (position.x > maxX) {
                maxX = position.x;
            }
            if (position.y < minY) {
                minY = position.y;
            }
            if (position.y > maxY) {
                maxY = position.y;
            }
        }

        sf::Vector2f center((minX + maxX) / 2.0f, (minY + maxY) / 2.0f);

        // Set the initial camera center
        view.setCenter(center);
        window.setView(view);
        isRunning = true;
    }

    void spawnEnemies(int numEnemies) {
        float startX = WINDOW_WIDTH - ENTITY_SIZE*2; // Set the starting X position for the enemies
        float startY = ENTITY_SIZE; // Set the starting Y position for the enemies
        float spacing = ENTITY_SIZE + 10; // Set the spacing between enemies

        for (int i = 0; i < numEnemies; ++i) {
            std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(startX, startY);
            entities.push_back(enemy);

            startX -= spacing; // Update the X position for the next enemy
        }
    }

    void run() {
        sf::Clock clock;
        while (window.isOpen() && isRunning) {
            float deltaTime = clock.restart().asSeconds();
            processEvents();
            update(deltaTime);
            render();
        }
    }

private:
    sf::RenderWindow window;
    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<MovableEntity>> entities;
    sf::View view;
    bool isRunning;
    sf::RectangleShape leftBorder;
    sf::RectangleShape rightBorder;
    sf::RectangleShape bottomBorder;
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::X) {
                isRunning = false; // Set the flag to stop the game loop
                window.close(); // Close the window
            }
        }

    }
private:
    void initializeBorders() {
        // Initialize the left border
        leftBorder.setSize(sf::Vector2f(10.0f, WINDOW_HEIGHT));
        leftBorder.setFillColor(sf::Color::White);
        leftBorder.setPosition(0.0f, 0.0f);

        // Initialize the right border
        rightBorder.setSize(sf::Vector2f(10.0f, WINDOW_HEIGHT));
        rightBorder.setFillColor(sf::Color::White);
        rightBorder.setPosition(WINDOW_WIDTH - 10.0f, 0.0f);

        // Bottom border
        bottomBorder.setSize(sf::Vector2f(WINDOW_WIDTH, 10.0f));
        bottomBorder.setFillColor(sf::Color::White);
        bottomBorder.setPosition(0.0f, WINDOW_HEIGHT - 10.0f);
    }
    void update(float deltaTime) {
        // Update all entities
        for (const auto& entity : entities) {
            entity->update(deltaTime, entities);
        }

        // Calculate the bounding box of all entities
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();

        for (const auto& entity : entities) {
            const sf::Vector2f& position = entity->getPosition();
            if (position.x < minX) {
                minX = position.x;
            }
            if (position.x > maxX) {
                maxX = position.x;
            }
            if (position.y < minY) {
                minY = position.y;
            }
            if (position.y > maxY) {
                maxY = position.y;
            }
        }

        // Calculate the player's position
        sf::Vector2f playerPosition = player->getPosition();

        // Calculate the camera center based on the bounding box and player's position
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;

        // Calculate the distance between the camera center and player's position
        float distanceX = std::abs(centerX - playerPosition.x);
        float distanceY = std::abs(centerY - playerPosition.y);

        // Adjust the camera center based on the distance between entities and player
        if (distanceX > 400.0f) {
            centerX = playerPosition.x + (centerX - playerPosition.x) * 0.5f;
        }
        if (distanceY > 300.0f) {
            centerY = playerPosition.y + (centerY - playerPosition.y) * 0.5f;
        }

        // Keep the camera within the window bounds
        float halfWidth = view.getSize().x / 2.0f;
        float halfHeight = view.getSize().y / 2.0f;

        if (centerX - halfWidth < 0.0f) {
            centerX = halfWidth;
        }
        else if (centerX + halfWidth > WINDOW_WIDTH) {
            centerX = WINDOW_WIDTH - halfWidth;
        }

        if (centerY - halfHeight < 0.0f) {
            centerY = halfHeight;
        }
        else if (centerY + halfHeight > WINDOW_HEIGHT) {
            centerY = WINDOW_HEIGHT - halfHeight;
        }

        sf::Vector2f center(centerX, centerY);

        sf::Vector2f currentCenter = view.getCenter();
        sf::Vector2f newCenter = currentCenter + (center - currentCenter) * 0.05f;
        view.setCenter(newCenter);
        window.setView(view);
    }

    void render() {
        window.clear();

        // Draw borders
        window.draw(leftBorder);

        window.draw(rightBorder);

        window.draw(bottomBorder);

        // Render all entities
        for (const auto& entity : entities) {
            renderEntity(entity);
        }

        window.display();
    }

    void renderEntity(const std::shared_ptr<MovableEntity>& entity) {
        sf::RectangleShape shape(sf::Vector2f(50.0f, 50.0f));
        shape.setPosition(entity->getPosition());

        if (std::dynamic_pointer_cast<Player>(entity) != nullptr) {
            shape.setFillColor(sf::Color::Green);
        }
        else {
            shape.setFillColor(sf::Color::Red);
        }

        window.draw(shape);
    }

};

int main() {
    Game game;
    game.run();

    return 0;
}
