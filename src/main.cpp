// Calculo de rayos: https://lodev.org/cgtutor/raycasting.html

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int world_map[MAP_WIDTH][MAP_HEIGHT] = {
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void handle_keyboard(sf::Time elapsed, int world_map[][MAP_HEIGHT], double& pos_x, double& pos_y, double& dir_x, double& dir_y, double& plane_x, double& plane_y);
void draw_world(sf::RenderWindow& window, double pos_x, double pos_y, double dir_x, double dir_y, double plane_x, double plane_y);

int main() {

	// Posicion inicial
	double pos_x = 22, pos_y = 12;

	// Direccion inicial
	double dir_x = -1, dir_y = 0;

	// "Plano" de la camara
	double plane_x = 0, plane_y = 0.66;

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Raycaster");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock clock;

	while (window.isOpen()) {

		// Polling de eventos
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::Time elapsed = clock.restart();

		handle_keyboard(elapsed, world_map, pos_x, pos_y, dir_x, dir_y, plane_x, plane_y);

		// Limpio la pantalla
		window.clear(sf::Color::Black);

		// Calculo rayos y dibujo al back buffer
		draw_world(window, pos_x, pos_y, dir_x, dir_y, plane_x, plane_y);

		// Muestro el back buffer
		window.display();

	}

	return 0;
}

void handle_keyboard(sf::Time elapsed, int world_map[][MAP_HEIGHT], double& pos_x, double& pos_y, double& dir_x, double& dir_y, double& plane_x, double& plane_y) {
	double move_speed = elapsed.asSeconds() * 5.0; // En cuadrados/seg
	double rot_speed = elapsed.asSeconds() * 3.0; // En rad/seg

	// Muevo hacia adelante si no hay pared
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		if (!world_map[(int)(pos_x + dir_x * move_speed)][(int)pos_y])
			pos_x += dir_x * move_speed;
		if (!world_map[(int)pos_x][(int)(pos_y + dir_y * move_speed)])
			pos_y += dir_y * move_speed;
	}
	// Muevo hacia atras si no hay pared
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		if (!world_map[(int)(pos_x - dir_x * move_speed)][(int)pos_y])
			pos_x -= dir_x * move_speed;
		if (!world_map[(int)pos_x][(int)(pos_y - dir_y * move_speed)])
			pos_y -= dir_y * move_speed;
	}
	// Rotacion hacia la derecha
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		// Roto la direccion del jugador y del plano de la camara
		double old_dir_x = dir_x;
		dir_x = dir_x * std::cos(-rot_speed) - dir_y * std::sin(-rot_speed);
		dir_y = old_dir_x * std::sin(-rot_speed) + dir_y * std::cos(-rot_speed);
		double old_plane_x = plane_x;
		plane_x = plane_x * std::cos(-rot_speed) - plane_y * std::sin(-rot_speed);
		plane_y = old_plane_x * std::sin(-rot_speed) + plane_y * std::cos(-rot_speed);
	}
	// Rotacion hacia la izquierda
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		// Roto la direccion del jugador y del plano de la camara
		double old_dir_x = dir_x;
		dir_x = dir_x * std::cos(rot_speed) - dir_y * std::sin(rot_speed);
		dir_y = old_dir_x * std::sin(rot_speed) + dir_y * std::cos(rot_speed);
		double old_plane_x = plane_x;
		plane_x = plane_x * std::cos(rot_speed) - plane_y * std::sin(rot_speed);
		plane_y = old_plane_x * std::sin(rot_speed) + plane_y * std::cos(rot_speed);
	}

}

void draw_world(sf::RenderWindow& window, double pos_x, double pos_y, double dir_x, double dir_y, double plane_x, double plane_y) {
	for (int x = 0; x < SCREEN_WIDTH; x++) {

		// Calculo posicion y direccion
		double camera_x = 2 * x / double(SCREEN_WIDTH) - 1;
		double ray_dir_x = dir_x + plane_x * camera_x;
		double ray_dir_y = dir_y + plane_y * camera_x;

		// Coordenadas de la celda actual
		int map_x = (int)pos_x;
		int map_y = (int)pos_y;

		// Longitud del rayo desde la posicion actual hasta la siguiente interseccion con una celda en x o en y
		double side_dist_x;
		double side_dist_y;

		// Longitud del rayo desde una posicion x o y hasta la proxima
		double delta_dist_x = (ray_dir_x == 0) ? 1e30 : std::abs(1 / ray_dir_x);
		double delta_dist_y = (ray_dir_y == 0) ? 1e30 : std::abs(1 / ray_dir_y);
		double perp_wall_dist;

		// En que direccion moverse
		int step_x;
		int step_y;

		// Hubo colision?
		int hit = 0;

		// Impacto en paredes Norte-Sur o Este-Oeste?
		int side;

		// Calculo el paso y la distancia al lado inicial
		if (ray_dir_x < 0) {
			step_x = -1;
			side_dist_x = (pos_x - map_x) * delta_dist_x;
		}
		else {
			step_x = 1;
			side_dist_x = (map_x + 1.0 - pos_x) * delta_dist_x;
		}
		if (ray_dir_y < 0) {
			step_y = -1;
			side_dist_y = (pos_y - map_y) * delta_dist_y;
		}
		else {
			step_y = 1;
			side_dist_y = (map_y + 1.0 - pos_y) * delta_dist_y;
		}

		// Inicio del algoritmo DDA
		while (!hit) {

			// Salto a la siguiente celda del mapa, en x o en y
			if (side_dist_x < side_dist_y) {
				side_dist_x += delta_dist_x;
				map_x += step_x;
				side = 0;
			}
			else {
				side_dist_y += delta_dist_y;
				map_y += step_y;
				side = 1;
			}

			// Hubo impacto?
			if (world_map[map_x][map_y] > 0)
				hit = 1;
		}

		// Calculo la distancia del rayo a la pared, proyectada en el plano de la camara
		if (!side)
			perp_wall_dist = (side_dist_x - delta_dist_x);
		else
			perp_wall_dist = (side_dist_y - delta_dist_y);

		// Calculo la altura de la linea a dibujar en pantalla
		int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);

		// Calculo el pixel mas alto y mas bajo para rellenar la franja actual
		int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
		if (draw_start < 0)
			draw_start = 0;
		int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
		if (draw_end >= SCREEN_HEIGHT)
			draw_end = SCREEN_HEIGHT - 1;

		// Elijo el color de la pared
		sf::Color color;
		switch (world_map[map_x][map_y]) {
		case 1: color = sf::Color::Red; break;
		case 2: color = sf::Color::Green; break;
		case 3: color = sf::Color::Blue; break;
		case 4: color = sf::Color::White; break;
		default: color = sf::Color::Yellow; break;
		}

		// Diferentes brillos para los lados x e y
		if (side == 1)
			color = sf::Color(color.r,color.g,color.b,color.a/2);

		// Dibujo la linea vertical
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(x, draw_start), color),
			sf::Vertex(sf::Vector2f(x, draw_end), color)
		};
		window.draw(line, 2, sf::Lines);

	}
}