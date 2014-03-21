from math import sin, pi
TableLength = 1024
Table = [0] * TableLength

def SquareWave():
    for n in range(0,TableLength):
        Table[n] = sin((2*pi*n)/TableLength)
        Table[n] += 1.0/3.00 * sin((06.0*pi*n)/TableLength)
        Table[n] += 1.0/5.00 * sin((10.0*pi*n)/TableLength)
        Table[n] += 1.0/7.00 * sin((14.0*pi*n)/TableLength)
        Table[n] += 1.0/9.00 * sin((18.0*pi*n)/TableLength)
        Table[n] += 1.0/11.00 * sin((22.0*pi*n)/TableLength)
        Table[n] *= 2.0/pi
    
    print(Table)

def SawtoothWave():
    for n in range(0,TableLength):
        Table[n] = 0
        Table[n] -= 1.000/pi*sin((2*pi*n)/TableLength)
        Table[n] -= 0.500/pi*sin((4*pi*n)/TableLength)
        Table[n] -= 0.333/pi*sin((6*pi*n)/TableLength)
        Table[n] -= 0.250/pi*sin((8*pi*n)/TableLength)
        Table[n] -= 0.200/pi*sin((10*pi*n)/TableLength)
        Table[n] -= 0.166/pi*sin((12*pi*n)/TableLength)
        Table[n] -= 0.142/pi*sin((14*pi*n)/TableLength)
        Table[n] -= 0.125/pi*sin((16*pi*n)/TableLength)
        Table[n] -= 0.111/pi*sin((18*pi*n)/TableLength)
        Table[n] *= 0.9
        
    print(Table)
