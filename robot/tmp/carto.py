import tkinter
import time

#--------------------------------------------------------FONCTIONS----------------------------------------------------------------#

n = 2 #nombre de robots

class Node:
    def __init__(self):
        self.state = 0      #state : 0=pas encore visité, 1=déja visité, 2=voisins déja visités
        self.neigh = [-1,-1,-1,-1]
        self.visited = 0
        self.edges = [0,0,0,0] #arrete en  : 0 = up, 1 = right, 2 = down, 3 = left  # Si case=1 : ligneprise, case=3 lignepaprise ,case = 2:pas de ligne, case = 0:pas visité
    def setNeigh (self,n):
        for i in range(4):
            if(n[i] != -1):
                if (self.neigh[i] != 0):
                    self.neigh[i] = n[i]
    def getState(self):
        return self.state
    def getEdges(self):
        return self.edges
    def setState(self, s):
        self.state = s
    def setEdges(self,p): #-1 si on modifie pas
        for i in range(4):
            if((p[i]!=-1) and (self.edges[i]==0)):
                self.edges[i] = p[i]
    def visit(self,dir): #dir : 0 = up, 1 = right, 2 = down, 3 = left
        self.edges[dir] = 1
        self.state = 2
        for i in self.neigh:
            if (i != 0):
                if(i.getState()==0):
                    self.state=1

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
    def move(self, dir):        # [0,0] dir = 1  move en dir 1
        pos=[0,0]
        if(dir == 0):
            pos[0] = (self.presentPos[0]-1)
            pos[1] = self.presentPos[1]
        if(dir == 1):
            pos[0] = self.presentPos[0]
            pos[1] = (self.presentPos[1]+1)         #pos = [0,1]

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
                tableau[pos[0]][pos[1]].visit(0)
            elif (self.lastPos[1]==(pos[1]+1)):
                cdir = 3
                tableau[pos[0]][pos[1]].visit(1)
            elif (self.lastPos[0]==(pos[0]+1)):
                cdir = 0
                tableau[pos[0]][pos[1]].visit(2)
            elif (self.lastPos[1]==(pos[1]-1)):
                cdir = 1
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
            self.direction = dir
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
        availinter[0] = -1
    if(pos[0]==longueurX):
        availinter[2] = -1
    if(pos[1]==0):
        availinter[3] = -1
    if(pos[1]==longueurY):
        availinter[1] = -1
    incr=0    
    for e in tableau[pos[0]][pos[1]].getEdges():
        if (e == 2):
            availinter[incr] = -1
        incr +=1
    return availinter  #[haut, droite, bas, gauche]

