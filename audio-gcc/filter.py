from math import sin, pi
from pylab import plot as py_plot
from pylab import show as py_plot_show

Table = [0.0] * 1024 * 2
TableLength = len(Table) / 2
def SquareWave():
    harmonics = 20
    for n in range(0,len(Table)):
        Table[n] = sin((2*pi*n)/TableLength)
        for k in range(3, 2*harmonics, 2):
            Table[n] += 1.0/k * sin((2*k*pi*n)/TableLength)
        Table[n] *= 3.3/pi
    
    py_plot(Table)
    py_plot_show()


if __name__ == "__main__":

    SquareWave()
    
    fCutoff = pi * 1000 / 48000
    fDamping = 1.0
    fLow = 0.0
    fHigh = 0.0
    fBand = 0.0
    fDelay = [0.0, 0.0]

    for n in range(0,len(Table)):
        
        fInput = Table[n]
        
        fLow = fDelay[1] + fCutoff * fDelay[0]
        fHigh = fInput - fLow - fDamping * fDelay[0]
        fBand = fCutoff * fHigh + fDelay[0]
        fDelay[0] = fBand;
        fDelay[1] = fLow;

        Table[n] = fBand

    py_plot(Table)
    py_plot_show()
