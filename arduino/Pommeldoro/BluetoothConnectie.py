import pygame
from BluetoothFunctie import BLE

pygame.init()
screen = pygame.display.set_mode((1280, 720))
klok = pygame.time.Clock()
font = pygame.font.SysFont(None, 48)
state = "Wacht status"

Bluetooth = BLE()
Bluetooth.verbind()          # Start verbinding op achtergrond

running = True
while running:
    #Bluetooth uitlezen
    waarde = Bluetooth.lees_waarde()
    status = Bluetooth.is_verbonden()


    if status == True:
        achtergrondKleur = pygame.Color(120,200,120)
    else:
        achtergrondKleur= pygame.Color(200,120,120)
    
    screen.fill(achtergrondKleur)
    
    if waarde == 0:
        state = "Klaar om te studeren"
    elif waarde == 1:
        state = "Kantelen"
    elif waarde == 2:
        state= "Studeren"
    elif waarde ==3:
        state = "Kantelen"
    elif waarde ==4:
        state ="Pauze"
    if status==False:
        state = "Wachten op connectie"
    
    Txt = font.render(state,1,'white')
    Txt_rect = Txt.get_rect(center=(screen.width/2,screen.height/2))
    screen.blit(Txt,Txt_rect)



    pygame.display.flip()
    klok.tick(60)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False



Bluetooth.verbreek()         # Netjes afsluiten
pygame.quit()