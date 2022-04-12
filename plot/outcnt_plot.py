import numpy as np
import matplotlib.pyplot as plt
from matplotlib import mlab
import sys
import array
import os
import gc


if len(sys.argv) < 1 :
    print ('usage: python %s <file.dat' % sys.argv[0])
    sys.exit(0)

print(sys.argv[1])

vals =np.loadtxt(sys.argv[2], comments="#", delimiter=",", unpack=False) 
vals1 =np.loadtxt(sys.argv[3], comments="#", delimiter=",", unpack=False) 


print(vals)

x=range(0,len(vals))

x1=0;
x2=int(len(vals));
plt.figure(num=None,figsize=(7.5,3))
plt.rc('xtick', labelsize=12)          # fontsize of the tick labels
plt.rc('ytick', labelsize=12)
#plt.ylim(-10000,10000)   
#plt.xticks(np.arange(0,260000,50000))
#plt.yticks(np.linspace(y1,y2,5));
#plt.xticks(np.linspace(x1,144,4));    #Green
#plt.xticks(np.arange(2))              #Min
plt.xticks(np.arange(6))              #Pin

axis_font = {'size':'14'}
axes = plt.gca()

plt.xlabel('Segment ID', **axis_font)
plt.ylabel('Bytes of output',**axis_font )

plt.plot(x,vals,'b-',label="Baseline");

plt.plot(x,vals1,'r+',label="LAMP")

plt.legend(loc=1,ncol=4, bbox_to_anchor=(.8, 1.35),prop={'size': 12} )

plt.tight_layout()
plt.savefig("outcnt.pdf", format='pdf',bbox_inches="tight",pad_inches=0)
#plt.show()
