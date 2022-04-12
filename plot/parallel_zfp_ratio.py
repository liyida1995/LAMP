import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.pyplot import MultipleLocator

data1 = np.loadtxt("zfp_baseline.txt")
list1 = np.array(data1)
data2 = np.loadtxt("zfp_lamp.txt")
list2 = np.array(data2)


length = len(list1)
x = np.arange(length)


namelist = ["1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1K", "2K"]
fig = plt.figure(figsize = (16,4))
total_width, n = 1, 4
width = total_width / n
x1 = x - width / 2
x2 = x + width / 2
axis_font = {'size':'18'}
plt.rc('xtick', labelsize = 18)
plt.rc('ytick', labelsize = 18)
font = {'size':'32'}
ax = fig.add_subplot(111)
ax.set_xticks(x+width)
ax.set_xlim([-0.5,11.5])
ax.set_ylim([0,20])
#plt.title("SZ compress ratio")
plt.ylabel("Compression ratio", fontsize = 24)
plt.xlabel("Number of parallel processors", fontsize = 24)
plt.bar(x1, list1, width = width, label = "Baseline")
plt.bar(x2, list2, width = width, label = "LAMP")

plt.xticks(x, namelist)
#plt.legend(fontsize = 18,loc='upper left')
plt.legend(fontsize = 18,loc='upper left',ncol=2)
#plt.show()
plt.savefig('zfp_parallel.pdf',format='pdf',bbox_inches="tight",pad_inches=0)
