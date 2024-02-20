import tkinter

#--------------------------------------------------------FONCTIONS----------------------------------------------------------------#

n = 2 #nombre de robots

#state : 0=pas encore visité, 1=déja visité, 2=voisins déja visités
class Node:
    def __init__(self):
        self.state = 0
        self.edges = [0,0,0,0] #dir : 0 = up, 1 = right, 2 = down, 3 = left
    def getState(self):
        return self.state
    def getEdges(self):
        return self.edges
    def setState(self, s):
        self.state = s
    def visit(self,dir): #dir : 0 = up, 1 = right, 2 = down, 3 = left
        self.edges[dir] = 1
        if (self.edges == [1,1,1,1]):
            self.state = 2
        else:
            self.state = 1

def valid(pos):
    # to be valid the position has to be inside the tableau and not yet been visited
    if ((pos[0]>=len(tableau))or(pos[1]>=len(tableau))):
        return False
    for i in robList:
        pp = i.getPresentPos()
        if ((pp[0]==pos[0]) and (pp[1]==pos[1])):
            return False
    if (tableau[pos[0]][pos[1]].getState() == 2):
        return False
    return True

class Robot:
    def __init__(self,id):
        self.id = id
        self.lastPos = [0,0]
        self.presentPos = [0,0]
        self.history = [[0,0]]
    def getLastPos(self):
        return self.lastPos
    def getPresentPos(self):
        return self.presentPos
    def getOneHistory (self):
        rep = self.history.pop(-1)
        return rep
    def move(self, pos):
        if(valid(pos)):
            self.lastPos = self.presentPos
            self.presentPos = pos
            self.history.append(self.lastPos)
            if (self.lastPos[0]==(pos[0]-1)):
                tableau[self.lastPos[0]][self.lastPos[1]].visit(2)
                tableau[pos[0]][pos[1]].visit(0)
            elif (self.lastPos[1]==(pos[1]+1)):
                tableau[self.lastPos[0]][self.lastPos[1]].visit(3)
                tableau[pos[0]][pos[1]].visit(1)
            elif (self.lastPos[0]==(pos[0]+1)):
                tableau[self.lastPos[0]][self.lastPos[1]].visit(0)
                tableau[pos[0]][pos[1]].visit(2)
            elif (self.lastPos[1]==(pos[1]-1)):
                tableau[self.lastPos[0]][self.lastPos[1]].visit(1)
                tableau[pos[0]][pos[1]].visit(3)
            return 1
        else:
            return 0

def updateEverySecond():
    #------------------------------DECISION----------------------------#  
    
    
    
    #------------------------------AFFICHE-----------------------------#    
    longueur_case = W//(longueurX)
    largeur_case = H//(longueurY)
    
    for i in range(longueurY):
        for j in range(longueurX):
            a, b = i+1, j+1
            haut, bas, gauche, droite = (None,)*4
            now = tableau[i][j].getState()
            if i != 0:
                if (tableau[i][j].getEdges()[0] == 1):
                    haut = 1
                else:
                    haut = 0
            if i != len(tableau) -1:
                if (tableau[i][j].getEdges()[2] == 1):
                    bas = 1
                else:
                    bas = 0
            if j != 0:
                if (tableau[i][j].getEdges()[3] == 1):
                    gauche = 1
                else:
                    gauche = 0
            if j != len(tableau[i]) -1:
                if (tableau[i][j].getEdges()[1] == 1):
                    droite = 1
                else:
                    droite = 0
            # print(tableau[0][1].getState())  
            if now != 0:
                if droite:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    (b+1)*longueur_case, a*largeur_case,
                                    fill ='black', width = 2)
                if bas:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    b*longueur_case, (a+1)*largeur_case,
                                    fill ='black', width = 2)
    fenetre.after(1000, updateEverySecond)


#--------------------------------------------------------MAIN----------------------------------------------------------------#
fenetre = tkinter.Tk()
fenetre.title("Affichage du Labyrinthe")
W = 500
H = 400

canva = tkinter.Canvas(fenetre, width = W, height = H)
canva.pack()

robList = []
for i in range(n):
    robList.append(Robot(i))

longueurX = 10
longueurY = 10
tableau = []
for i in range(longueurY):
    tableau.append([Node()])
    for j in range(longueurX-1):
        tableau[i].append(Node())

tableau[0][0].setState(1)
robList[1].move([0,1])
robList[0].move([1,0])

updateEverySecond()
fenetre.mainloop()
            
            
