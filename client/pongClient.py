import socket
import constants
import pygame
import random

pygame.init()

# Configuración de la pantalla
WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Pong Two Players")

# Colores
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

# Tamaño de las paletas y la pelota
PADDLE_WIDTH, PADDLE_HEIGHT = 10, 100
BALL_SIZE = 10

# Velocidades
PADDLE_SPEED = 5
BALL_SPEED = 5
BALL_X_SPEED = random.choice((1, -1))
BALL_Y_SPEED = random.choice((1, -1))

# Posiciones iniciales
player1_y = HEIGHT // 2 - PADDLE_HEIGHT // 2
player2_y = HEIGHT // 2 - PADDLE_HEIGHT // 2
ball_x, ball_y = WIDTH // 2, HEIGHT // 2

# Puntuación
player1_score, player2_score = 0, 0

# Reloj para controlar la velocidad de la pantalla
clock = pygame.time.Clock()

# Función para dibujar las paletas, la pelota y la puntuación
def draw_objects():
    screen.fill(BLACK)
    pygame.draw.rect(screen, WHITE, (0, player1_y, PADDLE_WIDTH, PADDLE_HEIGHT))
    pygame.draw.rect(screen, WHITE, (WIDTH - PADDLE_WIDTH, player2_y, PADDLE_WIDTH, PADDLE_HEIGHT))
    pygame.draw.ellipse(screen, WHITE, (ball_x, ball_y, BALL_SIZE, BALL_SIZE))
    font = pygame.font.Font(None, 36)
    score_text = font.render(f"{player1_score} - {player2_score}", True, WHITE)
    screen.blit(score_text, (WIDTH // 2 - score_text.get_width() // 2, 10))
    
def connect_to_server(player_name): # Función para conectar al servidor
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((constants.IP_SERVER, constants.PORT))
    connect_message = f'{constants.CONNECT} {player_name}'
    client_socket.send(bytes(connect_message, constants.ENCODING_FORMAT))
    data_received = client_socket.recv(constants.RECV_BUFFER_SIZE)
    decoded_data = data_received.decode(constants.ENCODING_FORMAT)
    print(decoded_data)
    parts = decoded_data.split()
    if len(parts) >= 3 and parts[0] == "CONNECTED":
        player_id = int(parts[2])
        return client_socket, player_id

def main():
    print('***********************************')
    print('Client is running...')
    player_name = input('Enter your player name: ')
    if player_name == '':
        print('ERROR MISSING_NAME...')
        player_name = input('Please input a name...')
    client_socket, player_id = connect_to_server(player_name)

    # Bucle principal del juego
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                command_to_send = constants.DISCONNECT
                client_socket.send(bytes(command_to_send, constants.ENCODING_FORMAT))
                data_received = client_socket.recv(constants.RECV_BUFFER_SIZE)
                print(data_received.decode(constants.ENCODING_FORMAT))
                print('Closing connection...')

        keys = pygame.key.get_pressed()
        if keys[pygame.K_w] and player1_y > 0:
            player1_y -= PADDLE_SPEED
        if keys[pygame.K_s] and player1_y < HEIGHT - PADDLE_HEIGHT:
            player1_y += PADDLE_SPEED

        if keys[pygame.K_UP] and player2_y > 0:
            player2_y -= PADDLE_SPEED
        if keys[pygame.K_DOWN] and player2_y < HEIGHT - PADDLE_HEIGHT:
            player2_y += PADDLE_SPEED

        ball_x += BALL_SPEED * BALL_X_SPEED
        ball_y += BALL_SPEED * BALL_Y_SPEED

        # Colisión con las paletas
        if ball_x <= PADDLE_WIDTH and player1_y <= ball_y <= player1_y + PADDLE_HEIGHT:
            BALL_X_SPEED = 1
        elif ball_x >= WIDTH - PADDLE_WIDTH - BALL_SIZE and player2_y <= ball_y <= player2_y + PADDLE_HEIGHT:
            BALL_X_SPEED = -1

        # Colisión con las paredes superior e inferior
        if ball_y <= 0 or ball_y >= HEIGHT - BALL_SIZE:
            BALL_Y_SPEED *= -1

        # Puntuación
        if ball_x > WIDTH:
            point_message = f"{constants.POINT} {player_name}"
            client_socket.send(bytes(point_message, constants.ENCODING_FORMAT))
            player1_score += 1
            ball_x, ball_y = WIDTH // 2, HEIGHT // 2
            BALL_X_SPEED = random.choice((1, -1))
        elif ball_x < 0:
            data_received = client_socket.recv(constants.RECV_BUFFER_SIZE)
            if data_received == constants.OPPOSITE_POINT:
                player2_score += 1
                ball_x, ball_y = WIDTH // 2, HEIGHT // 2
                BALL_X_SPEED = random.choice((1, -1))

        draw_objects()
        pygame.display.update()
        clock.tick(60)

    pygame.quit()
    client_socket.close()

if __name__ == '__main__':
    main()