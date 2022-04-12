import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.pyplot import MultipleLocator

data1 = np.loadtxt("baseline_zfp_ratio.txt")
data2 = np.loadtxt("zmesh_zfp_ratio.txt")
data3 = np.loadtxt("lamp_zfp_ratio.txt")


#list1 = ([4.033474,4.002399,4.589420,6.653342,6.549865,3.751759,3.660729,4.201679,3.913578,12.089611,4.847482,4.310263,4.980873,10.055283,3.860097,7.354801,6.336699,10.755272])
list1 = np.array(data1)
list2 = np.array(data2)
list3 = np.array(data3)

length = len(list1)
x = np.arange(length)

barWidth = 0.25
namelist = ["M1", "M2", "M3", "M4", "M5", "M6", "P1", "P2", "P3", "P4", "P5", "P6","G1", "G2", "G3", "G4", "G5", "G6"]
fig = plt.figure(figsize = (16,4))
total_width, n = 0.8, 2
width = total_width / n
x1 = x 
x2 = x + barWidth
x3 = x + barWidth*2
axis_font = {'size':'18'}
plt.rc('xtick', labelsize = 18)
plt.rc('ytick', labelsize = 18)
font = {'size':'32'}
ax = fig.add_subplot(111)
ax.set_xticks(x+width)
ax.set_xlim([-0.5,18])
#plt.title("SZ compress ratio")
plt.ylabel("Compression ratio", fontsize = 24)
plt.bar(x1, list1, width = barWidth, label = "Baseline", color='#130074')
plt.bar(x2, list2, width = barWidth, label = "zMesh", color='#CB181B')
plt.bar(x3, list3, width = barWidth, label = "LAMP", color='#008B45')

plt.xticks(x+0.25, namelist)
plt.legend(fontsize = 18, loc = 'upper left')
#plt.show()
plt.savefig('total_ZFP_ratio.pdf',format='pdf',bbox_inches="tight",pad_inches=0)





