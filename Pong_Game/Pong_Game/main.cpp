#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>

const std::string BACKGROUND_IMAGE_PATH = "pong.jpg";
const sf::Vector2f BACKGROUND_SIZE(1000,750);
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 750;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 160;
const float BALL_RADIUS = 10;
const int SCORE_TEXT_SIZE = 40;
const float BASE_BALL_SPEED = 0.4f;
const float BALL_SPEED_INCREMENT = 0.01f;
const float PADDLE3_SPEED = 0.3f;
sf::Color paddle1Color = sf::Color(9,1,158);
sf::Color paddle2Color = sf::Color(9,1,158);
sf::Color paddle3Color = sf::Color(43,5,250);
sf::Color ballColor = sf::Color(43,5,250);




class Paddle
{
public:
    Paddle(float startX, float startY, float speed)
    {
        shape.setPosition(startX, startY);
        shape.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
        shape.setFillColor(sf::Color::White);
        shape.setOrigin(PADDLE_WIDTH / 2, PADDLE_HEIGHT / 2);
        paddleSpeed = speed;
    }

    void move(float offsetY)
    {
        shape.move(0, offsetY * paddleSpeed);
    }

    void update()
    {
        // Additional logic for the paddle if needed
        float paddleY = shape.getPosition().y;
        if (paddleY - PADDLE_HEIGHT / 2 < 0)
        {
            shape.setPosition(shape.getPosition().x, PADDLE_HEIGHT / 2);
            paddleSpeed = -paddleSpeed; // Reverse paddle direction
        }
        else if (paddleY + PADDLE_HEIGHT / 2 > WINDOW_HEIGHT)
        {
            shape.setPosition(shape.getPosition().x, WINDOW_HEIGHT - PADDLE_HEIGHT / 2);
            paddleSpeed = -paddleSpeed; // Reverse paddle direction
        }
    }

    sf::RectangleShape shape;

private:
    float paddleSpeed;
};

class Ball
{
public:
    Ball(float startX, float startY, Paddle& paddle1, Paddle& paddle2, Paddle& paddle3)
        : paddle1(paddle1), paddle2(paddle2), paddle3(paddle3)
    {
        shape.setPosition(startX, startY);
        shape.setRadius(BALL_RADIUS);
        shape.setFillColor(sf::Color::White);
        shape.setOrigin(BALL_RADIUS, BALL_RADIUS);
        reset();
    }

    void reset()
    {
        float paddleOffset = PADDLE_HEIGHT / 2.0f;
        float paddle1Y = paddle1.shape.getPosition().y;
        float paddle2Y = paddle2.shape.getPosition().y;

        if (rand() % 2 == 0) // Randomly choose which paddle the ball starts on
        {
            shape.setPosition(paddle1.shape.getPosition().x + PADDLE_WIDTH, paddle1Y);
        }
        else
        {
            shape.setPosition(paddle2.shape.getPosition().x - PADDLE_WIDTH, paddle2Y);
        }

        float angle = rand() % 90 - 45;
        float pi = 3.14159f;
        direction.x = cos(angle * pi / 180);
        direction.y = sin(angle * pi / 180);
        speed = BASE_BALL_SPEED;
        isStationary = true;
    }

    void update()
    {
        if (!gameStarted || isStationary)
            return;

        shape.move(direction * speed);

        if (shape.getPosition().y - BALL_RADIUS < 0 || shape.getPosition().y + BALL_RADIUS > WINDOW_HEIGHT)
        {
            direction.y = -direction.y;
        }
    }

    void startMovement()
    {
        isStationary = false;
    }

    void increaseSpeed()
    {
        speed += BALL_SPEED_INCREMENT;
    }

    void checkPaddleCollision()
    {
        if (shape.getGlobalBounds().intersects(paddle1.shape.getGlobalBounds()))
        {
            direction.x = std::abs(direction.x); // Ball bounces towards the right
            direction.y = calculateRandomDirection(); // Set a new random vertical direction
            increaseSpeed();
        }
        else if (shape.getGlobalBounds().intersects(paddle2.shape.getGlobalBounds()))
        {
            direction.x = -std::abs(direction.x); // Ball bounces towards the left
            direction.y = calculateRandomDirection(); // Set a new random vertical direction
            increaseSpeed();
        }
        else if (shape.getGlobalBounds().intersects(paddle3.shape.getGlobalBounds()))
        {
            direction.x = -direction.x; // Ball bounces towards the opposite direction
            direction.y = calculateRandomDirection(); // Set a new random vertical direction
            increaseSpeed();
        }
    }

