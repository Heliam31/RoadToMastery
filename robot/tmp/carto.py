import tkinter

#--------------------------------------------------------FONCTIONS----------------------------------------------------------------#

n = 2 #nombre de robots

class Node:
    def __init__(self):
        self.state = 0      #state : 0=pas encore visité, 1=déja visité, 2=voisins déja visités
        self.edges = [0,0,0,0] #arrete en  : 0 = up, 1 = right, 2 = down, 3 = left  # Si case=1 : ligne, case = 2:pas de ligne, case = 0:pas visité
    def getState(self):
        return self.state
    def getEdges(self):
        return self.edges
    def setState(self, s):
        self.state = s
    def setEdges(self,p): #-1 si on modifie pas
        for i in range(4):
            if(p[i]!=-1):
                self.edges[i] = p[i]
    def visit(self,dir): #dir : 0 = up, 1 = right, 2 = down, 3 = left
        self.edges[dir] = 1
        self.state = 2
        for i in self.edges:
            if (i == 0):
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
        self.relativeDirection = 0  # Direction a prendre sur la réponse PC->Robot     0 = tout droit, 1 = right, 2 = demi tour, 3 = left   
        self.direction = 0   #Direction sur le tableau  0 = up, 1 = right, 2 = down, 3 = left
    def getLastPos(self):
        return self.lastPos
    def getPresentPos(self):
        return self.presentPos
    def getOneHistory (self):
        rep = self.history.pop(-1)
        return rep
    def getDir (self):
        return self.direction
    def setDir (self,d):
        self.direction = d
    def move(self, dir): 
        pos=[0,0]
        if(dir == 0):
            pos[0] = (self.presentPos[0]-1)
            pos[1] = self.presentPos[1]
        if(dir == 1):
            pos[0] = self.presentPos[0]
            pos[1] = (self.presentPos[1]+1)
        if(dir == 2):
            pos[0] = (self.presentPos[0]+1)
            pos[1] = self.presentPos[1]
        if(dir == 3):
            pos[0] = self.presentPos[0]
            pos[1] = (self.presentPos[1]-1)
        
        if(valid(pos)):
            self.lastPos = self.presentPos
            self.presentPos = pos
            self.history.append(self.lastPos)
            cdir = 0
            if (self.lastPos[0]==(pos[0]-1)):
                cdir = 2
                tableau[self.lastPos[0]][self.lastPos[1]].visit(cdir)
                tableau[pos[0]][pos[1]].visit(0)
            elif (self.lastPos[1]==(pos[1]+1)):
                cdir = 3
                tableau[self.lastPos[0]][self.lastPos[1]].visit(cdir)
                tableau[pos[0]][pos[1]].visit(1)
            elif (self.lastPos[0]==(pos[0]+1)):
                cdir = 0
                tableau[self.lastPos[0]][self.lastPos[1]].visit(cdir)
                tableau[pos[0]][pos[1]].visit(2)
            elif (self.lastPos[1]==(pos[1]-1)):
                cdir = 1
                tableau[self.lastPos[0]][self.lastPos[1]].visit(cdir)
                tableau[pos[0]][pos[1]].visit(3)
            
            if (self.direction==0):
                self.relativeDirection = cdir
            elif (self.direction==1):
                if (cdir == 0):
                    self.relativeDirection = 3
                elif (cdir == 1):
                    self.relativeDirection = 0
                elif (cdir==2):
                    self.relativeDirection = 1
                elif (cdir == 3):
                    self.relativeDirection = 2
            elif (self.direction==2):
                if (cdir == 0):
                    self.relativeDirection = 2
                elif (cdir == 1):
                    self.relativeDirection = 3
                elif (cdir==2):
                    self.relativeDirection = 0
                elif (cdir == 3):
                    self.relativeDirection = 1
            elif (self.direction==3):
                if (cdir == 0):
                    self.relativeDirection = 1
                elif (cdir == 1):
                    self.relativeDirection = 2
                elif (cdir==2):
                    self.relativeDirection = 3
                elif (cdir == 3):
                    self.relativeDirection = 0
            self.direction = cdir
            return 1
        else:
            return 0