def updateEverySecond():
    #------------------------------DECISION----------------------------#  
    
    i=robList[1]
    availinter = recupInfo(i)
    for x in range(4):
        if (availinter[x]==0):
            availinter[x]==2
    pos = i.getPresentPos()
    tableau[pos[0]][pos[1]].setEdges(availinter)
    val = []
    for z in range(4):
        if(availinter[z]!=-1):
            test = tableau[pos[0]][pos[1]]
            if (z==0):
                test = tableau[pos[0]-1][pos[1]]
            if (z==1):
                test = tableau[pos[0]][pos[1]+1]
            if (z==2):
                test = tableau[pos[0]+1][pos[1]]
            if (z==3):
                test = tableau[pos[0]][pos[1]-1]
            if(test.getState() != 2):
                val.append(z)
    dir = -1
    for vp in val:
        test = tableau[pos[0]][pos[1]]
        if (vp==0):
            test = tableau[pos[0]-1][pos[1]]
        if (vp==1):
            test = tableau[pos[0]][pos[1]+1]
        if (vp==2):
            test = tableau[pos[0]+1][pos[1]]
        if (vp==3):
            test = tableau[pos[0]][pos[1]-1]
        print(test.getState())
        if (test.getState() == 0):
            dir = vp
            break
        if (test.getState() == 1):
            dir = vp
    if (dir == -1):
        ouva = i.getOneHistory()
        if (pos[0]==ouva[0]+1):
            dir = 0
        elif (pos[0]==ouva[0]-1):
            dir = 2 
        elif (pos[1]==ouva[1]-1):
            dir = 1 
        elif (pos[1]==ouva[1]+1):
            dir = 3
    i.move(dir)       
    #-----------------------ROBOT 2-------------------------#
    i=robList[0]
    availinter = recupInfo(i)
    for x in range(4):
        if (availinter[x]==0):
            availinter[x]==2
    pos = i.getPresentPos()
    tableau[pos[0]][pos[1]].setEdges(availinter)
    print(tableau[pos[0]][pos[1]+1].getState())
    val = []
    z = 0
    if(availinter[z]!=-1):
        test = tableau[pos[0]][pos[1]]
        if (z==0):
            test = tableau[pos[0]-1][pos[1]]
        if (z==1):
            test = tableau[pos[0]][pos[1]+1]
        if (z==2):
            test = tableau[pos[0]+1][pos[1]]
        if (z==3):
            test = tableau[pos[0]][pos[1]-1]
        if(test.getState() != 2):
            val.append(z)
    for z in range(3, 1, -1):
        if(availinter[z]!=-1):
            if (z==0):
                test = tableau[pos[0]-1][pos[1]]
            if (z==1):
                test = tableau[pos[0]][pos[1]+1]
            if (z==2):
                test = tableau[pos[0]+1][pos[1]]
            if (z==3):
                test = tableau[pos[0]][pos[1]-1]
            if(test.getState() != 2):
                val.append(z)
    dir = -1
    for vp in val:
        test = tableau[pos[0]][pos[1]]
        if (vp==0):
            test = tableau[pos[0]-1][pos[1]]
        if (vp==1):
            test = tableau[pos[0]][pos[1]+1]
        if (vp==2):
            test = tableau[pos[0]+1][pos[1]]
        if (vp==3):
            test = tableau[pos[0]][pos[1]-1]
            
        if (test.getState() == 0):
            dir = vp
            break
        if (test.getState() == 1):
            dir = vp
    if (dir == -1):
        ouva = i.getOneHistory()
        if (pos[0]==ouva[0]+1):
            dir = 0
        elif (pos[0]==ouva[0]-1):
            dir = 2 
        elif (pos[1]==ouva[1]-1):
            dir = 1 
        elif (pos[1]==ouva[1]+1):
            dir = 3
    i.move(dir)           
    
    #------------------------------AFFICHE-----------------------------#    

    
    for i in range(longueurX):
        for j in range(longueurY):
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
                    canva.create_line(j*longueur_case+5, i*largeur_case+5,
                                    (j+1)*longueur_case+5, i*largeur_case+5,
                                    fill ='black', width = 2)
                if droite==2:
                    canva.create_line(j*longueur_case+5, i*largeur_case+5,
                                    (j+1)*longueur_case+5, i*largeur_case+5,
                                    fill ='red', width = 2)
                if gauche==1:
                    canva.create_line(j*longueur_case+5, i*largeur_case+5,
                                    (j-1)*longueur_case+5, i*largeur_case+5,
                                    fill ='black', width = 2)
                if gauche==2:
                    canva.create_line(j*longueur_case+5, i*largeur_case+5,
                                    (j-1)*longueur_case+5, i*largeur_case+5,
                                    fill ='red', width = 2)
                if bas==1:
                    canva.create_line(j*longueur_case+5, i*largeur_case+5,
                                    j*longueur_case+5, (i+1)*largeur_case+5,
                                    fill ='black', width = 2)
                if bas==2:
                    canva.create_line(j*longueur_case+5, i*largeur_case+5,
                                    j*longueur_case+5, (i+1)*largeur_case+5,
                                    fill ='red', width = 2)
                if haut==1:
                    canva.create_line(j*longueur_case+5, i*largeur_case+5,
                                    j*longueur_case+5, (i-1)*largeur_case+5,
                                    fill ='black', width = 2)
                if haut==2:
                    canva.create_line(j*longueur_case+5, i*largeur_case+5,
                                    j*longueur_case+5, (i-1)*largeur_case+5,
                                    fill ='red', width = 2)
    for i in robList:
        canva.delete(cercleList[0])
        cercleList.pop(0)
        p = i.getPresentPos()
        cercleList.append(canva.create_oval((p[1] * longueur_case)-2+5, (p[0] * largeur_case)-2+5, (p[1] * longueur_case)+2+5, (p[0] * largeur_case)+2+5, fill='red'))
    fenetre.after(500, updateEverySecond)


#--------------------------------------------------------MAIN----------------------------------------------------------------#
fenetre = tkinter.Tk()
fenetre.title("Affichage du Labyrinthe")
W = 500
H = 400


longueurX = 10
longueurY = 10

longueur_case = (W//(longueurX))-10
largeur_case = (H//(longueurY))-10

canva = tkinter.Canvas(fenetre, width = W, height = H)
canva.pack()

cercleList = [0,0]
robList = []
for i in range(n):
    robList.append(Robot(i))


tableau = []
for i in range(longueurY+1):
    tableau.append([Node()])
    for j in range(longueurX):
        tableau[i].append(Node())

tableau[0][0].setState(2)
tableau[0][0].setEdges([2,0,0,2])
robList[1].setDir(1)
robList[0].setDir(2)

for i in range(longueurY):
    for j in range(longueurX):
        if (i==0):
            tableau[i][j].setNeigh([0,-1,-1,-1])
            tableau[i][j].setEdges([2,-1,-1,-1])
        if (j==0):
            tableau[i][j].setNeigh([-1,-1,-1,0])
            tableau[i][j].setEdges([-1,-1,-1,2])
        if (j==longueurX-1):
            tableau[i][j].setNeigh([-1,0,-1,-1])
            tableau[i][j].setEdges([-1,2,-1,-1])
        if (i==longueurY-1):
            tableau[i][j].setNeigh([-1,-1,0,-1])
            tableau[i][j].setEdges([-1,-1,2,-1])
        tableau[i][j].setNeigh([tableau[i-1][j],tableau[i][j+1],tableau[i+1][j],tableau[i][j-1]])

robList[1].move(1)
robList[0].move(2)


updateEverySecond()
fenetre.mainloop()
            
            
