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

def generate_random_points(nPoints, xPoints, yPoints, dist):

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

def generate_bridson_points(nPoints, xPoints, yPoints, dist):
    
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
    


def main():
    full_cmd_arguments = sys.argv
    argument_list = full_cmd_arguments[1:]
    nPoints = int(argument_list[0])
    selection = int(argument_list[1])
    random.seed(138)
    xPoints = []
    yPoints = []

    #dist = nPoints*0.7
    dist = 0
    if(selection == 0):
        generate_random_points(nPoints, xPoints, yPoints, dist )
    else:
        generate_bridson_points(nPoints, xPoints, yPoints, dist)

    write_points(xPoints, yPoints)
    

if __name__ == "__main__":
    main()
