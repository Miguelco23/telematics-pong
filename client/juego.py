import pygame
import random

# Inicialización de Pygame
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

# Velocidad de las paletas
PADDLE_SPEED = 5

# Velocidad de la pelota
BALL_SPEED = 5
BALL_X_SPEED = random.choice((1, -1))
BALL_Y_SPEED = random.choice((1, -1))

# Posiciones iniciales de las paletas
player1_y, player2_y = HEIGHT // 2 - PADDLE_HEIGHT // 2, HEIGHT // 2 - PADDLE_HEIGHT // 2

# Posiciones iniciales de la pelota
ball_x, ball_y = WIDTH // 2, HEIGHT // 2

# Puntuación
player1_score, player2_score = 0, 0

# Reloj para controlar la velocidad de la pantalla
clock = pygame.time.Clock()

# Función para dibujar las paletas y la pelota
def draw_objects():
    screen.fill(BLACK)
    pygame.draw.rect(screen, WHITE, (0, player1_y, PADDLE_WIDTH, PADDLE_HEIGHT))
    pygame.draw.rect(screen, WHITE, (WIDTH - PADDLE_WIDTH, player2_y, PADDLE_WIDTH, PADDLE_HEIGHT))
    pygame.draw.ellipse(screen, WHITE, (ball_x, ball_y, BALL_SIZE, BALL_SIZE))

# Bucle principal del juego
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

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
    if ball_x < 0:
        player2_score += 1
        ball_x, ball_y = WIDTH // 2, HEIGHT // 2
        BALL_X_SPEED = random.choice((1, -1))
    elif ball_x > WIDTH:
        player1_score += 1
        ball_x, ball_y = WIDTH // 2, HEIGHT // 2
        BALL_X_SPEED = random.choice((1, -1))

    draw_objects()
    pygame.display.update()
    clock.tick(60)

pygame.quit()