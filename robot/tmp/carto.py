import tkinter
n = 2 #nombre de robots
#state : 0=pas encore visité, 1=déja visité, 2=voisins déja visités
class Node:
    def __init__(self):
        self.state = 0
        self.edges = [0,0,0,0]
    def set(self, )
    
    
def valid(x, y, grid, myid, presentX, presentY):
    # to be valid the position has to be inside the grid and not yet been visited
    for i in range(n)
    return 

def updateEverySecond(fenetre: tkinter.Tk, canva: tkinter.Canvas, tableau, W, H, lastX, lastY, presentX, presentY):
    
                    
    
    
    longueur_case = W//(len(tableau[0]))
    largeur_case = H//(len(tableau))
    
    for i in range(len(tableau)):
        for j in range(len(tableau[i])):
            a, b = i+1, j+1
            haut, bas, gauche, droite = (None,)*4
            now = tableau[i][j]
            if i != 0:
                haut = tableau[i-1][j]
            if i != len(tableau) -1:
                bas = tableau[i+1][j]
            if j != 0:
                gauche = tableau[i][j-1]
            if j != len(tableau[i]) -1:
                droite = tableau[i][j+1]
                
            if now:
                if droite:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    (b+1)*longueur_case, a*largeur_case,
                                    fill ='black', width = 2)
                if bas:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    b*longueur_case, (a+1)*largeur_case,
                                    fill ='black', width = 2)
    fenetre.after(1000, updateEverySecond)

fenetre = tkinter.Tk()
fenetre.title("Affichage du Labyrinthe")
W = 500
H = 400
canva = tkinter.Canvas(fenetre, width = W, height = H)
canva.pack()

lastX = []
lastY = []
presentX = []
presentY = []
for i in range(n):
    lastX.append(0)
    lastY.append(0)
    presentX.append(0)
    presentY.append(0)
presentX[1] = 1
presentY[0] = 1


tableau = [[1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1],
[1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1],
[1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1],
[1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1],
[1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1],
[1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1],
[1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1],
[1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1],
[1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1],
[1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1],
[1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1],
[1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1],
[1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1],
[1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1],
[0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1],
[0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1],
[0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1],
[0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0]]

for i in range(len(tableau)):
    for j in range(len(tableau[i])):
        tableau[i][j] = Node()

updateEverySecond(fenetre, canva, tableau, W, H, lastX, lastY)
fenetre.mainloop()
            
            
