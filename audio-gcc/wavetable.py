from pylab import plot as py_plot
from pylab import show as py_plot_show
from math import sin, pi

TableLength = 1024
Table = [0] * TableLength

def SineWave():
    for n in range(0,TableLength):
        Table[n] = sin((2*pi*n)/TableLength)
    
    py_plot(Table)
    py_plot_show()
    f = open("sine.txt", "w") 
    f.write("= {")
    for num in Table:
        f.write(str(num) + ",\n")
    
    f.write("};")
    #print(Table)
    f.close()      
    

def SquareWave():
    harmonics = 20
    for n in range(0,TableLength):
        Table[n] = sin((2*pi*n)/TableLength)
        for k in range(3, 2*harmonics, 2):
            Table[n] += 1.0/k * sin((2*k*pi*n)/TableLength)
        Table[n] *= 3.3/pi
    
    py_plot(Table)
    py_plot_show()
    f = open("square.txt", "w")
    
    f.write("= {")
    for num in Table:
        f.write(str(num) + ",\n")
    
    f.write("};")
    #print(Table)
    f.close()   
    

def SawtoothWave():
    harmonics = 20
    for n in range(0,TableLength):
        Table[n] = 0
        for k in range(1,harmonics):
            Table[n] -= 1/(k*pi)*sin((2*k*pi*n)/TableLength)
        Table[n] *= 1.7
    
    py_plot(Table)
    py_plot_show()
    f = open("sawtooth.txt", "w")
    
    f.write("= {")
    for num in Table:
        f.write(str(num) + ",\n")
    
    f.write("};")
    #print(Table)
    f.close()


def NoteFrequency():
    notes = 84
    NoteTable = [0]*notes
    NoteTable[0] =   65.4063913251 / 2
    NoteTable[1] =   69.2956577442 / 2
    NoteTable[2] =   73.4161919794 / 2
    NoteTable[3] =   77.7817459305 / 2 
    NoteTable[4] =   82.4068892282 / 2
    NoteTable[5] =   87.3070578583 / 2
    NoteTable[6] =   92.4986056779 / 2
    NoteTable[7] =   97.9988589954 / 2
    NoteTable[8] =  103.8261743950 / 2
    NoteTable[9] =  110.0000000000 / 2 
    NoteTable[10] = 116.5409403795 / 2 
    NoteTable[11] = 123.4708253140 / 2
    
    for m in range(1, notes//12):
        index = m*12
        for n in range(0,12):
            NoteTable[(index+n)] = 2*NoteTable[(index+n)-12] 

    f = open("notefrequency.txt", "w")
    
    f.write("= {")
    for num in NoteTable:
        f.write(str(num) + ",\n")
    
    f.write("};")
    f.close()
        

if __name__ == "__main__":
    #SineWave()
    #SquareWave()
    #SawtoothWave()
    NoteFrequency()
