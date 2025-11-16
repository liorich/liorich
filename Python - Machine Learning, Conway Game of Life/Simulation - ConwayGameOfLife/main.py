"""
# Lior Yechiely
# Introduction to Computer Science - Assignement #2.

In loving memory of John Horton Conway.

The following is a class that simulates a zero-player game session of Conway's Game Of Life.
It recieves the size of the board, the starting mode and rules.
Moreover if an RLE is present, it will ignore the starting mode and translate the RLE code into the matrix of choice.

It must be noted that the board game is a periodical space, so an input of an RLE close to the borders will loop around the board.
"""

import game_of_life_interface
import numpy as np
import matplotlib.pyplot as plt

def copydouble(mylist: list):
    # This function correctly copies a matrix-like list, since you cannot simply use list1=list2.
    a=mylist.copy()
    for i in range(len(mylist)):
        a[i]=mylist[i].copy()
    return a

class GameOfLife(game_of_life_interface.GameOfLife):  # note for myself: This is the way you construct a class that inherits properties
    def __init__(self, size_of_board, board_start_mode, rules, rle, pattern_position):
        """
        Creates the game.
        :param size_of_board: Integer.
        :param board_start_mode: Integer. Decides which method to draw the board with.
        :param rules: String, written "B(numbers)/S(numbers)" where the numbers after B is the reincarnation criteria,
        and the numbers after S is the survival criteria.
        Example: The classic set, B3/S23:
        If a dead cell has 3 living neighbors around, it will reincarnate.
        If a living cell has 2 living neighbors around, it will survive the iteration. Dies otherwise.
        :param rle: a string, that if present, will be used to draw the matrix using transform_rle_to_matrix.
        :param pattern_position: the upper-left from which the RLE will translate to the matrix.
        self.mat = holds the board.
        """
        self.SOB = int(size_of_board)
        self.BSM = int(board_start_mode)
        self.rules = rules
        self.rle = rle
        self.PP = [None,None]
        if(type(pattern_position) == tuple and len(pattern_position)>0):
            self.PP[0] = pattern_position[0]
            self.PP[1] = pattern_position[1]
        self.mat = np.zeros((self.SOB,self.SOB)).tolist()

        # Create the board.
        """
        Start 1: 50% chance of a cell being alive or dead.
        Start 2: 80% chance of a cell being alive.
        Start 3: 20% chance of a cell being alive.
        Start 4: Gosper glider gun.
        """
        if(self.rle == ""):
            if (self.BSM == 1):
                for i in range(self.SOB):
                    for j in range(self.SOB): # Assuming the board is always square.
                        self.mat[i][j] = np.random.choice(np.arange(1, 3), p=[0.51, 0.49])
                        if self.mat[i][j] == 1:
                            self.mat[i][j] = 0
                        else:
                            self.mat[i][j] = 255
            elif (self.BSM == 2):
                for i in range(self.SOB):
                    for j in range(self.SOB):
                        self.mat[i][j] = np.random.choice(np.arange(1, 3), p=[0.2, 0.8])
                        if self.mat[i][j] == 1:
                            self.mat[i][j] = 0
                        else:
                            self.mat[i][j] = 255
            elif (self.BSM == 3):
                for i in range(self.SOB):
                    for j in range(self.SOB):
                        self.mat[i][j] = np.random.choice(np.arange(1, 3), p=[0.2, 0.8])
                        if self.mat[i][j] == 1:
                            self.mat[i][j] = 255
                        else:
                            self.mat[i][j] = 0
            elif (self.BSM == 4): # Gosper glider gun.
                self.PP[0]=10
                self.PP[1]=10
                self.mat=self.transform_rle_to_matrix("24bo11b$22bobo11b$12boo6boo12boo$11bo3bo4boo12boo$oo8bo5bo3boo14b$oo8bo3boboo4bobo11b$10bo5bo7bo11b$11bo3bo20b$12boo22b!")
        else: self.mat=self.transform_rle_to_matrix(self.rle)

        # Read the rules.
        a = self.rules
        self.B = [] # Reborn criteria
        self.S = [] # Survive criteria
        for i in range(len(a)):
            if (a[i] == 'B'):
                n = 1
                while (True):
                    if ((i + n >= len(a))): break
                    if (a[i + n].isnumeric()):
                        self.B.append(int(a[i + n]))
                        n += 1
                    else:
                        break
            if (a[i] == 'S'):
                n = 1
                while (True):
                    if ((i + n >= len(a))): break
                    if (a[i + n].isnumeric()):
                        self.S.append(int(a[i + n]))
                        n += 1
                    else:
                        break

    def update(self):
        """
        Performs a single iteration of the game according to the given rules.
        :return The updated board.
        """
        t1 = 0
        t2 = 0  # t1 and t2 are placeholders that avoid exceeding the list's range and looping around instead.
        b=copydouble(self.mat)
        bsave = copydouble(b) # I make two temporary lists to work with.
        sum = 0  # sums how many neighbors are "alive".
        for i in range(len(b)):
            for j in range(len(b)):  # i and j are used to move along the entire list, while bsave saves the original list.
                # print('cellcheck: {}-{}'.format(j, i)) # (prints were used for testing)
                for k in range(-1, 2, 1):
                    for n in range(-1, 2,1):  # k and n are used to move along the neighbors of cell i-j. if they are out of range, they correct themselves.
                        t1 = i + k
                        t2 = j + n
                        if (t1 >= len(b)): t1 = k - 1
                        if (t2 >= len(b)): t2 = n - 1
                        # print('checking:{},{}'.format(t2,t1))
                        if (bsave[t1][t2] == 255 and not (k == 0 and n == 0)):  # check the cells, except the cell itself
                            sum += 1
                # print("sum {}, cell {}-{}".format(sum,i,j))
                if (bsave[i][j] == 0):
                    if (sum in self.B): b[i][j] = 255 # Dead cell in reborn criteria
                elif (bsave[i][j] == 255 and not(sum in self.S)): # Live cell NOT in survive criteria
                    b[i][j] = 0
                else: pass # Else, do nothing. (Don't really need this line)
                sum = 0
        self.mat=copydouble(b)
        return self.mat

    def save_board_to_file(self, file_name):
        """
        :param file_name: name for the file.
        :return saves the current game matrix onto a file.
        """
        plt.imshow(self.mat)
        plt.imsave(file_name,self.mat)

    def display_board(self):
        # Display the current board.
        plt.imshow(self.mat)
        plt.show()

    def return_board(self):
        # Return the current board.
        return self.mat

    def transform_rle_to_matrix(self, rle):
        """ This method transforms an rle coded pattern to a two dimensional list that holds the pattern,
         Dead will be donated with 0 while alive will be donated with 255.
        Input an rle coded string.
        Output a two dimensional list that holds a pattern with a size of the bounding box of the pattern.
        """
        #rle = input('Input RLE: ')
        #print(len(rle))
        a=copydouble(self.mat)
        rlesplit = rle.split('$')
        #print(rlesplit)
        flag = False
        x = self.PP[0]
        y = self.PP[1]
        #i_m and j_m tell me the current coordinates of the matrix.
        i_m = x
        j_m = y
        num = 0
        for i in range(len(rlesplit)):
            for j in range(len(rlesplit[i])):
                # case 1: b(dead) or o(alive)
                if (i_m >= self.SOB): i_m -= self.SOB #loop around vertically.
                if (rlesplit[i][j] == 'b' and flag == False):
                    # Here, the exceptions tell the program to loop around horizontally!
                    # If the matrix index j_m is outside of the range, an exception occurs.
                    try:
                        a[i_m][j_m] = 0
                        j_m += 1
                    except:
                        j_m -= self.SOB
                        a[i_m][j_m] = 0
                        j_m += 1
                elif (rlesplit[i][j] == 'o' and flag == False):
                    try:
                        a[i_m][j_m] = 255
                        j_m += 1
                    except:
                        j_m -= self.SOB
                        a[i_m][j_m] = 255
                        j_m += 1

                elif (flag == True):  # related to case 2
                    flag = False
                    continue
                elif (rlesplit[i][j].isnumeric()):  # case 2: numeric followed by b o $
                    try:
                        if (rlesplit[i][j + 1] == 'b'):  # NUM amount of dead cells
                            num += int(rlesplit[i][j])
                            while (num > 0):
                                try:
                                    a[i_m][j_m] = 0
                                    j_m += 1
                                    num -= 1
                                except:
                                    j_m -= self.SOB
                                    a[i_m][j_m] = 0
                                    j_m += 1
                                    num -= 1
                            num = 0
                            flag = True  # Raise a flag to skip the character
                        elif (rlesplit[i][j + 1] == 'o'):  # NUM amount of living cells
                            num += int(rlesplit[i][j])
                            while (num > 0):
                                try:
                                    a[i_m][j_m] = 255
                                    j_m += 1
                                    num -= 1
                                except:
                                    j_m -= self.SOB
                                    a[i_m][j_m] = 255
                                    j_m += 1
                                    num -= 1
                            num = 0
                            flag = True  # Raise a flag to skip the character
                        elif (rlesplit[i][j + 1].isnumeric()):  # Add more than 9 indexes (This can actually account for numbers no matter how big)
                            num = int(rlesplit[i][j]) * 10
                    except:  # end of line
                        i_m += int(rlesplit[i][j]) - 1
                        j_m = y
                        num = 0
                # case 3: '!' encountered
                if (rlesplit[i][j] == '!'): break
            if (rlesplit[i][j] == '!'): break
            i_m += 1
            j_m = y
        self.mat=copydouble(a)
        return self.mat


if __name__ == '__main__':  # You should keep this line for our auto-grading code.
    print('write your tests here')  # don't forget to indent your code here!
    Game=GameOfLife(100,1,'B3/S23',"","")
    Game.display_board()
    for i in range(2000):
        if(i%100):
            Game.display_board()
        Game.update()
    Game.display_board()
