import pygame
import random
import sys
pygame.init()


WIDTH, HEIGHT = 800, 400
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Sword Duel Simulator")


WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
BLUE = (0, 0, 255)
GOLD = (255, 215, 0)

class Duelist:
    def __init__(self, x, y, controls, color):
        self.x = x
        self.y = y
        self.width = 40
        self.height = 80
        self.health = 3
        self.stamina = 100
        self.attack_cooldown = 0
        self.block_cooldown = 0
        self.attack_height = None 
        self.block_height = None
        self.is_blocking = False
        self.color = color
        self.controls = controls 
        self.facing = 1 if x < WIDTH//2 else -1  
        self.dodge_timer = 0
    
    def draw(self):
        
        pygame.draw.rect(screen, self.color, (self.x, self.y, self.width, self.height))
        
        
        if self.attack_height == 'high':
            pygame.draw.line(screen, GOLD, (self.x + self.width//2, self.y + 10), 
                            (self.x + self.width//2 + 50*self.facing, self.y - 20), 3)
        elif self.attack_height == 'mid':
            pygame.draw.line(screen, GOLD, (self.x + self.width//2, self.y + self.height//2), 
                            (self.x + self.width//2 + 60*self.facing, self.y + self.height//2), 3)
        elif self.attack_height == 'low':
            pygame.draw.line(screen, GOLD, (self.x + self.width//2, self.y + self.height - 10), 
                            (self.x + self.width//2 + 40*self.facing, self.y + self.height + 20), 3)
        
        
        for i in range(self.health):
            pygame.draw.circle(screen, RED, (self.x + 10 + i*15, self.y - 20), 5)
        
        
        pygame.draw.rect(screen, BLUE, (self.x, self.y - 10, self.stamina/2, 5))
    
    def update(self):
        
        if self.attack_cooldown > 0:
            self.attack_cooldown -= 1
        else:
            self.attack_height = None
        
        if self.block_cooldown > 0:
            self.block_cooldown -= 1
        else:
            self.is_blocking = False
            self.block_height = None
        
        if self.dodge_timer > 0:
            self.dodge_timer -= 1
        
        
        if self.stamina < 100:
            self.stamina += 0.1
    
    def attack(self, height):
        if self.attack_cooldown == 0 and self.stamina >= 20:
            self.attack_height = height
            self.attack_cooldown = 20
            self.stamina -= 20
            return True
        return False
    
    def block(self, height):
        if self.block_cooldown == 0 and self.stamina >= 10:
            self.is_blocking = True
            self.block_height = height
            self.block_cooldown = 15
            self.stamina -= 10
            return True
        return False
    
    def dodge(self):
        if self.dodge_timer == 0 and self.stamina >= 30:
            self.dodge_timer = 30
            self.stamina -= 30
            return True
        return False
    
    def move(self, dx):
        if self.dodge_timer > 25:  
            self.x += dx * 5 * self.facing
        elif self.dodge_timer > 0: 
            self.x += dx * 0.5 * self.facing
        else:
            self.x += dx * 2 * self.facing
        
        
        self.x = max(0, min(WIDTH - self.width, self.x))
        self.facing = 1 if self.x < WIDTH//2 else -1

player1 = Duelist(100, HEIGHT//2 - 40, {
    'up': pygame.K_w,
    'down': pygame.K_s,
    'left': pygame.K_a,
    'right': pygame.K_d,
    'high': pygame.K_q,
    'mid': pygame.K_e,
    'low': pygame.K_r,
    'block': pygame.K_LSHIFT,
    'dodge': pygame.K_SPACE
}, RED)

player2 = Duelist(WIDTH - 140, HEIGHT//2 - 40, {
    'up': pygame.K_UP,
    'down': pygame.K_DOWN,
    'left': pygame.K_LEFT,
    'right': pygame.K_RIGHT,
    'high': pygame.K_u,
    'mid': pygame.K_i,
    'low': pygame.K_o,
    'block': pygame.K_RSHIFT,
    'dodge': pygame.K_RETURN
}, BLUE)

game_active = True
winner = None
clock = pygame.time.Clock()
font = pygame.font.SysFont(None, 74)

def check_collision():
    
    if player1.attack_height and abs(player1.x - player2.x) < 80:
        if player2.dodge_timer > 20:  
            pass
        elif player2.is_blocking and player2.block_height == player1.attack_height:
            
            player2.stamina -= 5 
            if player2.block_cooldown > 12:  
                player1.attack_cooldown = 30  
        else:
           
            player2.health -= 1 if player1.attack_height == 'low' else 2 if player1.attack_height == 'mid' else 3
    
    if player2.attack_height and abs(player1.x - player2.x) < 80:
        if player1.dodge_timer > 20:
            pass
        elif player1.is_blocking and player1.block_height == player2.attack_height:
            player1.stamina -= 5
            if player1.block_cooldown > 12:
                player2.attack_cooldown = 30
        else:
            player1.health -= 1 if player2.attack_height == 'low' else 2 if player2.attack_height == 'mid' else 3


while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()
        if event.type == pygame.KEYDOWN and not game_active:
            if event.key == pygame.K_r:
                
                player1 = Duelist(100, HEIGHT//2 - 40, player1.controls, RED)
                player2 = Duelist(WIDTH - 140, HEIGHT//2 - 40, player2.controls, BLUE)
                game_active = True
                winner = None
    
    if game_active:
        
        keys = pygame.key.get_pressed()
        
       
        dx = keys[player1.controls['right']] - keys[player1.controls['left']]
        dy = keys[player1.controls['down']] - keys[player1.controls['up']]
        player1.move(dx)
        
        
        if keys[player1.controls['high']]: player1.attack('high')
        if keys[player1.controls['mid']]: player1.attack('mid')
        if keys[player1.controls['low']]: player1.attack('low')
        if keys[player1.controls['block']]: player1.block('mid')  
        if keys[player1.controls['dodge']]: player1.dodge()
        
        
        dx = keys[player2.controls['right']] - keys[player2.controls['left']]
        dy = keys[player2.controls['down']] - keys[player2.controls['up']]
        player2.move(dx)
        
       
        if keys[player2.controls['high']]: player2.attack('high')
        if keys[player2.controls['mid']]: player2.attack('mid')
        if keys[player2.controls['low']]: player2.attack('low')
        if keys[player2.controls['block']]: player2.block('mid')
        if keys[player2.controls['dodge']]: player2.dodge()
        
        
        player1.update()
        player2.update()
        
        
        check_collision()
        
        
        if player1.health <= 0:
            game_active = False
            winner = "Player 2"
        elif player2.health <= 0:
            game_active = False
            winner = "Player 1"
    

    screen.fill(BLACK)
    

    pygame.draw.rect(screen, (50, 50, 50), (0, HEIGHT//2 + 40, WIDTH, HEIGHT//2 - 40))
    player1.draw()
    player2.draw()
    

    if not game_active and winner:
        text = font.render(f"{winner} wins!", True, WHITE)
        restart = font.render("Press R to restart", True, WHITE)
        screen.blit(text, (WIDTH//2 - text.get_width()//2, HEIGHT//2 - 50))
        screen.blit(restart, (WIDTH//2 - restart.get_width()//2, HEIGHT//2 + 50))
    
    pygame.display.flip()
    clock.tick(60)