def recupInfo(i):
    #lecture csv recherche message du robot i
    #direction disponibles
    availinter=[1,1,1] #gauche, avant, droite 
    pos = i.getPresentPos()
    
    if (i.direction==0):
        availinter = [availinter[1],availinter[2],1,availinter[0]]
    elif (i.direction==1):
        availinter = [availinter[0],availinter[1],availinter[2],1]
    elif (i.direction==2):
        availinter = [1,availinter[0],availinter[1],availinter[2]]
    elif (i.direction==3):
        availinter = [availinter[2],1,availinter[0],availinter[1]]
    if(pos[0]==0):
        availinter[3] = -1
    if(pos[0]==longueurY):
        availinter[2] = -1
    if(pos[1]==0):
        availinter[0] = -1
    if(pos[1]==longueurX):
        availinter[1] = -1
    return availinter  #[haut, droite, bas, gauche]

def updateEverySecond():
    #------------------------------DECISION----------------------------#  
    
    for i in robList:
        availinter = recupInfo(i)
        for x in range(4):
            if (availinter[x]==0):
                availinter[x]==2
        pos = i.getPresentPos()
        tableau[pos[0]][pos[1]].setEdges(availinter)
        val = []
        for x in range(4):
            test = tableau[pos[0]][pos[1]]
            if (x==0):
                test = tableau[pos[0]-1][pos[1]]
            if (x==1):
                test = tableau[pos[0]][pos[1]+1]
            if (x==2):
                test = tableau[pos[0]+1][pos[1]]
            if (x==3):
                test = tableau[pos[0]][pos[1]-1]
            oui = test.getState()
            if(oui != 2):
                val.append(x)
        dir = 0
        for vp in val:
            if (vp == 0):
                dir = vp
                break
        
        i.move(dir)       
        
    
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
                elif (tableau[i][j].getEdges()[0] == 2):
                    haut = 2
                else:
                    haut = 0
            if i != len(tableau) -1:
                if (tableau[i][j].getEdges()[2] == 1):
                    bas = 1
                elif (tableau[i][j].getEdges()[2] == 2):
                    bas = 2
                else:
                    bas = 0
            if j != 0:
                if (tableau[i][j].getEdges()[3] == 1):
                    gauche = 1
                elif (tableau[i][j].getEdges()[3] == 2):
                    gauche = 2
                else:
                    gauche = 0
            if j != len(tableau[i]) -1:
                if (tableau[i][j].getEdges()[1] == 1):
                    droite = 1
                elif (tableau[i][j].getEdges()[1] == 2):
                    droite = 2
                else:
                    droite = 0
            # print(tableau[0][1].getState())  
            if now != 0:
                if droite==1:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    (b+1)*longueur_case, a*largeur_case,
                                    fill ='black', width = 2)
                if droite==2:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    (b+1)*longueur_case, a*largeur_case,
                                    fill ='red', width = 2)
                if gauche==1:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    (b-1)*longueur_case, a*largeur_case,
                                    fill ='black', width = 2)
                if gauche==2:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    (b-1)*longueur_case, a*largeur_case,
                                    fill ='red', width = 2)
                if bas==1:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    b*longueur_case, (a+1)*largeur_case,
                                    fill ='black', width = 2)
                if bas==2:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    b*longueur_case, (a+1)*largeur_case,
                                    fill ='red', width = 2)
                if haut==1:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    b*longueur_case, (a-1)*largeur_case,
                                    fill ='black', width = 2)
                if haut==2:
                    canva.create_line(b*longueur_case, a*largeur_case,
                                    b*longueur_case, (a-1)*largeur_case,
                                    fill ='red', width = 2)
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

tableau[0][0].setState(2)
robList[1].setDir(1)
robList[0].setDir(2)

tableau[0][longueurX-1].setEdges([2,2,0,0])
tableau[longueurY-1][0].setEdges([0,0,2,2])
tableau[longueurY-1][longueurX-1].setEdges([0,2,2,0])

robList[1].move(1)
robList[0].move(2)

updateEverySecond()
fenetre.mainloop()
            
            
