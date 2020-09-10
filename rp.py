import random
import getopt, sys
import numpy as np
from bridson import poisson_disc_samples



def write_points(xPoints, yPoints):
    largo =len(xPoints)
    f = open('data.dat', 'w')
    f.write("2 rbox {} D2 z\n".format(largo))
    f.write('{}\n'.format(largo))
    for i in range(0,largo):
        f.write('{0} {1}\n'.format(xPoints[i], yPoints[i]))

    f.close()

def generate_random_points(nPoints, xPoints, yPoints, dist = 0):

    uwu = nPoints*10

    for _ in range(0,nPoints-4):
        x = random.randrange(-1*uwu, uwu)
        y = random.randrange(-1*uwu, uwu)
        xPoints.append(int(x))
        yPoints.append(int(y))

    xPoints.append(-1*(uwu + dist))
    yPoints.append(-1*(uwu + dist))

    xPoints.append(uwu + dist)
    yPoints.append(uwu + dist)

    xPoints.append(-1*(uwu + dist))
    yPoints.append(uwu+ dist)

    xPoints.append(uwu+ dist)
    yPoints.append(-1*(uwu + dist))

def generate_bridson_points(nPoints, xPoints, yPoints, dist = 0):
    
    lst = poisson_disc_samples(width=nPoints, height=nPoints, r=10)    
    xPoints.extend(x[0] for x in lst)
    yPoints.extend(x[1] for x in lst)
    
    xPoints.append(0 - dist)
    yPoints.append(0 - dist)

    xPoints.append(nPoints + dist)
    yPoints.append(nPoints + dist)

    xPoints.append(0 - dist)
    yPoints.append(nPoints + dist)

    xPoints.append(nPoints + dist)
    yPoints.append(0 - dist)
    
def generate_uniform_points(nPoints, xPoints, yPoints, dist = 0):
    
    for i in range(int(np.sqrt(nPoints))):
        for j in range(int(np.sqrt(nPoints))):
            xPoints.append(j)
            yPoints.append(i)   

def generate_semi_uniform_points(nPoints, xPoints, yPoints, dist = 0):
    uwu = int(np.sqrt(nPoints))
    for i in range(uwu):
        for j in range(uwu):
            xPoints.append(j + random.uniform(-dist, dist))
            yPoints.append(i + random.uniform(-dist, dist))    
    #xPoints.append(0 - dist)
    #yPoints.append(0 - dist)
#
    #xPoints.append(uwu + dist)
    #yPoints.append(uwu + dist)
#
    #xPoints.append(0 - dist)
    #yPoints.append(uwu + dist)
#
    #xPoints.append(uwu + dist)
    #yPoints.append(0 - dist)


def main():
    full_cmd_arguments = sys.argv
    argument_list = full_cmd_arguments[1:]

    if(len(argument_list) == 2):
        nPoints = int(argument_list[0])
        selection = int(argument_list[1])
        dist = 0
    else:
        nPoints = int(argument_list[0])
        selection = int(argument_list[1])
        dist = float(argument_list[2])

    random.seed(138)
    xPoints = []
    yPoints = []

    #dist = nPoints*0.7
    
    if(selection == 0):
        generate_random_points(nPoints, xPoints, yPoints, dist )
    elif selection == 1:
        generate_bridson_points(nPoints, xPoints, yPoints, dist)
    elif selection == 2:
        generate_semi_uniform_points(nPoints, xPoints, yPoints, dist)

        

    write_points(xPoints, yPoints)
    

if __name__ == "__main__":
    main()
