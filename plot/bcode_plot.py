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

vals =np.loadtxt(sys.argv[1], comments="#", delimiter=",", unpack=False) 

plt.figure(num=None,figsize=(3.5,3))
plt.rc('xtick', labelsize=12)          # fontsize of the tick labels
plt.rc('ytick', labelsize=12)

axis_font = {'size':'14'}
axes = plt.gca()
#axes.spines['right'].set_visible(True)
plt.xlabel('Bcode value', **axis_font)
plt.ylabel('Frequency',**axis_font )

n, bins, patches=plt.hist(vals,bins=[0,1,2,3, 4,5,6,7,8],density=True,stacked=True, histtype='bar', label='png', color='black')

plt.tight_layout()
plt.savefig("bcode.pdf", format='pdf',bbox_inches="tight",pad_inches=0)
#plt.show()