    float calculateRandomDirection()
    {
        // Generate a random float value between -1 and 1
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
    }
    sf::CircleShape shape;
    sf::Vector2f direction;
    bool gameStarted = false;

private:

    float speed;
    bool isStationary = true;
    Paddle& paddle1;
    Paddle& paddle2;
    Paddle& paddle3;
};

int main()
{
    sf::Texture backgroundTexture;
       if (!backgroundTexture.loadFromFile(BACKGROUND_IMAGE_PATH))
       {
           // Error handling if the background image fails to load
           return -1;
       }

       sf::Sprite backgroundSprite;
       backgroundSprite.setTexture(backgroundTexture);
       backgroundSprite.setPosition(0, 0);
       backgroundSprite.setScale(BACKGROUND_SIZE.x / backgroundSprite.getLocalBounds().width, BACKGROUND_SIZE.y / backgroundSprite.getLocalBounds().height);

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pong");

    Paddle paddle1(20, WINDOW_HEIGHT / 2, 0.4f);
    Paddle paddle2(WINDOW_WIDTH - 20, WINDOW_HEIGHT / 2, 0.4f);
    Paddle paddle3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, PADDLE3_SPEED);

    paddle1.shape.setFillColor(paddle1Color);
    paddle2.shape.setFillColor(paddle2Color);
    paddle3.shape.setFillColor(paddle3Color);

    float ballStartX = WINDOW_WIDTH / 2;
    float ballStartY = rand() % (WINDOW_HEIGHT - PADDLE_HEIGHT) + PADDLE_HEIGHT / 2;

    Ball ball(ballStartX, ballStartY, paddle1, paddle2, paddle3);
    ball.shape.setFillColor(ballColor);

    int score1 = 0;
    int score2 = 0;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        return -1;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(SCORE_TEXT_SIZE);
    scoreText.setFillColor(sf::Color::Blue);
    scoreText.setPosition(WINDOW_WIDTH / 2 - 20, 10);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && paddle1.shape.getPosition().y - PADDLE_HEIGHT / 2 > 0)
        {
            paddle1.move(-1.0f);
            ball.gameStarted = true;
            ball.startMovement();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && paddle1.shape.getPosition().y + PADDLE_HEIGHT / 2 < WINDOW_HEIGHT)
        {
            paddle1.move(1.0f);
            ball.gameStarted = true;
            ball.startMovement();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && paddle2.shape.getPosition().y - PADDLE_HEIGHT / 2 > 0)
        {
            paddle2.move(-1.0f);
            ball.gameStarted = true;
            ball.startMovement();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && paddle2.shape.getPosition().y + PADDLE_HEIGHT / 2 < WINDOW_HEIGHT)
        {
            paddle2.move(1.0f);
            ball.gameStarted = true;
            ball.startMovement();
        }

        paddle3.move(1.0f);
        if (paddle3.shape.getPosition().y - PADDLE_HEIGHT / 2 > WINDOW_HEIGHT)
        {
            paddle3.shape.setPosition(WINDOW_WIDTH / 2, -PADDLE_HEIGHT / 2);
        }
        else if (paddle3.shape.getPosition().y + PADDLE_HEIGHT / 2 < 0)
        {
            paddle3.shape.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT + PADDLE_HEIGHT / 2);
        }

        paddle1.update();
        paddle2.update();
        paddle3.update();

        ball.update();

        if (ball.shape.getGlobalBounds().intersects(paddle1.shape.getGlobalBounds()) ||
            ball.shape.getGlobalBounds().intersects(paddle2.shape.getGlobalBounds()) ||
            ball.shape.getGlobalBounds().intersects(paddle3.shape.getGlobalBounds()))
        {
            ball.direction.x = -ball.direction.x;
            ball.increaseSpeed();
        }
        else if (ball.shape.getPosition().x - BALL_RADIUS < 0)
        {
            ball.reset();
            score2++;
        }
        else if (ball.shape.getPosition().x + BALL_RADIUS > WINDOW_WIDTH)
        {
            ball.reset();
            score1++;
        }

        window.clear();
        window.draw(backgroundSprite);


        window.draw(paddle1.shape);
        window.draw(paddle2.shape);
        window.draw(paddle3.shape);
        window.draw(ball.shape);

        scoreText.setString(std::to_string(score1) + " - " + std::to_string(score2));
        window.draw(scoreText);

        window.display();
    }

    return 0;
}
