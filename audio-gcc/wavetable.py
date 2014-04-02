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

if __name__ == "__main__":
    SineWave()
    SquareWave()
    SawtoothWave()
    
