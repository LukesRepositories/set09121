#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

const Keyboard::Key controls[4] = {
	Keyboard::A,	// Player1 UP
	Keyboard::Z,	// Player1 Down
	Keyboard::Up,	// Player2 Up
	Keyboard::Down,	// Player2 Down
};
const Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;

CircleShape ball;
RectangleShape paddles[2];

Vector2f ballVelocity;
bool isPlayer1Serving = false;
const float initialVelocityX = 100.f;
const float initialVelocityY = 60.f;

const float velocityMultiplier = 1.1f;

Font font;
Text text;

int player1Score = -1;
int player2Score = 0;

void Reset(bool isPlayer1ServingArg) {
	isPlayer1Serving = isPlayer1ServingArg;
	// reset paddle position
	paddles[0].setPosition(Vector2f(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f));
	paddles[1].setPosition(Vector2f((gameWidth - (paddleOffsetWall + paddleSize.x / 2.f)), (gameHeight / 2.f))); // Fill in the ...
	// reset Ball Position
	ball.setPosition((gameWidth / 2.f), (gameHeight / 2.f));

	ballVelocity = { (isPlayer1Serving ? initialVelocityX : -initialVelocityX), initialVelocityY };

	// Update score
	if (isPlayer1ServingArg) {
		player2Score++;
	}
	else {
		player1Score++;
	}

	string funcText = to_string(player1Score) + " - " + to_string(player2Score);

	// Update Score Text
	text.setString(funcText);
	// Keep Score Text Centered
	text.setPosition((gameWidth * .5f) - (text.getLocalBounds().width * .5f), 0);
}

void Load() {
	// Set size and origin of paddles
	for (auto& p : paddles) {
		p.setSize(paddleSize);
		p.setOrigin(paddleSize / 2.f);
	}
	// Set size and origin of ball
	ball.setRadius(ballRadius);
	ball.setOrigin(Vector2f(ballRadius, ballRadius)); // Should be half the ball width and height
	
	// Load font-face from res dir
	font.loadFromFile("C:/Users/lukem/source/repos/set09121/res/OpenSans-Regular.ttf"); // CHANGE THIS via CMake so that it works on different devices
	// Set text element to use font
	text.setFont(font);
	// set the character size to 24 pixels
	text.setCharacterSize(24);

	// Reset function 
	Reset(isPlayer1Serving);
}

void Update(RenderWindow& window) {
	// Reset clock, recalculate deltatime
	static Clock clock;
	float dt = clock.restart().asSeconds();
	// check and consume events
	Event event;
	while (window.pollEvent(event)) {
		if (event.type == Event::Closed) {
			window.close();
			return;
		}
	}

	// Quit via ESC Key
	if (Keyboard::isKeyPressed(Keyboard::Escape)) {
		window.close();
	}

	ball.move(ballVelocity * dt);

	// handle paddle movement Player1
	float direction = 0.0f;
	if (Keyboard::isKeyPressed(controls[0]) && (paddles[0].getPosition().y - (paddleSize.y * 0.5)) > 0) {
		direction--;
	}
	if (Keyboard::isKeyPressed(controls[1]) && (paddles[0].getPosition().y + (paddleSize.y * 0.5)) < gameHeight) {
		direction++;
	}
	paddles[0].move(Vector2f(0.f, direction * paddleSpeed * dt));

	// handle paddle movement Player2 or AI paddle movement
	direction = 0.0f;
	if (ball.getPosition().y < paddles[1].getPosition().y && (paddles[1].getPosition().y - (paddleSize.y * 0.5)) > 0) {
		direction--;
	}
	if (ball.getPosition().y > paddles[1].getPosition().y && (paddles[1].getPosition().y + (paddleSize.y * 0.5)) < gameHeight) {
		direction++;
	}
	paddles[1].move(Vector2f(0.f, direction * paddleSpeed * dt));

	// check ball collision
	const float bx = ball.getPosition().x;
	const float by = ball.getPosition().y;
	if (by > gameHeight) { //bottom wall
		// bottom wall
		ballVelocity.x *= velocityMultiplier;
		ballVelocity.y *= -velocityMultiplier;
		ball.move(Vector2f(0.f, -10.f));
	}
	else if (by < 0) { //top wall
		// top wall
		ballVelocity.x *= velocityMultiplier;
		ballVelocity.y *= -velocityMultiplier;
		ball.move(Vector2f(0.f, 10.f));
	}

	else if (bx > gameWidth) {
		// right wall
		Reset(false);
	}
	else if (bx < 0) {
		// left wall
		Reset(true);
	}

	else if (
		//ball is inline or behind paddle AND
		bx < paddleSize.x + paddleOffsetWall &&
		//ball is below top edge of paddle AND
		by > paddles[0].getPosition().y - (paddleSize.y * 0.5) &&
		//ball is above bottom edge of paddle
		by < paddles[0].getPosition().y + (paddleSize.y * 0.5))
	{
		// bounce off left paddle
		ballVelocity.x *= -velocityMultiplier;
		ballVelocity.y *= velocityMultiplier;
		ball.move(Vector2f(10.f, 0.f));
	}
	else if (
		//ball is inline or behind paddle AND
		bx > (gameWidth - paddleSize.x - paddleOffsetWall) &&
		//ball is below top edge of paddle AND
		by > paddles[1].getPosition().y - (paddleSize.y * 0.5) &&
		//ball is above bottom edge of paddle
		by < paddles[1].getPosition().y + (paddleSize.y * 0.5))
	{
		// bounce off right paddle
		ballVelocity.x *= -velocityMultiplier;
		ballVelocity.y *= velocityMultiplier;
		ball.move(Vector2f(-10.f, 0.f));
	}

}

void Render(RenderWindow& window) {
	// Draw Everything
	window.draw(paddles[0]);
	window.draw(paddles[1]);
	window.draw(ball);
	window.draw(text);
}

int main() {
	RenderWindow window(VideoMode(gameWidth, gameHeight), "PONG");
	Load();
	while (window.isOpen()) {
		window.clear();
		Update(window);
		Render(window);
		window.display();
	}
	return 0;
}