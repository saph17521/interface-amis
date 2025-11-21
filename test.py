import pygame
import tkinter as tk
from tkinter import filedialog
import pandas as pd

def open_excel_table_console():
    """
    Demande via la console :
    - Le nombre de lignes et de colonnes du tableau Excel.
    - Les valeurs de chaque ligne (les valeurs doivent être séparées par un espace).
    
    Puis utilise Tkinter pour ouvrir une boîte de dialogue permettant de choisir
    l'emplacement et le nom du fichier Excel. Le tableau est ensuite sauvegardé
    grâce à pandas.
    """
    try:
        rows = int(input("Entrez le nombre de lignes : "))
        cols = int(input("Entrez le nombre de colonnes : "))
    except ValueError:
        print("Entrée invalide pour les dimensions.")
        return
    
    matrix = []
    for i in range(rows):
        row_input = input(f"Entrez les {cols} valeurs pour la ligne {i+1} (séparées par un espace) : ")
        row_values = row_input.split()
        if len(row_values) != cols:
            print(f"Nombre de valeurs incorrect pour la ligne {i+1}.")
            return
        matrix.append(row_values)
    
    # Utiliser Tkinter pour choisir le chemin d'enregistrement du fichier Excel
    root = tk.Tk()
    root.withdraw()  # Masquer la fenêtre principale Tkinter
    file_path = filedialog.asksaveasfilename(
        title="Enregistrer le tableau Excel",
        defaultextension=".xlsx",
        filetypes=[("Fichiers Excel", "*.xlsx"), ("Tous les fichiers", "*.*")]
    )
    if file_path:
        df = pd.DataFrame(matrix)
        df.to_excel(file_path, index=False, header=False)
        print("Tableau Excel sauvegardé dans", file_path)
    else:
        print("Aucun chemin sélectionné, opération annulée.")
    root.destroy()

def main():
    # Initialisation de Pygame
    pygame.init()
    screen = pygame.display.set_mode((500, 300))
    pygame.display.set_caption("Pygame - Saisie de Tableau Excel")
    
    # Bouton pour lancer la saisie du tableau Excel via la console et enregistrer avec Tkinter
    button_excel_table_rect = pygame.Rect(190, 125, 120, 50)
    
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if button_excel_table_rect.collidepoint(event.pos):
                    open_excel_table_console()
        
        # Affichage de l'interface Pygame
        screen.fill((255, 255, 255))
        pygame.draw.rect(screen, (0, 128, 255), button_excel_table_rect)
        
        font = pygame.font.SysFont(None, 20)
        table_text = font.render("Tableau Excel", True, (255, 255, 255))
        screen.blit(table_text, table_text.get_rect(center=button_excel_table_rect.center))
        
        pygame.display.flip()
    
    pygame.quit()

if __name__ == '__main__':
    main()